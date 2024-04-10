#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <deque>
#include <functional>
#include <atomic>
#include <chrono>
#include <cmath>
#include <cpr/cpr.h>
#include <nlohmann/json.hpp>

enum class EventType { INVOKE, NOTHING, ERROR };

template<typename T>
class SafeQueue {
private:
    std::queue<T> queue;
    mutable std::mutex mtx;
    std::condition_variable cond;

public:
    void enqueue(T value) {
        {
            std::lock_guard<std::mutex> lock(mtx);
            queue.push(std::move(value));
        }
        cond.notify_one();
    }

    T dequeue() {
        std::unique_lock<std::mutex> lock(mtx);
        cond.wait(lock, [this] { return !queue.empty(); });
        T value = std::move(queue.front());
        queue.pop();
        return value;
    }

    bool empty() const {
        std::lock_guard<std::mutex> lock(mtx);
        return queue.empty();
    }
};

struct RequestResult {
    nlohmann::json response;
    std::chrono::steady_clock::time_point timestamp;
    std::chrono::milliseconds latency;
};

class ThreadPool {
private:
    std::vector<std::thread> workers;
    SafeQueue<std::function<void()>> tasks;
    std::atomic<bool> stop{false};

    void worker() {
        while (true) {
            auto task = tasks.dequeue();
            if (!task) break; // Check for a null task
            task();
        }
    }

public:
    ThreadPool(size_t threads) {
        for (size_t i = 0; i < threads; ++i) {
            workers.emplace_back([this] { worker(); });
        }
    }

    ~ThreadPool() {
        stop = true;
        // Add a null task for each worker thread to allow them to exit
        for (size_t i = 0; i < workers.size(); ++i) {
            tasks.enqueue(nullptr);
        }
        for (auto& worker : workers) {
            if(worker.joinable()) worker.join();
        }
    }

    void enqueue(std::function<void()> task) {
        if (!stop) {
            tasks.enqueue(std::move(task));
        }
    }
};

class EventHandler {
private:
    ThreadPool pool;
    SafeQueue<RequestResult> results;
    std::chrono::seconds timeWindow;
    mutable std::mutex latencyMtx;
    std::deque<RequestResult> latencyWindow;
    long long sumLatencies{0};
    long long sumSquaredLatencies{0};

    void performRPCRequest() {
        auto start = std::chrono::steady_clock::now();
        std::string url = "https://api.mainnet-beta.solana.com";
        nlohmann::json jsonPayload = {
            {"jsonrpc", "2.0"},
            {"id", 1},
            {"method", "getBalance"},
            {"params", {"GPNmU6ShynWA6zoDLbb5uN1gbPqFgp1eB2pDSCnZaeo6"}}
        };

        auto response = cpr::Post(cpr::Url{url}, cpr::Body{jsonPayload.dump()}, cpr::Header{{"Content-Type", "application/json"}});
        auto jsonResponse = nlohmann::json::parse(response.text);
        auto end = std::chrono::steady_clock::now();
        std::chrono::milliseconds latency = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

        RequestResult result{jsonResponse, end, latency};
        results.enqueue(result);
        updateLatencyStats(result);
    }

    void updateLatencyStats(const RequestResult& result) {
        std::lock_guard<std::mutex> lock(latencyMtx);
        
        // Add new result
        latencyWindow.push_back(result);
        sumLatencies += result.latency.count();
        sumSquaredLatencies += std::pow(result.latency.count(), 2);

        // Remove old results outside the time window
        auto now = std::chrono::steady_clock::now();
        while (!latencyWindow.empty() && (now - latencyWindow.front().timestamp) > timeWindow) {
            sumLatencies -= latencyWindow.front().latency.count();
            sumSquaredLatencies -= std::pow(latencyWindow.front().latency.count(), 2);
            latencyWindow.pop_front();
        }
    }

public:
    EventHandler(size_t threads, std::chrono::seconds win) : pool(threads), timeWindow(win) {}

    void handle_event(EventType event) {
        if(event == EventType::INVOKE) {
            pool.enqueue([this] { performRPCRequest(); });
        } else if(event == EventType::ERROR) {
            std::cerr << "ERROR event encountered." << std::endl;
        }
        // No action for EventType::NOTHING
    }

    RequestResult getOldestResult() {
        return results.dequeue();
    }

    bool resultsEmpty() const {
        return results.empty();
    }

    double getLatencyStdDev() {
        std::lock_guard<std::mutex> lock(latencyMtx);
        if (latencyWindow.empty()) return 0.0;
        double mean = static_cast<double>(sumLatencies) / latencyWindow.size();
        double meanSq = static_cast<double>(sumSquaredLatencies) / latencyWindow.size();
        return std::sqrt(meanSq - std::pow(mean, 2));
    }
};

int main() {
    EventHandler handler(4, std::chrono::seconds(30)); // EventHandler with a pool of 4 threads and a 30-second window for latency tracking

    // Simulate receiving multiple events
    handler.handle_event(EventType::INVOKE);
    handler.handle_event(EventType::ERROR);
    handler.handle_event(EventType::NOTHING);
    handler.handle_event(EventType::INVOKE);

    // Allow time for all tasks to complete
    std::this_thread::sleep_for(std::chrono::seconds(1));

    // Get and print the oldest result
    if (!handler.resultsEmpty()) {
        auto oldestResult = handler.getOldestResult();
        std::cout << "Oldest Response: " << oldestResult.response.dump(4) 
                  << ", Latency: " << oldestResult.latency.count() << "ms" << std::endl;
    }

    // Print standard deviation of latencies
    std::cout << "Standard Deviation of Latencies: " << handler.getLatencyStdDev() << "ms" << std::endl;

    return 0;
}


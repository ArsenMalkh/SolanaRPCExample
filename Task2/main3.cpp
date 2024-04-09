#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <functional>
#include <atomic>
#include <chrono>
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
        std::lock_guard<std::mutex> lock(mtx);
        queue.push(std::move(value));
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

// Custom type for holding request results along with their metadata.
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
        while (!stop) {
            auto task = tasks.dequeue();
            if (task) task();
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
        for (auto& worker : workers) {
            if(worker.joinable()) worker.join();
        }
    }

    void enqueue(std::function<void()> task) {
        tasks.enqueue(std::move(task));
    }
};

class EventHandler {
private:
    ThreadPool pool;
    SafeQueue<RequestResult> results;

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

	// Store the response along with its retrieval time and latency.
        results.enqueue({jsonResponse, std::chrono::steady_clock::now(), latency});
	
	//if uncomment will be Task2 point2
        // Optionally, you can log the response for debugging.
        // std::cout << "Response: " << jsonResponse.dump(4) << std::endl;
    }

public:
    EventHandler(size_t threads) : pool(threads) {}

    void handle_event(EventType event) {
        if(event == EventType::INVOKE) {
            pool.enqueue([this] { performRPCRequest(); });
        } else if(event == EventType::ERROR) {
            std::cerr << "ERROR event encountered." << std::endl;
        }
        // No action for EventType::NOTHING
    }

    // Method to retrieve and remove the oldest request result.
    RequestResult getOldestResult() {
        return results.dequeue();
    }

    bool resultsEmpty() const {
        return results.empty();
    }
};

int main() {
    EventHandler handler(4); // Create an EventHandler with a pool of 4 threads.

    // Simulate receiving multiple events.
    handler.handle_event(EventType::INVOKE);
    handler.handle_event(EventType::ERROR);
    handler.handle_event(EventType::NOTHING);
    handler.handle_event(EventType::INVOKE);

    // Give some time for all tasks to finish.
    std::this_thread::sleep_for(std::chrono::seconds(1));

    // Retrieve and print the oldest result.
    if (!handler.resultsEmpty()) {
        auto oldestResult = handler.getOldestResult();
        std::cout << "Oldest Response: " << oldestResult.response.dump(4) 
                  << ", Latency: " << oldestResult.latency.count() << "ms" << std::endl;
    }

    return 0;
}


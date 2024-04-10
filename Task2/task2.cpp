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
            if (!task) break; // Проверка на пустую задачу
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
        // Добавляем пустую задачу для каждого рабочего потока
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

        results.enqueue({jsonResponse, end, latency});
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

    RequestResult getOldestResult() {
        return results.dequeue();
    }

    bool resultsEmpty() const {
        return results.empty();
    }
};

int main() {
    EventHandler handler(4); // Создание EventHandler с пулом из 4 потоков

    // Симуляция получения множества событий
    handler.handle_event(EventType::INVOKE);
    handler.handle_event(EventType::ERROR);
    handler.handle_event(EventType::NOTHING);
    handler.handle_event(EventType::INVOKE);

    // Даем время для выполнения всех задач
    std::this_thread::sleep_for(std::chrono::seconds(1));

    // Получаем и печатаем старейший результат
    if (!handler.resultsEmpty()) {
        auto oldestResult = handler.getOldestResult();
        std::cout << "Oldest Response: " << oldestResult.response.dump(4) 
                  << ", Latency: " << oldestResult.latency.count() << "ms" << std::endl;
    }

    return 0;
}


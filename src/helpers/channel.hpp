#pragma once

#include <mutex>
#include <condition_variable>
#include <queue>

namespace slugcat::vrm::helpers {

/**
 * dead-simple producer-consumer channel.
 */
template <typename T>
class Channel final {
    std::mutex _mutex;
    std::condition_variable _cv;
    std::queue<T> _elements;

public:
    Channel() = default;
    ~Channel() = default;

    void write(T elem) {
        std::lock_guard lock(_mutex);

        _elements.push(std::move(elem));
        _cv.notify_one();
    }

    T read() {
        std::unique_lock lock(_mutex);

        _cv.wait(lock, [this] { return !_elements.empty(); });

        T copy = std::move(_elements.front());
        _elements.pop();
        return std::move(copy);
    }

    bool try_read(T& into) {
        std::unique_lock lock(_mutex);

        if (!_elements.empty()) {
            into = std::move(_elements.front());
            _elements.pop();
            return true;
        }

        return false;
    }
};

}

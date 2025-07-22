#pragma once

#include <deque>
#include <mutex>
#include <chrono>
#include <vector>
#include <string>

class TopTracker {
public:
    using Action = std::string;
    using TimePoint = std::chrono::steady_clock::time_point;
    
    TopTracker(size_t max_actions, std::chrono::milliseconds timeout);
    
    void add(const Action& action);
    std::vector<Action> get_actions();
    
private:
    void cleanup();

    std::deque<std::pair<TimePoint, Action>> actions_;
    size_t max_actions_;
    std::chrono::milliseconds timeout_;
    mutable std::mutex mutex_;
};

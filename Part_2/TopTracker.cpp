#include "TopTracker.h"

TopTracker::TopTracker(size_t max_actions, std::chrono::milliseconds timeout)
    : max_actions_(max_actions), timeout_(timeout) {}

void TopTracker::add(const Action& action) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto now = std::chrono::steady_clock::now();
    actions_.emplace_back(now, action);
    cleanup();
}

std::vector<TopTracker::Action> TopTracker::get_actions() {
    std::lock_guard<std::mutex> lock(mutex_);
    cleanup();
    
    std::vector<Action> result;
    for (auto it = actions_.rbegin(); it != actions_.rend(); ++it) {
        result.push_back(it->second);
    }
    return result;
}

void TopTracker::cleanup() {
    auto now = std::chrono::steady_clock::now();
    auto cutoff = now - timeout_;
    
    // Удаление по таймауту
    while (!actions_.empty() && actions_.front().first < cutoff) {
        actions_.pop_front();
    }
    
    // Удаление по превышению лимита
    while (actions_.size() > max_actions_) {
        actions_.pop_front();
    }
}

#include "TopTracker.h"
#include <cassert>
#include <thread>
#include <iostream>

void test_basic_functionality() {
    TopTracker tracker(3, std::chrono::milliseconds(100));
    
    tracker.add("Action1");
    tracker.add("Action2");
    auto actions = tracker.get_actions();
    assert(actions.size() == 2);
    assert(actions[0] == "Action2");
    assert(actions[1] == "Action1");
    
    tracker.add("Action3");
    tracker.add("Action4");
    actions = tracker.get_actions();
    assert(actions.size() == 3); // Проверка ограничения N
    assert(actions[0] == "Action4");
    assert(actions[2] == "Action2");
}

void test_timeout_cleanup() {
    TopTracker tracker(10, std::chrono::milliseconds(100));
    
    tracker.add("OldAction");
    std::this_thread::sleep_for(std::chrono::milliseconds(150));
    tracker.add("NewAction");
    
    auto actions = tracker.get_actions();
    assert(actions.size() == 1);
    assert(actions[0] == "NewAction");
}

void test_thread_safety() {
    TopTracker tracker(100, std::chrono::seconds(1));
    
    auto worker = [&tracker](int id) {
        for (int i = 0; i < 1000; ++i) {
            tracker.add("Thread" + std::to_string(id) + "_Action" + std::to_string(i));
        }
    };
    
    std::thread t1(worker, 1);
    std::thread t2(worker, 2);
    t1.join();
    t2.join();
    
    auto actions = tracker.get_actions();
    assert(actions.size() == 100); // Проверка ограничения N
}

int main() {
    test_basic_functionality();
    test_timeout_cleanup();
    test_thread_safety();
    std::cout << "All tests passed!" << std::endl;
    return 0;
}

#pragma once

#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

namespace timer {
using Callback = std::function<void()>;
using TimerId = unsigned int;
using Duration = unsigned int;

struct Task {
  TimerId id;
  Duration duration;
  Duration remain;
  bool is_cleared{false};
  bool is_interval;
  std::shared_ptr<Callback> callback;

  Task(Duration duration, bool is_interval, std::shared_ptr<Callback> callback);
};

class Timer {
 public:
  TimerId SetTimeout(const std::shared_ptr<Callback>& callback,
                     Duration duration);
  TimerId SetInterval(const std::shared_ptr<Callback>& callback,
                      Duration duration);
  void Clear(TimerId id);

 private:
  TimerId id_{0};
  std::mutex id_mutex_;
  std::mutex pending_tasks_mutex_;
  std::mutex fulfilled_callbacks_mutex_;
  std::mutex task_map_mutex_;
  std::map<TimerId, std::shared_ptr<Task>> task_map_;
  std::unique_ptr<std::vector<std::shared_ptr<Task>>> pending_tasks_{
      new std::vector<std::shared_ptr<Task>>()};
  std::unique_ptr<std::vector<std::shared_ptr<Callback>>> fulfilled_callbacks_{
      new std::vector<std::shared_ptr<Callback>>()};
  std::thread timer_thread_{Timer::TimerThread, this};
  std::thread task_thread_{Timer::TaskThread, this};

  TimerId AddTask(const std::shared_ptr<Task>& task);
  static void TimerThread(Timer* timer_ptr);
  static void TaskThread(Timer* timer_ptr);
};
}  // namespace timer

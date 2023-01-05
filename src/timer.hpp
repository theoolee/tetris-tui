#pragma once

#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

namespace timer {
using callback = std::function<void()>;
using timer_id = unsigned int;
using duration = unsigned int;

struct task {
  timer_id id;
  timer::duration duration;
  timer::duration remain;
  bool is_cleared{false};
  bool is_interval;
  std::shared_ptr<callback> callback;

  task(timer::duration duration,
       bool is_interval,
       std::shared_ptr<timer::callback> callback);
};

class timer {
 public:
  timer_id set_timeout(const std::shared_ptr<callback>& callback,
                       duration duration);
  timer_id set_interval(const std::shared_ptr<callback>& callback,
                        duration duration);
  void clear(timer_id id);

 private:
  timer_id id_{0};
  std::mutex id_mutex_;
  std::mutex pending_tasks_mutex_;
  std::mutex fulfilled_callbacks_mutex_;
  std::mutex task_map_mutex_;
  std::map<timer_id, std::shared_ptr<task>> task_map_;
  std::unique_ptr<std::vector<std::shared_ptr<task>>> pending_tasks_{
      new std::vector<std::shared_ptr<task>>()};
  std::unique_ptr<std::vector<std::shared_ptr<callback>>> fulfilled_callbacks_{
      new std::vector<std::shared_ptr<callback>>()};
  std::thread timer_thread_{timer::timer_thread, this};
  std::thread task_thread_{timer::task_thread, this};

  timer_id add_task(const std::shared_ptr<task>& task);
  static void timer_thread(timer* timer_ptr);
  static void task_thread(timer* timer_ptr);
};
}  // namespace timer

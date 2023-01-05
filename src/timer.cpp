#include "timer.hpp"

namespace timer {
using namespace std::chrono_literals;

Task::Task(Duration duration,
           bool is_interval,
           std::shared_ptr<Callback> callback)
    : duration(duration),
      remain(duration),
      is_interval(is_interval),
      callback(std::move(callback)) {}

TimerId Timer::SetTimeout(const std::shared_ptr<Callback>& callback,
                          Duration duration) {
  return AddTask(std::make_shared<Task>(duration, false, callback));
}

TimerId Timer::SetInterval(const std::shared_ptr<Callback>& callback,
                           Duration duration) {
  return AddTask(std::make_shared<Task>(duration, true, callback));
}

void Timer::Clear(TimerId id) {
  task_map_mutex_.lock();
  auto task = task_map_[id];
  task_map_mutex_.unlock();

  if (task != nullptr) {
    task->is_cleared = true;
  }
}

TimerId Timer::AddTask(const std::shared_ptr<Task>& task) {
  id_mutex_.lock();
  TimerId id = id_++;
  id_mutex_.unlock();
  task->id = id;
  pending_tasks_mutex_.lock();
  pending_tasks_->push_back(task);
  pending_tasks_mutex_.unlock();
  task_map_mutex_.lock();
  task_map_[id] = task;
  task_map_mutex_.unlock();
  return id;
}

void Timer::TimerThread(Timer* timer_ptr) {
  while (true) {
    std::this_thread::sleep_for(1ms);
    timer_ptr->pending_tasks_mutex_.lock();
    auto loop_tasks = *timer_ptr->pending_tasks_;
    timer_ptr->pending_tasks_->clear();
    timer_ptr->pending_tasks_mutex_.unlock();

    for (auto& task : loop_tasks) {
      task->remain--;
      bool should_execute = task->remain <= 0 && !task->is_cleared;
      bool should_loop =
          (task->remain > 0 || task->is_interval) && !task->is_cleared;

      if (should_execute) {
        timer_ptr->fulfilled_callbacks_mutex_.lock();
        timer_ptr->fulfilled_callbacks_->push_back(task->callback);
        timer_ptr->fulfilled_callbacks_mutex_.unlock();

        if (task->is_interval) {
          task->remain = task->duration;
        }
      }

      if (should_loop) {
        timer_ptr->pending_tasks_mutex_.lock();
        timer_ptr->pending_tasks_->push_back(task);
        timer_ptr->pending_tasks_mutex_.unlock();
      } else {
        timer_ptr->task_map_mutex_.lock();
        timer_ptr->task_map_.erase(task->id);
        timer_ptr->task_map_mutex_.unlock();
      }
    }
  }
}

void Timer::TaskThread(Timer* timer_ptr) {
  while (true) {
    timer_ptr->fulfilled_callbacks_mutex_.lock();
    auto loop_callbacks = *timer_ptr->fulfilled_callbacks_;
    timer_ptr->fulfilled_callbacks_->clear();
    timer_ptr->fulfilled_callbacks_mutex_.unlock();

    for (auto& loop_callback : loop_callbacks) {
      (*loop_callback)();
    }
  }
}

}  // namespace timer

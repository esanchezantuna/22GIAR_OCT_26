#include <iostream>
#include <thread>

class thread_guard {
  private:
    std::thread& t;
  public:
    explicit thread_guard(std::thread& t_) : t(t_) {}

    ~thread_guard() {
        if (t.joinable()) {
            t.join();
        }
    }
    thread_guard(thread_guard const&) = delete;
    thread_guard& operator=(thread_guard const&) = delete;
};

class Task {
public:
  int& i;
  Task(int& i_) : i(i_) {}
  void operator() () {
    for(int j=0; j<1000000; ++j) {
        i += j;
    }
  }
};

void any_function() {
  int local_variable = 0;
  Task my_task(local_variable);
  std::thread my_thread(my_task);
  thread_guard my_guard(my_thread);

  std::chrono::seconds dura(2);
  std::this_thread::sleep_for(dura);
}

int main() {
    any_function();

    return 0;
}
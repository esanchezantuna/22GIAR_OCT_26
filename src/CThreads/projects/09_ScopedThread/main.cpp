#include <iostream>
#include <thread>

class scoped_thread {
  private:
    std::thread t;
  public:
    explicit scoped_thread(std::thread t_) :
      t(std::move(t_)) {
        if (!t.joinable())
          throw std::logic_error("No Thread");
      }

    ~scoped_thread() {
        t.join();
    }

    scoped_thread(scoped_thread const&) = delete;
    scoped_thread& operator=(scoped_thread const&) = delete;
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
  scoped_thread t{std::thread(Task(local_variable))};
  std::chrono::seconds dura(2);
  std::this_thread::sleep_for(dura);
}

int main() {
    any_function();

    return 0;
}
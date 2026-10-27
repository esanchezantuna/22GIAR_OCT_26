#include <iostream>
#include <thread>
#include <cstdint>

class Task {
  public:
    u_int32_t& i;

    Task(u_int32_t& i_) : i(i_) {}

    void operator() () {
        for(int j=0; j<1000000; ++j) {
            i += j;
        }

        std::cout << "Thread = " << i << std::endl;
    }
};

void any_function() {
  u_int32_t local_variable = 0;
  Task my_task(local_variable);

  std::thread my_thread(my_task);
  my_thread.detach();

  std::cout << "Value = " << local_variable << std::endl;
}

int main() {
    any_function();

    return 0;
}
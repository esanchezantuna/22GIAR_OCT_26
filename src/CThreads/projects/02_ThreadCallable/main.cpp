#include <iostream>
#include <thread>

class Task {
  public:
      void operator() () const {
        std::cout << "From Thread" << std::endl;
      }
};

int main() {
    Task t;
    std::thread my_thread(t);
    std::cout << "From Main Thread" << std::endl;
    my_thread.join();
    return 0;
}

#include <iostream>
#include <thread>

void function() {
    std::cout << "From Thread " << std::this_thread::get_id() << std::endl;
}

int main() {
    std::thread t(function);
    std::cout << "From Main " << std::this_thread::get_id() << std::endl;

    t.join();
    return 0;
}
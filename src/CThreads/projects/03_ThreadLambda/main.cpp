#include <iostream>
#include <thread>

int main() {
    int value = 42;

    std::thread my_thread([value] {
        std::cout << "From Thread val=" << value<< std::endl;
    });

    std::cout << "From Main Thread" << std::endl;
    my_thread.join();
    return 0;
}
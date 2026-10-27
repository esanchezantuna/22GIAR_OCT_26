#include <iostream>
#include <thread>

void function() {                 //Function to be executed in a separate thread
    std::cout << "From Thread" << std::endl;
}

int main() {
    std::thread my_thread(function);
    
    std::cout << "From Main Thread" << std::endl;

    std::cout << std::boolalpha << my_thread.joinable() << std::endl;

    my_thread.join();
    return 0;
}
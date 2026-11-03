#include <iostream>
#include <thread>

void tarea() {
    std::cout << "Ejecutando tarea\n";
}

int main() {
    std::thread t1(tarea);

    std::thread t2 = std::move(t1);

    std::cout << std::boolalpha;
    std::cout << "t1: " << t1.joinable() << '\n';
    std::cout << "t2: " << t2.joinable() << '\n';

    t2.join();

    return 0;
}
#include <iostream>
#include <thread>

int contador = 435;

void incrementar() {
    for (int i = 0; i < 1000000; ++i) {
        ++contador;
    }
    
}

int main() {
    std::thread t1(incrementar);
    std::thread t2(incrementar);

    t1.join();
    t2.join();

    std::cout << "Contador = " << contador << std::endl;

    return 0;
}
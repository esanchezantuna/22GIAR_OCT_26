#include <iostream>
#include <thread>

int contador = 0;
std::mutex m;

void incrementar() {
    m.lock();
    for (int i = 0; i < 1000000; ++i) {        
        ++contador;        
    }   
    m.unlock(); 
}

int main() {
    std::thread t1(incrementar);
    std::thread t2(incrementar);

    t1.join();
    t2.join();

    std::cout << "Contador = " << contador << std::endl;

    return 0;
}
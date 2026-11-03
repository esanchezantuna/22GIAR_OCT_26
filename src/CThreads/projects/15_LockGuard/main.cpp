#include <iostream>
#include <thread>

int contador = 0;
std::mutex m;

void incrementar() {
    {
        std::lock_guard<std::mutex> guard(m);    // m.lock();  //enter
        for (int i = 0; i < 1000000; ++i) {      //seccion critica
            ++contador;        
        }                           
    }                                            // m.unlock();  //exit

    int a = 200;    // remainder section
    a = a * 345;
}                                           

int main() {
    std::thread t1(incrementar);
    std::thread t2(incrementar);

    t1.join();
    t2.join();

    std::cout << "Contador = " << contador << std::endl;

    return 0;
}
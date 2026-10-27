#include <iostream>
#include <thread>

void function(int i, const std::string& s) {
    std::cout << s << i << std::endl;
}

void actualizar(int& dato) {
    dato = 100;
}

int main() {
    std::thread my_thread(function, 3, "Hello ");
    my_thread.join();

    int valor = 0;
    std::cout << valor << std::endl;
    std::thread t(actualizar, std::ref(valor));    
    t.join();

    std::cout << valor << std::endl;

    return 0;
}
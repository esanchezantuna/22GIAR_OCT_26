#include <iostream>
#include <memory>
#include <thread>

class Dato {
public:
    int valor = 42;
};

void procesar(std::unique_ptr<Dato> dato) {
    // El hilo recibe la propiedad de dato.
    std::cout << dato->valor << std::endl;
}

int main() {
    std::unique_ptr<Dato> p = std::make_unique<Dato>();
    std::thread t(procesar, std::move(p));
    t.join();
    return 0;
}
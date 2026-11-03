#include <cstdint>
#include <iostream>
#include <thread>
#include <vector>

void procesar(std::uint32_t id) {
    std::cout << "Procesando tarea " << id << '\n';
}

int main() {
    std::vector<std::thread> hilos;

    for (std::uint32_t i = 0; i < 5; ++i) {
        hilos.emplace_back(procesar, i);
    }

    for (auto& hilo : hilos) {
        hilo.join();
    }

    std::cout << "Todas las tareas han terminado\n";

    return 0;
}
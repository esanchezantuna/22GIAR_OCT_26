#include <cstdint>
#include <iostream>
#include <thread>
#include <vector>

#include "engine/metrics.h"
#include "engine/globals.h"

void procesar(std::uint32_t id) {
    MetricsScope metrics("Funcion Procesar");
    std::cout << "Procesando tarea " << id << '\n';
}

int main() {
    Global::metrics = std::make_unique<MetricsState>();
    Metrics_StartRecording(PROJECT_PATH "metrics.json");

    std::vector<std::thread> hilos;

    for (std::uint32_t i = 0; i < 5; ++i) {
        hilos.emplace_back(procesar, i);
    }

    for (auto& hilo : hilos) {
        hilo.join();
    }

    std::cout << "Todas las tareas han terminado\n";

    Metrics_StopRecording();
    Global::metrics.reset(nullptr);

    return 0;
}
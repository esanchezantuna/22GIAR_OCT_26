#include <iostream>
#include <thread>

#include "engine/metrics.h"
#include "engine/globals.h"

int contador = 435;

void incrementar() {
    MetricsScope metrics("Incrementar");
    for (int i = 0; i < 1000000; ++i) {
        ++contador;
    }    
}

int main() {
    Global::metrics = std::make_unique<MetricsState>();
    Metrics_StartRecording(PROJECT_PATH "metrics.json");

    std::thread t1(incrementar);
    std::thread t2(incrementar);

    t1.join();
    t2.join();

    std::cout << "Contador = " << contador << std::endl;

    Metrics_StopRecording();
    Global::metrics.reset(nullptr);

    return 0;
}
#ifndef _GLOBALS_H
#define _GLOBALS_H 1

#include "metricsstate.h"

namespace Global {
  extern std::unique_ptr<MetricsState> metrics;
}

#endif //MBR_CORE_GLOBALS_H
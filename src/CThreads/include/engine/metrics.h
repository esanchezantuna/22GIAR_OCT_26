#ifndef _METRICS_H
#define _METRICS_H 1

#ifndef _ENABLE_METRICS
#define _ENABLE_METRICS 0
#endif

void Metrics_SetThreadName(const char *name);
void Metrics_TaskStart(const char *txt);
void Metrics_TaskEnd();

void Metrics_StartRecording(const char *file);
void Metrics_StopRecording();

struct MetricsScope {
  MetricsScope() = delete;
  MetricsScope(const MetricsScope &) = delete;
  explicit MetricsScope(const char *txt) : _txt(txt) { Metrics_TaskStart(txt) ; }
  ~MetricsScope() { Metrics_TaskEnd(); }
  const char *_txt;
};

#endif

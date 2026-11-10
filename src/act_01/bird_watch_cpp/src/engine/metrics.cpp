#include <chrono>
#include <thread>
#include <algorithm>
#include <engine/metrics.h>
#include "engine/metricsstate.h"

std::atomic<uint32_t> MetricsState::Thread::base_id = {0};

namespace Global {
  std::unique_ptr<MetricsState> metrics;
}

thread_local MetricsState::Thread MetricsState::thread_data;
std::chrono::high_resolution_clock::time_point MetricsState::_baseTime;

#if _ENABLE_METRICS
static MetricsState::Thread* GetThread() {
  return &MetricsState::thread_data;
}

void Metrics_SetThreadName(const char *name) {
  const auto t = GetThread();
  t->name = name;
}

static uint32_t Metrics_RAW_END(char *buffer, size_t buffer_size) {
  #define K_INT(K) "\"" #K "\":%llu,"
  #define K_STR(K) "\"" #K "\":\"%s\","
      uint32_t added = 0;
      {
        const int64_t time = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - MetricsState::_baseTime).count();
        const char *format_str = "{" K_INT(tid)  K_STR(name) K_STR(ph) K_INT(ts) "\"pid\":1}\n";
        int32_t result = snprintf(buffer, buffer_size, format_str, (uint64_t) 0, "END", "R", (uint64_t) time);
        if (result < 0  || result >= buffer_size) {
          // Invalid TRACE!
          abort();
        }
        added = (uint32_t) result;
      }
  #undef K_INT
  #undef K_STR
  return added;
}

static uint32_t Metrics_RAW(char *buffer, size_t buffer_size, uint32_t tid, const char *name, const char *type, const char *args = nullptr) {
  #define K_INT(K) "\"" #K "\":%llu,"
  #define K_STR(K) "\"" #K "\":\"%s\","
      uint32_t added = 0;
      {
        const int64_t time = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - MetricsState::_baseTime).count();
        const char *format_str = "{" K_INT(tid)  K_STR(name) K_STR(ph) K_INT(ts) "\"pid\":1,\"args\":{%s} },\n";
        int32_t result = snprintf(buffer, buffer_size, format_str, (uint64_t) tid, name, type, (uint64_t) time, args?args:"");
        if (result < 0  || result >= buffer_size) {
          // Invalid TRACE!
          abort();
        }
        added = (uint32_t) result;
      }
  #undef K_INT
  #undef K_STR
  return added;
}

static void Metrics_Flush(MetricsState::Thread *owner) {
  if (owner->buffer_pos > 0) {
    auto m = Global::metrics.get();
    if (m) {
      m->mutex.lock();
      if (m->output) {
        char buff1[256];
        char buff2[256];
        uint32_t d1 = Metrics_RAW(buff1, sizeof(char) * 256, 0, "MetricsFlush", "B");
        fwrite(&owner->buffer[0], 1, owner->buffer_pos, m->output);
        uint32_t d2 = Metrics_RAW(buff2, sizeof(char) * 256, 0, "MetricsFlush", "E");
        fwrite(buff1, 1, d1, m->output);
        fwrite(buff2, 1, d2, m->output);
      }
      m->mutex.unlock();
    }
    owner->buffer_pos = 0;
    owner->buffer[0] = 0;
  }
}

static void Metrics_Log(uint32_t tid, const char *raw_name, const char *type, const char *args = nullptr) {
  char name[256];
  for(int i = 0, j = 0; i < sizeof(char) * 256 ; ++i, ++j) {
    name[i] = raw_name[j];
    if (raw_name[j] == '\\') name[++i] = '\\';
    if (raw_name[j] == '\0') break;
  }
  name[255] = '\0';

  auto t = GetThread();
  if (!t->_initialized) {
    if (!Global::metrics) return;
    Global::metrics->mutex.lock();
    t->buffer[0] = 0;
    t->buffer_pos = 0;
    t->_initialized = true;
    Global::metrics->threads_ptr.push_back(t);
    Global::metrics->mutex.unlock();
  }

  char buff[1024];
  uint32_t added = Metrics_RAW(buff, sizeof(char) * 1024, tid, name, type, args);
  if (t->buffer_pos + added > MetricsState::Thread::_kMaxBufferSize) {
    Metrics_Flush(t);
  }

  // copy metrics to buffer
  memcpy(t->buffer + t->buffer_pos, buff, sizeof(char) * static_cast<size_t>(added));
  t->buffer_pos += static_cast<size_t>(added);
  t->buffer[t->buffer_pos] = 0;
}

MetricsState::Thread::~Thread() {
  if (buffer) {
    if (Global::metrics) {
      Metrics_Flush(this);
      Global::metrics->threads_ptr.erase(std::remove(Global::metrics->threads_ptr.begin(), Global::metrics->threads_ptr.end(), this), Global::metrics->threads_ptr.end());
    }
    buffer_pos = 0;
  }
}

void Metrics_TaskStart(const char *txt) {
  auto t = GetThread();
  t->stack.push_back(txt);
  Metrics_Log(t->id, txt, "B");
}

void Metrics_TaskEnd() {
  auto t = GetThread();
  const char *task_name = t->stack.back();
  Metrics_Log(t->id, task_name, "E");
  t->stack.pop_back();
}

void Metrics_StartRecording(const char *filename) {
  Metrics_StopRecording();
  auto m = Global::metrics.get();
  char path[255];

  auto file = fopen(filename, "w");
  if (file) {
    if (m->output) Metrics_StopRecording();
    m->mutex.lock();
    m->_baseTime = std::chrono::high_resolution_clock::time_point();
    m->frame_count = 0;
    m->output = file;
    fprintf(file, "{\"traceEvents\":[\n");
    m->mutex.unlock();
  } else {
    std::this_thread::sleep_for(std::chrono::seconds(10));
  }
}

void Metrics_StopRecording() {
  auto m = Global::metrics.get();

  for (auto thread : m->threads_ptr) {
    Metrics_Flush(thread);
  }

  m->mutex.lock();
  if (m->output) {
    char buffer_tmp[512];
    Metrics_RAW_END(buffer_tmp, sizeof(char) * 512);
    fprintf(m->output, "%s] }", buffer_tmp);
    fclose(m->output);
    m->output = nullptr;
  }
  m->mutex.unlock();
}

#else
  void Metrics_SetThreadName(const char *filename) {}
  void Metrics_TaskStart(const char *) {}
  void Metrics_TaskEnd() {}
  void Metrics_StartRecording(const char *) {}
  void Metrics_StopRecording() {}
  MetricsState::Thread::~Thread() {}
#endif

#ifndef _METRICS_STATE_H
#define _METRICS_STATE_H 1

#include <mutex>
#include <vector>
#include <cstring>
#include <string>
#include <atomic>

struct MetricsState {
  struct Token {
      static const size_t length = 128;
      char data[length];
      Token(const char *c) { copy(c); }
      Token() { data[0] = 0; }
      Token& operator=(const Token &t) {copy(t.data); return *this;}
      void copy(const char *c) {
          size_t m = std::min(strlen(c), length-1);
          memcpy(data, c, m);
          data[m] = 0;
      }
      operator const char *() const { return data; }
  };

  struct Thread {
    static std::atomic<uint32_t> base_id; 
    Thread()  {
      name = "noname";
      id = base_id.fetch_add(1) + 100;
    }

    ~Thread();
    uint32_t id;
    bool _initialized = false;
    std::string name;
    std::vector<Token> stack;
    static constexpr uint32_t _kMaxBufferSize = 1024 * 1024 * 2;
    char buffer[_kMaxBufferSize];
    uint32_t buffer_pos = 0;
  };

  std::mutex mutex;
  FILE *output = nullptr;
  uint32_t frame_count = 0;
  std::vector<Thread *> threads_ptr;

  static std::chrono::high_resolution_clock::time_point _baseTime;
  static thread_local Thread thread_data;
};

#endif

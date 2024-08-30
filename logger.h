#ifndef LOGGING_H
#define LOGGING_H
#include <iostream>
#include <string.h>
#include <sstream>

enum Level
{
  info,
  debug,
  error
};

class Logger
{
  public:
    Logger(const char* file, int line);
    ~Logger();

    template<typename T>
    Logger& operator<<(const T& data)
    {
      buffer_ << data;
      return *this;
    }

  private:
    const char* file_;
    std::ostringstream buffer_;
    int line_;
    Level level_;
};

#define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#define LOG(level) Logger(__FILENAME__, __LINE__)
// #define LOG(level)

#endif

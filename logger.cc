#include "logger.h"


Logger::Logger(const char* file, int line) : file_(file), line_(line)
{
  // todo 
} 


Logger::~Logger()
{
  std::cout << file_ << ":" << line_ << " - " << buffer_.str() << std::endl;
}


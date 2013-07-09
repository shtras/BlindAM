#pragma once
#include <fstream>

class Logger
{
public:
  static ENGINE_API Logger& getInstance();

  ENGINE_API void log(CString logName, CString message);
private:
  Logger();
  ~Logger();

  map<CString, ofstream*> logs_;
};

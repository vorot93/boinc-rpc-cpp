#ifndef _EXCEPTION_UTIL_HPP_
#define _EXCEPTION_UTIL_HPP_

#include <string>

#ifndef DEFINE_EXCEPTION
#define DEFINE_EXCEPTION(ClassName, Message)                                              \
  class ClassName : public std::exception                                                 \
  {                                                                                       \
  private:                                                                                \
    std::string msg;                                                                      \
                                                                                          \
  public:                                                                                 \
    ClassName() { msg = std::string(Message); }                                           \
    ClassName(std::string arg) { msg = std::string(Message) + std::string(" : ") + arg; } \
    virtual const char* what() const throw() { return msg.data(); }                       \
  }
#endif

#endif

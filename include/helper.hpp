#ifndef HELPER_HPP
#define HELPER_HPP

#include <string>
#include "types.hpp"

//#define DEBUG
#ifdef DEBUG
#define DEBUG_PRINT(...) std::cout << __VA_ARGS__ << std::endl
#else
#define DEBUG_PRINT(...)
#endif

void throwWarn(std::string message);
void tokenDebug(DataToken* token, int indent = 0);

#endif
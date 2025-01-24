#ifndef CALC_HPP
#define CALC_HPP

#include "types.hpp"

DataToken TokenConstructNumber(double value);
void TokenFree(DataToken* token);
void TokenFree(DataToken rootToken);
void TokenAppendSubtoken(DataToken* base, DataToken* ext);
double VariableGet(std::string name);
DataToken FunctionCall(std::string name, DataToken data);
DataToken TokenCalc(DataToken* tokenList);


#endif
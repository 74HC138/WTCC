#ifndef CALC_HPP
#define CALC_HPP

#include "types.hpp"

DataToken ObjectGet(std::string name);
void ObjectSet(std::string name, double val);
void ObjectSet(std::string name, char* text);
void ObjectSet(std::string name, DataToken (*function)(DataToken));
ObjectType ObjectGetType(std::string name);
DataToken ObjectCall(std::string name, DataToken data);

DataToken TokenEval(DataToken* tokenList);
DataToken* TokenizeString(std::string input);

#endif
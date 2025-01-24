#ifndef TYPES_HPP
#define TYPES_HPP

#include <string>

enum TokenType {
    TOKEN_NONE = 0,
    TOKEN_NUMBER,
    TOKEN_OPERATOR,
    TOKEN_SUBTOKEN,
    TOKEN_VARIABLE,
    TOKEN_FUNCTION,
    TOKEN_TEXT,
    TOKEN_ARGSEPERATOR,
    TOKEN_UNKNOWN
};

enum OperatorType {
    OP_NONE = 0,
    OP_ADDITION,
    OP_SUBTRACTION,
    OP_MULTIPLICATION,
    OP_DIVISION,
    OP_POWER,
    OP_MODULUS,
    OP_UNKNOWN
};

struct DataToken {
    TokenType type; //type of token
    DataToken* nextToken; //pointer to next token in linked list
    double value; //value of token when number token
    OperatorType operatorType; //type of operator when operator token
    DataToken* subToken; //pointer to sub token list of subtoken
    char* name; //name of variable, function or text value when variable, function or text token token
};

struct DataVariable {
    std::string name; //name of variable
    double value; //value of variable
};

struct DataFunction {
    DataToken (*function)(DataToken data); //pointer to function to be executet
};


#endif
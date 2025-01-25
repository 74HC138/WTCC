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
    std::string name; //name of function
    DataToken (*function)(DataToken data); //pointer to function to be executet
};

enum ObjectType {
    OBJ_NONE = 0,
    OBJ_NUMBER,
    OBJ_TEXT,
    OBJ_FUNCTION
};

struct DataObject {
    std::string name; //name of object
    ObjectType type; //type of object
    union {
        double value; //value for number type
        char* text; //text for text type
        DataToken (*function)(DataToken data); //function for function type
    };
};


#endif
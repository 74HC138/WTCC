#ifndef TYPES_HPP
#define TYPES_HPP

#include <string>

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
enum TokenType {
    TOKEN_NONE = 0,
    TOKEN_NUMBER,
    TOKEN_OPERATOR,
    TOKEN_SUBTOKEN,
    TOKEN_OBJECT,
    TOKEN_TEXT,
    TOKEN_ARGSEPERATOR,
    TOKEN_UNKNOWN
};

class DataToken {
    public:
        DataToken();
        DataToken(double);
        DataToken(std::string, bool isObject = false);
        DataToken(OperatorType);
        ~DataToken();

        TokenType type; //type of token
        DataToken* nextToken; //pointer to next token in linked list
        double value; //value of token when number token
        OperatorType operatorType; //type of operator when operator token
        DataToken* subToken; //pointer to sub token list of subtoken
        std::string name; //name of variable, function or text value when variable, function or text token token
};

enum ObjectType {
    OBJ_NONE = 0,
    OBJ_NUMBER,
    OBJ_TEXT,
    OBJ_FUNCTION,
    OBJ_CFUNCTION
};

struct DataObject {
    std::string name; //name of object
    ObjectType type; //type of object
    union {
        double value; //value for number type
        char* text; //text for text type
        DataToken (*cFunction)(DataToken data); //function for function type
        DataToken* function;
    };
};


#endif
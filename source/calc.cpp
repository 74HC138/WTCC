#include <vector>
#include <iostream>

#include "calc.hpp"
#include "types.hpp"
#include "helper.hpp"
#include <math.h>


//structs and enums for operator und token functionality
struct OperatorFunction {
    OperatorType type;
    int (*function)(DataToken* dest, DataToken* src);
};
enum TokenHandlerReturn {
    TH_CONTINUE = 0,
    TH_EXIT,
    TH_SKIP,
    TH_ERROR
};
struct TokenHandlerType {
    TokenType type;
    TokenHandlerReturn (*function)(DataToken** currentResult, DataToken* token);
};

//object storage for variables and functions
std::vector<DataObject> objectStorage;
//functions for the object system to set, retrieve and delete functions
std::vector<DataObject>::iterator ObjectGetRawIterator() {
    return objectStorage.begin();
}
std::vector<DataObject>::iterator ObjectGetRawEnditerator() {
    return objectStorage.end();
}
DataToken ObjectGet(std::string name) {
    for (auto it = objectStorage.begin(); it != objectStorage.end(); it++) {
        if (name.compare(it->name) == 0) {
            DataToken retToken;
            switch (it->type) {
                case OBJ_NUMBER:
                    retToken = DataToken(it->value);
                    break;
                case OBJ_TEXT:
                    retToken = DataToken(it->text);
                    break;
                case OBJ_FUNCTION:
                case OBJ_CFUNCTION:
                case OBJ_NONE:
                default:
                    //cant get a function
                    retToken = DataToken(0);
            }
            return retToken;
        }
    }
    throwWarn("Object " + name + " does not exist\n");
    return DataToken(0);
}
void ObjectSet(std::string name, double val) {
    for (auto it = objectStorage.begin(); it != objectStorage.end(); it++) {
        if (name.compare(it->name) == 0) {
            if (it->type == OBJ_NUMBER) {
                it->value = val;
                return;
            }
            throwWarn("Object " + name + " already defined as different type!\n");
            return;
        }
    }
    DataObject obj = {.name = name, .type = OBJ_NUMBER, .value = val};
    objectStorage.push_back(obj);
}
void ObjectSet(std::string name, char* text) {
    for (auto it = objectStorage.begin(); it != objectStorage.end(); it++) {
        if (name.compare(it->name) == 0) {
            if (it->type == OBJ_TEXT) {
                delete it->text;
                it->text = text;
                return;
            }
            throwWarn("Object " + name + " already defined as different type\n");
            return;
        }
    }
    DataObject obj = {.name = name, .type = OBJ_TEXT, .text = text};
    objectStorage.push_back(obj);
}
void ObjectSet(std::string name, std::string text) {
    char* tmp = (char*) malloc(text.length() + 1);
    text.copy(tmp, text.length());
    ObjectSet(name, tmp);
}
void ObjectSet(std::string name, DataToken (*function)(DataToken)) {
    for (auto it = objectStorage.begin(); it != objectStorage.end(); it++) {
        if (name.compare(it->name) == 0) {
            if (it->type == OBJ_CFUNCTION) {
                it->cFunction = function;
                return;
            }
            throwWarn("Object " + name + " already defined as different type\n");
            return;
        }
    }
    DataObject obj = {.name = name, .type = OBJ_CFUNCTION, .cFunction = function};
    objectStorage.push_back(obj);
}
void ObjectSet(std::string name, DataToken* function) {
    for (auto it = objectStorage.begin(); it != objectStorage.end(); it++) {
        if (name.compare(it->name) == 0) {
            if (it->type == OBJ_FUNCTION) {
                it->function = function;
                return;
            }
            throwWarn("Object " + name + " already defined as different type\n");
            return;
        }
    }
    DataObject obj = {.name = name, .type = OBJ_FUNCTION, .function = function};
    objectStorage.push_back(obj);
}
ObjectType ObjectGetType(std::string name) {
    for (auto it = objectStorage.begin(); it != objectStorage.end(); it++) {
        if (name.compare(it->name) == 0) return it->type;
    }
    return OBJ_NONE;
}
DataToken ObjectCall(std::string name, DataToken data) {
    for (auto it = objectStorage.begin(); it != objectStorage.end(); it++) {
        if (name.compare(it->name) == 0) {
            if (it->type == OBJ_CFUNCTION) {
                return it->cFunction(data);
            } else if (it->type == OBJ_FUNCTION) {
                return *TokenEval(it->function); //wtcc functions do not support parameters for now. TODO
            } else {
                throwWarn("Attempted to call " + name + " a non function\n");
                return DataToken(0);
            }
        }
    }
    throwWarn("Object " + name + " does not exist\n");
    return DataToken(0);
}
void ObjectDelete(std::string name) {
    for (auto it = objectStorage.begin(); it != objectStorage.end(); it++) {
        if (name.compare(it->name) == 0) {
            objectStorage.erase(it);
            return;
        }
    }
}


//Operator evaluation functions, externalized from TokenEval for cleanlieness
const std::vector<OperatorFunction> OpFunctions = {
    {OP_NONE, [](DataToken* dest, DataToken* src) {
        //do nothing and return.... having done nothing....
        dest = dest;
        src = src;
        return 0;
    }},
    {OP_ADDITION, [](DataToken* dest, DataToken* src) {
        if (dest->type == TOKEN_NUMBER) {
            if (src->type == TOKEN_NUMBER) {
                dest->value += src->value;
                return 0;
            } else if (src->type == TOKEN_TEXT) {
                dest->value += std::stod(src->name);
                return 0;
            } else {
                //something that is not number and not text.... still bad
                throwWarn("unsuported source data type for addition!\n");
                return 1;
            }
        } else if (dest->type == TOKEN_TEXT) {
            if (src->type == TOKEN_NUMBER) {
                dest->name += std::to_string(src->value);
                return 0;
            } else if (src->type == TOKEN_TEXT) {
                dest->name += src->name;
                return 0;
            } else {
                //still something that is not a number nor text.... still very bad
                throwWarn("unsuported source data type for addition!\n");
                return 1;
            }
        } else {
            //something that is not number and not text.... bad
            throwWarn("unsuported destination data type for addition!\n");
            return 1;
        }
    }},
    {OP_SUBTRACTION, [](DataToken* dest, DataToken* src) {
        double val = 0;
        if (src->type == TOKEN_NUMBER) {
            val = src->value;
        } else {
            throwWarn("unsported source type for subtraction!\n");
            return 1;
        }
        if (dest->type == TOKEN_NUMBER) {
            dest->value -= val;
        } else if (dest->type == TOKEN_TEXT) {
            int len = (int) val;
            if (len >= (int) dest->name.length()) {
                dest->name = "";
            } else {
                if (val > 0) dest->name.erase(dest->name.end() - len, dest->name.end());
            }
        } else {
            //sorce data is not string or number
            throwWarn("unsuported destination type for subtraction\n");
            return 1;
        }
        return 0;
    }},
    {OP_MULTIPLICATION, [](DataToken* dest, DataToken* src) {
        double val = 0;
        if (src->type == TOKEN_NUMBER) {
            val = src->value;
        } else {
            throwWarn("unsporte source type for multiplication!\n");
            return 1;
        }
        if (dest->type == TOKEN_NUMBER) {
            dest->value *= val;
        } else if (dest->type == TOKEN_TEXT) {
            std::string tmp = "";
            for (int i = 0; i < val; i++) tmp += dest->name;
            dest->name = tmp;
        } else {
            throwWarn("unsported destination type for multiplication!\n");
            return 1;
        }
        return 0;
    }},
    {OP_DIVISION, [](DataToken* dest, DataToken* src) {
        if (src->type != TOKEN_NUMBER) {
            throwWarn("unsuported source type for division!\n");
            return 1;
        }
        if (dest->type != TOKEN_NUMBER) {
            throwWarn("unsuported destination type for division!\n");
            return 1;
        }
        if (src->value == 0) {
            throwWarn("Division by 0!\n");
            dest->value = 0;
            return 1;
        }
        dest->value /= src->value;
        return 0;
    }},
    {OP_POWER, [](DataToken* dest, DataToken* src) {
        if (src->type != TOKEN_NUMBER) {
            throwWarn("unsuported source type for power!\n");
            return 1;
        }
        if (dest->type != TOKEN_NUMBER) {
            throwWarn("unsuported destination type for power!\n");
            return 1;
        }
        dest->value = pow(dest->value, src->value);
        return 0;
    }},
    {OP_MODULUS, [](DataToken* dest, DataToken* src) {
        if (src->type != TOKEN_NUMBER) {
            throwWarn("unsuported source type for modulus!\n");
            return 1;
        }
        if (dest->type != TOKEN_NUMBER) {
            throwWarn("unsuported destination type for modulus!\n");
            return 1;
        }
        dest->value = fmod(dest->value, src->value);
        return 0;
    }},
    {OP_ASSIGN, [](DataToken* dest, DataToken* src) {
        if (dest->type != TOKEN_TEXT && dest->type != TOKEN_OBJECT) {
            throwWarn("cant assign to this type!\n");
            return 1;
        }
        if (src->type == TOKEN_NUMBER) {
            ObjectSet(dest->name, src->value);
        } else if (src->type == TOKEN_TEXT) {
            ObjectSet(dest->name, src->name);
        } else {
            throwWarn("cant assign this to a variable!\n");
            return 1;
        }
        return 0;
    }},
    {OP_UNKNOWN, [](DataToken* dest, DataToken* src) {
        //not good, should never be executet!
        throwWarn("trying to execute unknown operator!\n");
        dest = dest;
        src = src;
        return 1;
    }}
};
//Token handler functions, externalized from TokenEval for cleanlieness
const std::vector<TokenHandlerType> TokenHandler = {
    {TOKEN_NONE, [](DataToken** currentResult, DataToken* token) {
        currentResult = currentResult; //dummy
        token = token; //dummy
        return TH_CONTINUE;
    }},
    {TOKEN_NUMBER, [](DataToken** currentResult, DataToken* token) {
        **currentResult = DataToken(token->value);
        return TH_CONTINUE;
    }},
    {TOKEN_OPERATOR, [](DataToken** currentResult, DataToken* token) {
        DataToken* resultNext = new DataToken(0);
        if (token->nextToken) {
            resultNext = TokenEval(token->nextToken);
        }
        for (auto it = OpFunctions.begin(); it != OpFunctions.end(); it++) {
            if (it->type == token->operatorType) {
                it->function(*currentResult, resultNext);
                (*currentResult)->subToken = resultNext->subToken;
                resultNext->subToken = NULL;
                delete resultNext;
                return TH_EXIT;
            }
        }
        throwWarn("Operation not defined!\n");
        (*currentResult)->subToken = resultNext->subToken;
        resultNext->subToken = NULL;
        delete resultNext;
        return TH_EXIT;
    }},
    {TOKEN_SUBTOKEN, [](DataToken** currentResult, DataToken* token) {
        DataToken* resultSub = new DataToken(0);
        if (token->subToken) {
            resultSub = TokenEval(token->subToken);
            (**currentResult) = *resultSub;
            for (; (*currentResult)->subToken != NULL; *currentResult = (*currentResult)->subToken);
        } else {
            throwWarn("Subtoken without subtoken!\n");
        }
        resultSub->subToken = NULL;
        resultSub->nextToken = NULL;
        delete resultSub;
        return TH_CONTINUE;
    }},
    {TOKEN_OBJECT, [](DataToken** currentResult, DataToken* token) {
        switch(ObjectGetType(token->name)) {
            case OBJ_NONE:
            default:
                throwWarn("Object " + token->name + " is not defined!\n");
                delete (*currentResult);
                *currentResult = new DataToken(0);
                break;
            case OBJ_NUMBER:
            case OBJ_TEXT:
                **currentResult = ObjectGet(token->name);
                break;
            case OBJ_FUNCTION:
            case OBJ_CFUNCTION:
                if (token->nextToken == NULL) {
                    **currentResult = ObjectCall(token->name, DataToken(0));
                    for (; (*currentResult)->subToken != NULL; *currentResult = (*currentResult)->subToken);
                    return TH_CONTINUE;
                }
                if (token->nextToken->type == TOKEN_SUBTOKEN) {
                    DataToken* parameters = TokenEval(token->nextToken->subToken);
                    **currentResult = ObjectCall(token->name, *parameters);
                    delete parameters;
                    for (; (*currentResult)->subToken != NULL; *currentResult = (*currentResult)->subToken);
                    return TH_SKIP;

                }
                DataToken* parameter = TokenEval(token->nextToken);
                DataToken* subTokens = parameter->subToken;
                parameter->subToken = NULL;
                **currentResult = ObjectCall(token->name, *parameter);
                delete parameter;
                for (; (*currentResult)->subToken != NULL; *currentResult = (*currentResult)->subToken);
                (*currentResult)->subToken = subTokens;
                subTokens = NULL;
                return TH_EXIT;
        }
        return TH_CONTINUE;
    }},
    {TOKEN_TEXT, [](DataToken** currentResult, DataToken* token) {
        **currentResult = DataToken(token->name);
        return TH_CONTINUE;
    }},
    {TOKEN_ARGSEPERATOR, [](DataToken** currentResult, DataToken* token) {
        token = token; //dummy
        (*currentResult)->subToken = new DataToken(0);
        *currentResult = (*currentResult)->subToken;
        return TH_CONTINUE;
    }},
    {TOKEN_UNKNOWN, [](DataToken** currentResult, DataToken* token) {
        throwWarn("Trying to executate unknown token type!\n");
        currentResult = currentResult; //dummy
        token = token; //dummy
        return TH_CONTINUE;
    }}
};
//Evaluates a token chain returning the result
//If multiples results get returned then they get returned as a linked list atached to subToken
DataToken* TokenEval(DataToken* tokenList) {
    DataToken* rootResult = new DataToken(0);
    DataToken* current = rootResult;

    if (tokenList == NULL) return rootResult;

    for (DataToken* tk = tokenList; tk != NULL; tk = tk->nextToken) {
        bool exec = false;
        for (auto it = TokenHandler.begin(); it != TokenHandler.end(); it++) {
            if (it->type == tk->type) {
                int retcode = it->function(&current, tk);
                if (retcode == TH_EXIT) return rootResult;
                if (retcode == TH_ERROR) throwWarn("Token Handler returned Error!\n");
                if (retcode == TH_SKIP) {
                    tk = tk->nextToken;
                    if (tk == NULL) return rootResult;
                }
                exec = true;
                break;
            }
        }
        if (!exec) throwWarn("Could not execute token!\n");
    }
    return rootResult;
}

//String tokenization system
bool charIsNumber(char c) {
    if ((c >= '0' && c <= '9') || (c == '.')) return true;
    return false;
}
bool charIsAlpha(char c) {
    if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c == '_')) return true;
    return false;
}
//Takes a string and tokenizes it returning a linked list of tokens to execute
DataToken* TokenizeString(std::string input) {
    DEBUG_PRINT("## Called TokenizeString");
    DataToken* rootToken = new DataToken();
    DataToken* currentToken = rootToken;

    for (auto it = input.begin(); it != input.end(); it++) {
        //handle sub token
        if (*it == '(') {
            DEBUG_PRINT("## char belongs to subtoken");
            //entering subtoken

            int subLevel = 0;
            auto subIt = it;
            bool skip = false;
            for (;subIt != input.end(); subIt++) {
                if (*subIt == '\\' && skip == false) {
                    skip = true;
                    continue;
                }
                if (*subIt == '(' && skip == false) subLevel++;
                if (*subIt == ')' && skip == false) {
                    subLevel--;
                    if (subLevel == 0) {
                        //found end of sub
                        //spans from it to subIt exclusive
                        auto startIt = it + 1;
                        auto endIt = subIt;
                        if (startIt < endIt) {
                            //normal sub containing stuff
                            std::string subString = std::string(startIt, endIt);
                            DataToken* subToken = TokenizeString(subString);
                            currentToken->type = TOKEN_SUBTOKEN;
                            currentToken->subToken = subToken;
                            currentToken->nextToken = new DataToken();
                            currentToken = currentToken->nextToken;
                        } else {
                            //sub doesnt contain enything (e.g. "()")
                            currentToken->type = TOKEN_SUBTOKEN;
                            currentToken->subToken = new DataToken(0);
                            currentToken->nextToken = new DataToken();
                            currentToken = currentToken->nextToken;
                        }
                        break;
                    }
                }
                skip = false;
            }
            if (subLevel != 0) {
                //unclosed bracket, cant compute
                throwWarn("Error: unclosed bracket '('!\n");
                delete rootToken;
                rootToken = new DataToken;
                *rootToken = DataToken(0);
                return rootToken;
            }
            it = subIt;
        } else 
        //handle text string
        if (*it == '"') {
            DEBUG_PRINT("## char belongs to string");
            //entering string

            auto endIt = it + 1;
            bool skip = false;
            for (; endIt != input.end(); endIt++) {
                if (*endIt == '\\' && skip == false) {
                    skip = true;
                    continue;
                }
                if (*endIt ==  '"' && skip == false) {
                    //end of string reached
                    break;
                }
                skip = false;
            }

            if (endIt != input.end()) {
                //string is valid
                std::string subString = std::string(it + 1, endIt);
                currentToken->type = TOKEN_TEXT;
                currentToken->name = subString.c_str();
                currentToken->nextToken = new DataToken;
                currentToken = currentToken->nextToken;
                it = endIt;
            } else {
                //string has not been closed off! bad!
                throwWarn("String has not been closed off!\n");
                delete rootToken;
                rootToken = new DataToken;
                *rootToken = DataToken(0);
                return rootToken;
            }
        } else
        //handle number
        if (charIsNumber(*it) == true) {
            DEBUG_PRINT("## char belongs to number");
            //handle a number

            auto loopIt = it;
            for (; charIsNumber(*loopIt) == true; loopIt++);
            //number from it to loopIt
            std::string subStr = std::string(it, loopIt);
            it = loopIt - 1;
            double val = std::stod(subStr);
            *currentToken = DataToken(val);
            DEBUG_PRINT("curVal: " << currentToken->value);
            currentToken->nextToken = new DataToken();
            currentToken = currentToken->nextToken;
            if (charIsAlpha(*loopIt) == true) {
                //implied multiplication
                *currentToken = DataToken(OP_MULTIPLICATION);
                currentToken->nextToken = new DataToken();
                currentToken = currentToken->nextToken;
            }
        } else
        //handle object
        if (charIsAlpha(*it) == true) {
            DEBUG_PRINT("## char belongs to object");
            //handle text object

            auto loopIt = it;
            for (; (charIsAlpha(*loopIt) == true) || (charIsNumber(*loopIt) == true); loopIt++);
            //text from beginIt to loopIt
            std::string subStr = std::string(it, loopIt);
            it = --loopIt;
            *currentToken = DataToken(subStr, true);
            currentToken->nextToken = new DataToken();
            currentToken = currentToken->nextToken;
        } else {
            //handle operator
            DEBUG_PRINT("## char might be operator");

            switch (*it) {
                case '+':
                    *currentToken = DataToken(OP_ADDITION);
                    currentToken->nextToken = new DataToken();
                    currentToken = currentToken->nextToken;
                    break;
                case '-':
                    *currentToken = DataToken(OP_SUBTRACTION);
                    currentToken->nextToken = new DataToken();
                    currentToken = currentToken->nextToken;
                    break;
                case '*':
                    *currentToken = DataToken(OP_MULTIPLICATION);
                    currentToken->nextToken = new DataToken();
                    currentToken = currentToken->nextToken;
                    break;
                case '/':
                    *currentToken = DataToken(OP_DIVISION);
                    currentToken->nextToken = new DataToken();
                    currentToken = currentToken->nextToken;
                    break;
                case '^':
                    *currentToken = DataToken(OP_POWER);
                    currentToken->nextToken = new DataToken();
                    currentToken = currentToken->nextToken;
                    break;
                case '%':
                    *currentToken = DataToken(OP_MODULUS);
                    currentToken->nextToken = new DataToken();
                    currentToken = currentToken->nextToken;
                    break;
                case '=':
                    *currentToken = DataToken(OP_ASSIGN);
                    currentToken->nextToken = new DataToken();
                    currentToken = currentToken->nextToken;
                    break;
                case ';':
                    currentToken->type = TOKEN_ARGSEPERATOR;
                    currentToken->nextToken = new DataToken();
                    currentToken = currentToken->nextToken;
                    break;
                case ' ':
                case '\t':
                    //not an op but best place to handle whitespace
                    break;
                default:
                     DEBUG_PRINT("## char is unknown");
                    //at this point whatever this character is it doesnt belong in an equation
                    std::string warn = "Character \"";
                    warn.push_back(*it);
                    warn += "\" could not be parsed!\n";
                    throwWarn(warn);
                    break;
            }
        }
        DEBUG_PRINT("## it-> : " << *it);
        DEBUG_PRINT("## end of loop");
    }
    return rootToken;
}


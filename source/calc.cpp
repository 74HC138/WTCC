#include <vector>

#include "types.hpp"
#include "helper.hpp"
#include "malloc.h"
#include "math.h"

std::vector<DataVariable> variableStorage;
std::vector<DataFunction> functionStorage;
std::vector<DataObject> objectStorage;

DataToken TokenConstructNumber(double value) {
    DataToken returnToken = {TOKEN_NUMBER, NULL, value, OP_NONE, NULL, NULL};
    return returnToken;
}
void TokenFree(DataToken* token) {
    if (token->nextToken) TokenFree(token->nextToken);
    if (token->subToken) TokenFree(token->subToken);
    free(token);
}
void TokenFree(DataToken rootToken) {
    if (rootToken.nextToken) TokenFree(rootToken.nextToken);
    if (rootToken.subToken) TokenFree(rootToken.subToken);
}
void TokenAppendSubtoken(DataToken* base, DataToken* ext) {
    while (base->subToken != NULL) base = base->subToken;
    base->subToken = ext;
}

DataToken ObjectGet(std::string name) {
    for (auto it = objectStorage.begin(); it != objectStorage.end(); it++) {
        if (name.compare(it->name) == 0) {
            DataToken retToken;
            switch (it->type) {
                case OBJ_NUMBER:
                    retToken = TokenConstructNumber(it->value);
                    break;
                case OBJ_TEXT:
                    retToken = {TOKEN_TEXT, NULL, 0, OP_NONE, NULL, it->text};
                    break;
                case OBJ_FUNCTION:
                case OBJ_NONE:
                default:
                    //cant get a function
                    retToken = TokenConstructNumber(0);
            }
            return retToken;
        }
    }
    throwWarn("Object " + name + " does not exist\n");
    return TokenConstructNumber(0);
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
void ObjectSet(std::string name, DataToken (*function)(DataToken)) {
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
            if (it->type == OBJ_FUNCTION) {
                return it->function(data);
            } else {
                throwWarn("Attempted to call " + name + " a non function\n");
                return TokenConstructNumber(0);
            }
        }
    }
    throwWarn("Object " + name + " does not exist\n");
    return TokenConstructNumber(0);
}

DataToken TokenCalc(DataToken* tokenList) {
    DataToken rootResult = TokenConstructNumber(0);
    DataToken* current = &rootResult;

    while (true) {

        DataToken resultNext;
        DataToken subToken;
        DataToken* subTokenTmp;
        switch (tokenList->type) {
            case TOKEN_NUMBER:
                current->value = tokenList->value;
                break;
            case TOKEN_OPERATOR:
                resultNext = TokenConstructNumber(0);
                if (tokenList->nextToken == NULL) {
                    //token list is cut off, assume 0 and throw warning
                    throwWarn("Token list is cut off, assuming 0.\n");
                } else {
                    resultNext = TokenCalc(tokenList->nextToken);
                }
                if (resultNext.type != TOKEN_NUMBER) {
                    if (resultNext.type == TOKEN_TEXT) {
                        //seems like we hit text... store
                        //since we have already ingested all tokens after this we just return
                        *current = resultNext;
                        return rootResult;
                    } else {
                        //we cant deal with anything but number... assume 0 and throw warning
                        resultNext = {TOKEN_NUMBER, NULL, 0, OP_NONE, resultNext.subToken, NULL};
                        throwWarn("unexpected result from TokenCalc, assuming 0!\n");
                    }
                }
                if (current->type == TOKEN_TEXT) {
                    //we cant do math on text, so we add the subtokens from our result to our token list and return
                    current->subToken = resultNext.subToken;
                    return rootResult;
                }
                switch (tokenList->operatorType) {
                    case OP_ADDITION:
                        current->value = current->value + resultNext.value;
                        break;
                    case OP_SUBTRACTION:
                        current->value = current->value - resultNext.value;
                        break;
                    case OP_MULTIPLICATION:
                        current->value = current->value * resultNext.value;
                        break;
                    case OP_DIVISION:
                        if (resultNext.value == 0) {
                            //zero division... cant have that!
                            //throw warning and return 0
                            throwWarn("Division by 0!\n");
                            *current = {TOKEN_NUMBER, NULL, 0, OP_NONE, resultNext.subToken, NULL};
                            return rootResult;
                        }
                        current->value = current->value / resultNext.value;
                        break;
                    case OP_POWER:
                        current->value = pow(current->value, resultNext.value);
                        break;
                    case OP_MODULUS:
                        current->value = fmod(current->value, resultNext.value);
                        break;
                    case OP_NONE:
                    case OP_UNKNOWN:
                    default:
                        //woops, hit undefined operator... thats a fail
                        throwWarn("Hit undefined operator!\n");
                        current->subToken = resultNext.subToken;
                        return rootResult;
                        break;
                }
                //return from here because all data is allready ingested
                return rootResult;
            case TOKEN_SUBTOKEN:
                subToken = TokenConstructNumber(0);
                if (tokenList->subToken) {
                    subToken = TokenCalc(tokenList->subToken);
                } else {
                    //hit a sub token without a subtoken... strange
                    throwWarn("Hit sub token without defined subtoken!\n");
                }
                if (subToken.type != TOKEN_NUMBER) {
                    if (subToken.type == TOKEN_TEXT) {
                        //got text from subtoken... propegate up
                        *current = subToken;
                        break;
                    } else {
                        //got something else... bad... assume 0 and contine
                        throwWarn("unexpected result from subtoken!\n");
                        *current = TokenConstructNumber(0);
                        current->subToken = subToken.subToken;
                        break;
                    }
                }
                *current = subToken;
                break;
            case TOKEN_VARIABLE:
                *current = ObjectGet(tokenList->name);
                break;
            case TOKEN_FUNCTION:
                //if we have more then one parameter for the function we need to check if the next token
                //is a subtoken, evaluate that and give all the parameters to the function

                if (tokenList->nextToken == NULL) {
                    //we have no arguments for the function, call it with zero argument
                    *current = ObjectCall(tokenList->name, TokenConstructNumber(0));
                    return rootResult;
                } else {
                    if (tokenList->nextToken->type == TOKEN_SUBTOKEN) {
                        if (tokenList->nextToken->subToken == NULL) {
                            //sub token without subtoken...
                            throwWarn("hit sub token without sub token!\n");
                            *current = ObjectCall(tokenList->name, TokenConstructNumber(0));
                            break;
                        } else {
                            resultNext = TokenCalc(tokenList->nextToken->subToken);
                            *current = ObjectCall(tokenList->name, resultNext);
                        }
                        tokenList = tokenList->nextToken;
                    } else {
                        resultNext = TokenCalc(tokenList->nextToken);
                        subTokenTmp = resultNext.subToken;
                        resultNext.subToken = NULL;
                        *current = ObjectCall(tokenList->name, resultNext);
                        TokenAppendSubtoken(current, subTokenTmp);
                        return rootResult;
                    }
                }
                break;
            case TOKEN_TEXT:
                //if token is text we store that for now
                *current = {TOKEN_TEXT, NULL, current->value, OP_NONE, NULL, tokenList->name};
                break;
            case TOKEN_ARGSEPERATOR:
                current->subToken = (DataToken*) malloc(sizeof(DataToken));
                *current->subToken = TokenConstructNumber(0);
                current = current->subToken;
                break;
            case TOKEN_NONE:
            case TOKEN_UNKNOWN:
            default:
                //if first token is a NONE token just return 0 and throw an error for good measure
                throwWarn("hit TOKEN_UNKNOWN or TOKEN_NONE!\n");
                break;
        }
    
        if (tokenList->nextToken == NULL) break;
        tokenList = tokenList->nextToken;
    }

    return rootResult;
}
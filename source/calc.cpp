#include <vector>
#include <iostream>

#include "calc.hpp"
#include "types.hpp"
#include "helper.hpp"
#include "malloc.h"
#include "math.h"

std::vector<DataObject> objectStorage;

DataToken TokenConstructNumber(double value) {
    DataToken returnToken = DataToken(0);
    return returnToken;
}
void TokenFree(DataToken* token) {
    delete token;
}
void TokenFree(DataToken rootToken) {
    rootToken.~DataToken();
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
                    retToken = DataToken(it->value);
                    break;
                case OBJ_TEXT:
                    retToken = DataToken(it->text);
                    break;
                case OBJ_FUNCTION:
                case OBJ_NONE:
                default:
                    //cant get a function
                    retToken = DataToken(0);
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
                return TokenEval(it->function); //wtcc functions do not support parameters for now. TODO
            } else {
                throwWarn("Attempted to call " + name + " a non function\n");
                return TokenConstructNumber(0);
            }
        }
    }
    throwWarn("Object " + name + " does not exist\n");
    return TokenConstructNumber(0);
}

//Evaluates a token chain returning the result
//If multiples results get returned then they get returned as a linked list atached to subToken
DataToken TokenEval(DataToken* tokenList) {
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
                    resultNext = TokenEval(tokenList->nextToken);
                }
                if (resultNext.type != TOKEN_NUMBER) {
                    if (resultNext.type == TOKEN_TEXT) {
                        //seems like we hit text... store
                        //since we have already ingested all tokens after this we just return
                        *current = resultNext;
                        return rootResult;
                    } else {
                        //we cant deal with anything but number... assume 0 and throw warning
                        resultNext.~DataToken();
                        resultNext = DataToken(0);
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
                            current->~DataToken();
                            *current = DataToken(0);
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
                    subToken = TokenEval(tokenList->subToken);
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

            case TOKEN_OBJECT:
                switch (ObjectGetType(tokenList->name)) {
                    case OBJ_NONE:
                    default:
                        throwWarn("Object " + tokenList->name + "is not defined!");
                        *current = DataToken(0);
                        break;
                    case OBJ_NUMBER:
                    case OBJ_TEXT:
                        *current = ObjectGet(tokenList->name);
                        break;
                    case OBJ_FUNCTION:
                    case OBJ_CFUNCTION:
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
                                    resultNext = TokenEval(tokenList->nextToken->subToken);
                                    *current = ObjectCall(tokenList->name, resultNext);
                                }
                                tokenList = tokenList->nextToken;
                            } else {
                                resultNext = TokenEval(tokenList->nextToken);
                                subTokenTmp = resultNext.subToken;
                                resultNext.subToken = NULL;
                                *current = ObjectCall(tokenList->name, resultNext);
                                TokenAppendSubtoken(current, subTokenTmp);
                                return rootResult;
                            }
                        }
                        break;

                }
                break;
            case TOKEN_TEXT:
                //if token is text we store that for now
                *current = DataToken(tokenList->name);
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

bool charIsNumber(char c) {
    if ((c >= '0' && c <= '9') || (c == '.')) return true;
    return false;
}

bool charIsAlpha(char c) {
    if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c == '_')) return true;
    return false;
}

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
                *rootToken = TokenConstructNumber(0);
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
                *rootToken = TokenConstructNumber(0);
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
            it = --loopIt;
            double val = std::stod(subStr);
            *currentToken = DataToken(val);
            DEBUG_PRINT("curVal: " << currentToken->value);
            currentToken->nextToken = new DataToken();
            currentToken = currentToken->nextToken;
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


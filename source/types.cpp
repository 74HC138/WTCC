#include "types.hpp"
#include "helper.hpp"
#include <iostream>

DataToken::DataToken() {
    DEBUG_PRINT("[Constructing empty DataToken]");
    type = TOKEN_NONE;
    nextToken = NULL;
    subToken = NULL;
}
DataToken::DataToken(double v) {
    DEBUG_PRINT("[Constructing number DataToken]");
    this->type = TOKEN_NUMBER;
    this->value = v;
    nextToken = NULL;
    subToken = NULL;
}
DataToken::DataToken(std::string v, bool isObject) {
    if (isObject) {
        DEBUG_PRINT("[Constructing Object DataToken]");
        this->type = TOKEN_OBJECT;
    } else {
        DEBUG_PRINT("[Constructing text DataToken]");
        this->type = TOKEN_TEXT;
    }
    this->name = v;
    nextToken = NULL;
    subToken = NULL;
}
DataToken::DataToken(OperatorType op) {
    DEBUG_PRINT("[Constructing operator DataToken]");
    this->type = TOKEN_OPERATOR;
    this->operatorType = op;
    nextToken = NULL;
    subToken = NULL;
}
DataToken::~DataToken() {
    DEBUG_PRINT("[Deconstructing DataToken]");
    if (nextToken) delete nextToken;
    if (subToken) delete subToken;
    name.clear();
}
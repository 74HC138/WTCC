#include <iostream>
#include "helper.hpp"

void throwWarn(std::string message) {
    std::cout << message;
}

void tokenDebug(DataToken* token, int indent) {
    for (DataToken* tk = token; tk != NULL; tk = tk->nextToken) {
        for (int i = 0; i < indent; i++) std::cout << "\t";
        TokenType type = tk->type;
        switch (type) {
            case TOKEN_NONE:
                std::cout << "(TOKEN_NONE)\n";
                break;
            case TOKEN_NUMBER:
                std::cout << "(TOKEN_NUMBER) = " << tk->value << "\n";
                break;
            case TOKEN_OPERATOR:
                std::cout << "(TOKEN_OPERATOR) ";
                switch (tk->operatorType) {
                    case OP_NONE:
                        std::cout << "[OP_NONE]\n";
                        break;
                    case OP_ADDITION:
                        std::cout << "+\n";
                        break;
                    case OP_SUBTRACTION:
                        std::cout << "-\n";
                        break;
                    case OP_MULTIPLICATION:
                        std::cout << "*\n";
                        break;
                    case OP_DIVISION:
                        std::cout << "/\n";
                        break;
                    case OP_POWER:
                        std::cout << "^\n";
                        break;
                    case OP_MODULUS:
                        std::cout << "%\n";
                        break;
                    case OP_UNKNOWN:
                        std::cout << "[OP_UNKNOWN]\n";
                        break;
                    default:
                        std::cout << "[!OPERATOR OUT OF RANGE!]\n";
                        break;
                }
                break;
            case TOKEN_SUBTOKEN:
                std::cout << "(TOKEN_SUBTOKEN) (\n";
                if (tk->subToken) {
                    tokenDebug(tk->subToken, indent + 1);
                } else {
                    for (int i = 0; i < indent + 1; i++) std::cout << "\t";
                    std::cout << "[subToken not defined]\n";
                }
                for (int i = 0; i < indent; i++) std::cout << "\t";
                std::cout << ")\n";
                break;
            case TOKEN_OBJECT:
                std::cout << "(TOKEN_OBJECT) " << tk->name << "\n";
                break;
            case TOKEN_TEXT:
                std::cout << "(TOKEN_TEXT) \"" << tk->name << "\"\n";
                break;
            case TOKEN_ARGSEPERATOR:
                std::cout << "###### (TOKEN_ARGSEPERATOR) ######";
                break;
            case TOKEN_UNKNOWN:
                std::cout << "(TOKEN_UNKNOWN)\n";
                break;
            default:
                std::cout << "[!TYPE OUT OF RANGE!]\n";
                break;
        }
    }
}
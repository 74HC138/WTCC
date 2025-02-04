#include <iostream>

#include "types.hpp"
#include "helper.hpp"
#include "calc.hpp"

DataToken testFunction(DataToken data) {
    DataToken* current = &data;
    int cnt = 1;
    std::cout << "Test function:\n";
    while (true) {
        std::cout << "Token #" << cnt << " is ";
        if (current->type == TOKEN_NUMBER) {
            std::cout << "NUMBER with value " << current->value << "\n";
        } else if (current->type == TOKEN_TEXT) {
            std::cout << "TEXT with value \"" << current->name << "\"\n";
        } else {
            std::cout << "UNKNOWN!\n";
        }
        if (current->subToken == NULL) break;
        current = current->subToken;
        cnt++;
    }
    return data;
}

int main() {
    std::cout << "testing string tokenizer.\nEnter string: ";
    std::string string;
    std::getline(std::cin, string);
    
    DataToken* token = TokenizeString(string);

    tokenDebug(token);

    std::cout << "Evaluating token:\n";

    DataToken output = TokenEval(token);

    tokenDebug(&output);
    
    return 0;
}
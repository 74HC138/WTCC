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
    ObjectSet("test", &testFunction);
    ObjectSet("varA", 42);

    DataToken test[] = {
        {TOKEN_FUNCTION, &test[1], 0, OP_NONE, NULL, "test"},
        {TOKEN_SUBTOKEN, NULL, 0, OP_NONE, &test[2], NULL},
            {TOKEN_NUMBER, &test[3], 42, OP_NONE, NULL, NULL},
            {TOKEN_ARGSEPERATOR, &test[4], 0, OP_NONE, NULL, NULL},
            {TOKEN_TEXT, &test[5], 0, OP_NONE, NULL, "Die Antwort auf alles"},
            {TOKEN_ARGSEPERATOR, &test[6], 0, OP_NONE, NULL, NULL},
            {TOKEN_VARIABLE, NULL, 0, OP_NONE, NULL, "varA"}
    };
    
    std::cout << "Testing function call, variables and argument seperator\n";
    DataToken result = TokenCalc(test);

    if (result.type == TOKEN_NUMBER) {
        std::cout << "result is " << result.value << "\n";
    } else {
        std::cout << "this has not worked... the result is not a number!\n";
    }

    TokenFree(result);

    return 0;
}
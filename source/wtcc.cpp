#include <iostream>

#include "types.hpp"
#include "helper.hpp"
#include "calc.hpp"

int main() {
    DataToken test[] = {
        {TOKEN_NUMBER, &test[1], 2, OP_NONE, NULL, NULL},
        {TOKEN_OPERATOR, &test[2], 0, OP_ADDITION, NULL, NULL},
        {TOKEN_NUMBER, NULL, 2, OP_NONE, NULL, NULL}
    };
    
    std::cout << "testing if 2+2 equals 4...\n";
    DataToken result = TokenCalc(test);

    if (result.type == TOKEN_NUMBER) {
        std::cout << "result is " << result.value << "\n";
    } else {
        std::cout << "this has not worked... the result is not a number!\n";
    }
    return 0;
}
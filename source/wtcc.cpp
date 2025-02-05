#include <iostream>
#include <math.h>
#include <vector>

#include "types.hpp"
#include "helper.hpp"
#include "calc.hpp"

#define VERSION "v0.2"

struct ConstFn {
    std::string name;
    DataToken (*function)(DataToken);
};
const std::vector<ConstFn> constantFunctions = {
    //trigonometric functions
    {"sin", [](DataToken tk) {
        if (tk.type != TOKEN_NUMBER) return DataToken(0);
        return DataToken(sin(tk.value));
    }},
    {"cos", [](DataToken tk) {
        if (tk.type != TOKEN_NUMBER) return DataToken(0);
        return DataToken(cos(tk.value));
    }},
    {"tan", [](DataToken tk) {
        if (tk.type != TOKEN_NUMBER) return DataToken(0);
        return DataToken(tan(tk.value));
    }},
    {"cot", [](DataToken tk) {
        if (tk.type != TOKEN_NUMBER) return DataToken(0);
        return DataToken(cos(tk.value) / sin(tk.value));
    }},
    {"asin", [](DataToken tk) {
        if (tk.type != TOKEN_NUMBER) return DataToken(0);
        return DataToken(asin(tk.value));
    }},
    {"acos", [](DataToken tk) {
        if (tk.type != TOKEN_NUMBER) return DataToken(0);
        return DataToken(acos(tk.value));
    }},
    {"atan", [](DataToken tk) {
        if (tk.type != TOKEN_NUMBER) return DataToken(0);
        return DataToken(atan(tk.value));
    }},
    {"acot", [](DataToken tk) {
        if (tk.type != TOKEN_NUMBER) return DataToken(0);
        return DataToken(M_PI_2 - atan(tk.value));
    }},
    //numeric functions
    {"round", [](DataToken tk) {
        if (tk.type != TOKEN_NUMBER) return DataToken(0);
        return DataToken(round(tk.value));
    }},
    {"ceil", [](DataToken tk) {
        if (tk.type != TOKEN_NUMBER) return DataToken(0);
        return DataToken(ceil(tk.value));
    }},
    {"floor", [](DataToken tk) {
        if (tk.type != TOKEN_NUMBER) return DataToken(0);
        return DataToken(floor(tk.value));
    }},
    {"min", [](DataToken tk) {
        double val = INFINITY;
        for (DataToken* t = &tk; t != NULL; t = t->subToken) {
            if (t->type == TOKEN_NUMBER) if (t->value < val) val = t->value;
        }
        return DataToken(val);
    }},
    {"max", [](DataToken tk) {
        double val = INFINITY * -1;
        for (DataToken* t = &tk; t != NULL; t = t->subToken) {
            if (t->type == TOKEN_NUMBER) if (t->value > val) val = t->value;
        }
        return DataToken(val);
    }},
    {"abs", [](DataToken tk) {
        if (tk.type != TOKEN_NUMBER) return DataToken(0);
        return DataToken(abs(tk.value));
    }},
    //general functions
    {"help", [](DataToken tk) {
        tk.type = tk.type; //dummy to cirumvent unused argument warning
        std::cout << "\n----------------------------------\n";
        std::cout << "WTCC help:\n";
        std::cout << "WTCC tokenizes and evaluates a math function. It can do basic arithmatic, call functions and assign and use variables.\n";
        std::cout << "TODO proper help text\n";
        std::cout << "Use  \"list\" to list all objects in the object table\n";
        std::cout << "----------------------------------\n";
        return DataToken(0);
    }},
    {"list", [](DataToken tk) {
        tk.type = tk.type; //dummy to cirumvent unused argument warning
        std::cout << "\n----------------------------------\n";
        std::cout << "Object table:\n";
        for (auto it = ObjectGetRawIterator(); it != ObjectGetRawEnditerator(); it++) {
            switch (it->type) {
                case OBJ_NONE:
                    break;
                case OBJ_NUMBER:
                    std::cout << "var \"" << it->name << "\" = " << it->value << "\n";
                    break;
                case OBJ_TEXT:
                    std::cout << "var \"" << it->name << "\" = \"" << it->text << "\"\n";
                    break;
                case OBJ_FUNCTION:
                case OBJ_CFUNCTION:
                    std::cout << "fn \"" << it->name << "\"\n";
                    break;
                default:
                    break;
            }
        }
        std::cout << "----------------------------------\n";
        return DataToken(0);
    }},
    {"print", [](DataToken tk) {
        switch (tk.type) {
            case TOKEN_NUMBER:
                std::cout << tk.value << "\n";
                break;
            case TOKEN_TEXT:
                std::cout << tk.name << "\n";
                break;
            case TOKEN_NONE:
            case TOKEN_OPERATOR:
            case TOKEN_SUBTOKEN:
            case TOKEN_OBJECT:
            case TOKEN_ARGSEPERATOR:
            case TOKEN_UNKNOWN:
            default:
                break;
        }
        return DataToken(0);
    }},
    {"exit", [](DataToken tk) {
        tk.type = tk.type; //dummy to cirumvent unused argument warning
        if (tk.type == TOKEN_NUMBER) {
            exit((int) tk.value);
        } else exit(0);
        return DataToken(0);
    }},
    //object functions
    {"delete", [](DataToken tk) {
        if (tk.type != TOKEN_TEXT) {
            throwWarn("Cant delete this type\n");
            return DataToken(1);
        }
        ObjectDelete(tk.name);
        return DataToken(0);
    }}
};
struct ConstN {
    std::string name;
    double n;
};
const std::vector<ConstN> constantValue = {
    {"e", M_E},
    {"pi", M_PI},
    {"k", 1e3},
    {"M", 1e6},
    {"G", 1e9},
    {"m", 1e-3},
    {"µ", 1e-6},
    {"n", 1e-9}
};

void populateObjects() {
    for (auto it = constantFunctions.begin(); it != constantFunctions.end(); it++) {
        ObjectSet(it->name, it->function);
    }
    for (auto it = constantValue.begin(); it != constantValue.end(); it++) {
        ObjectSet(it->name, it->n);
    }
    
}

int main() {
    std::cout << "WTCC version " << VERSION << "\n";

    populateObjects();

    std::cout << "Object table initialized.\n";
    std::cout << "Use help to get started.\n";

    while (42) {
        std::cout << "-> ";
        std::string input;
        std::getline(std::cin, input);
        
        DataToken* tokenized = TokenizeString(input);
        #ifdef DEBUG
            tokenDebug(tokenized);
        #endif

        DataToken* output = TokenEval(tokenized);
        #ifdef DEBUG
            tokenDebug(output);
        #endif

        if (output->subToken) {
            std::cout << "= {";
            bool addPrefix = false;
            for (DataToken* tk = output; tk != NULL; tk = tk->subToken) {
                if (addPrefix) std::cout << ";";
                addPrefix = true;
                switch (tk->type) {
                    case TOKEN_NUMBER:
                        std::cout << tk->value;
                        break;
                    case TOKEN_TEXT:
                        std::cout << "\"" << tk->name << "\"";
                        break;
                    case TOKEN_NONE:
                    case TOKEN_OPERATOR:
                    case TOKEN_SUBTOKEN:
                    case TOKEN_OBJECT:
                    case TOKEN_ARGSEPERATOR:
                    case TOKEN_UNKNOWN:
                    default:
                        break;
                }
            }
            std::cout << "}\n";
        } else {
            std::cout << "= ";
            switch (output->type) {
                case TOKEN_NUMBER:
                    std::cout << output->value;
                    break;
                case TOKEN_TEXT:
                    std::cout << "\"" << output->name << "\"";
                    break;
                case TOKEN_NONE:
                case TOKEN_OPERATOR:
                case TOKEN_SUBTOKEN:
                case TOKEN_OBJECT:
                case TOKEN_ARGSEPERATOR:
                case TOKEN_UNKNOWN:
                default:
                    break;
            }
            std::cout << "\n";
        }

        delete output;
    }
    
    return 0;
}
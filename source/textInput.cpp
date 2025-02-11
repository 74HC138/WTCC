#include "textInput.hpp"

#include <vector>
#include <iostream>
#include <stdio.h>

std::vector<std::string> inputStorage;

void getCursor(int* x, int* y) {
    std::cout << "\x1b[6n";
    scanf("\x1b[%d;%dR", x, y);
}
void setCursor(int x, int y) {
    std::cout << "\x1b[" << x << ";" << y << "H";
}
void clearLine() {
    std::cout << "\x1b[2K\r";
}


std::string getInput() {
    std::string retString = "";
    std::cout << "-> ";
    int storageOffset = 0;

    while (true) {
        if (!std::cin.eof()) {
            int c = std::cin.get();
            if (c == '\x1b') {
                //special character
                std::cout << "Escape sequence!\n";
                char code;
                scanf("[%c", &code);
                switch (code) {
                    case 'A':
                        //up
                        if (inputStorage.size() > 0) {
                            storageOffset++;
                            unsigned long pos = inputStorage.size() - (unsigned long) storageOffset;
                            if (pos > 0) {
                                clearLine();
                                std::cout << "-> ";
                                retString = inputStorage[pos];
                                std::cout << retString;
                            } else {
                                storageOffset = (int) inputStorage.size();
                            }
                        }
                        break;
                    case 'B':
                        //down
                        if (inputStorage.size() > 0) {
                            storageOffset--;
                            if (storageOffset < 1) storageOffset = 1;
                            unsigned long pos = inputStorage.size() - (unsigned long) storageOffset;
                            clearLine();
                            std::cout << "-> ";
                            retString = inputStorage[pos];
                            std::cout << retString;
                        }
                        break;
                    case 'C':
                        //right
                        //not implimented yet
                        break;
                    case 'D':
                        //left
                        //not implimented yet
                        break;
                    default:
                        break;
                }
            } else if (c == '\b') {
                //back space
                retString.pop_back();
            } else if (c == '\n' || c == '\r') {
                //return
                inputStorage.push_back(retString);
                return retString;
            } else {
                //regular character
                retString += (char) c;
            }
        }
    }
}
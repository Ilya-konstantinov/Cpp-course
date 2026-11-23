#include <cstdint>
#include <iomanip>
#include <iostream>
#include <string>
#include "func.h"

void Print(const NumberFormat& format, std::uint32_t number) {
    std::cout << '|' << format.Format(number) << "|\n";
}

int main() {
    NumberFormat formats[2];
    int count;
    std::cin >> count;
    while (count-- > 0) {
        int index;
        std::string command;
        std::cin >> index >> command;
        NumberFormat& format = formats[index];
        if (command == "BASE") {
            int base;
            std::cin >> base;
            format.SetBase(base);
        } else if (command == "WIDTH") {
            int width;
            std::cin >> width;
            format.SetWidth(width);
        } else if (command == "FILL") {
            std::string fill;
            std::cin >> std::quoted(fill);
            format.SetFill(fill.front());
        } else if (command == "PRINT") {
            std::uint32_t number;
            std::cin >> number;
            Print(format, number);
        }
    }
}

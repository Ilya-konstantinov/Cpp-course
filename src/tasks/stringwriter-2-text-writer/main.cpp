#include <cstdint>
#include <iomanip>
#include <iostream>
#include <limits>
#include <string>
#include "func.h"

void PrintAddress(TextWriter& out, std::uint32_t address) {
    out.SetBase(16);
    out.SetFill('0');
    out.SetWidth(4);
    out.WriteNumber(address);
}

void PrintColumn(const TextWriter& out) {
    std::cout << out.Column() << '\n';
}

int main() {
    int lines;
    std::cin >> lines;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::string buffer;
    for (int i = 0; i < lines; ++i) {
        std::string line;
        std::getline(std::cin, line);
        if (i != 0) buffer += '\n';
        buffer += line;
    }
    TextWriter out(buffer);
    int commands;
    std::cin >> commands;
    for (int i = 0; i < commands; ++i) {
        std::string op;
        std::cin >> op;
        if (op == "BASE" || op == "WIDTH") {
            int value;
            std::cin >> value;
            if (op == "BASE") out.SetBase(value);
            else out.SetWidth(value);
        } else if (op == "FILL" || op == "TEXT") {
            std::string text;
            std::cin >> std::quoted(text);
            if (op == "FILL") out.SetFill(text.front());
            else out.WriteText(text);
        } else if (op == "NL") {
            out.WriteText("\n");
        } else if (op == "NUM" || op == "ADDR") {
            std::uint32_t value;
            std::cin >> value;
            if (op == "ADDR") PrintAddress(out, value);
            else out.WriteNumber(value);
        } else if (op == "COL") {
            PrintColumn(out);
        } else if (op == "SHOW") {
            std::cout << '[' << buffer << "]\n";
        }
    }
}

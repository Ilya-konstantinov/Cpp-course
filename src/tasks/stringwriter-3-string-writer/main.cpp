#include <cstdint>
#include <iomanip>
#include <iostream>
#include <limits>
#include <string>

// Готовое хранилище; доступно в подключаемом решении.
class StringStorage {
    std::string buffer_;
protected:
    explicit StringStorage(const std::string& initial) : buffer_(initial) {}
    std::string& Buffer() { return buffer_; }
    const std::string& Buffer() const { return buffer_; }
};

#include "func.h"

void PrintAddress(TextWriter& out, std::uint32_t address) {
    out.SetBase(16);
    out.SetFill('0');
    out.SetWidth(4);
    out.WriteNumber(address);
}

void RunCommand(StringWriter& writer, const std::string& op) {
    TextWriter& out = writer;
    if (op == "BASE" || op == "WIDTH") {
        int n;
        std::cin >> n;
        if (op == "WIDTH") out.SetWidth(n);
        else out.SetBase(n);
    } else if (op == "FILL" || op == "TEXT") {
        std::string s;
        std::cin >> std::quoted(s);
        if (op == "TEXT") out.WriteText(s);
        else out.SetFill(s.front());
    } else if (op == "NL") {
        out.WriteText("\n");
    } else if (op == "NUM" || op == "ADDR") {
        std::uint32_t n;
        std::cin >> n;
        if (op == "ADDR") PrintAddress(out, n);
        else out.WriteNumber(n);
    } else if (op == "SHOW" || op == "COL") {
        const StringWriter& view = writer;
        if (op == "SHOW") std::cout << '[' << view.Str() << "]\n";
        else std::cout << view.Column() << '\n';
    }
}

int main() {
    std::string initial;
    int count;
    std::cin >> std::quoted(initial) >> count;
    StringWriter seeded(initial);
    StringWriter empty;
    StringWriter* slots[] = {&seeded, &empty};
    while (count-- > 0) {
        int index;
        std::string op;
        std::cin >> index >> op;
        RunCommand(*slots[index], op);
    }
}

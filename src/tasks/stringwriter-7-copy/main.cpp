#include <cstdint>
#include <iomanip>
#include <iostream>
#include <limits>
#include <string>
#include <optional>

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
        if (op == "WIDTH") out << SetWidth(n);
        else if (n == 8) out << Oct();
        else if (n == 10) out << Dec();
        else out << Hex();
    } else if (op == "FILL" || op == "TEXT") {
        std::string s;
        std::cin >> std::quoted(s);
        if (op == "TEXT") out << s;
        else out << SetFill(s.front());
    } else if (op == "NL") {
        out << '\n';
    } else if (op == "NUM" || op == "ADDR") {
        std::uint32_t n;
        std::cin >> n;
        if (op == "ADDR") PrintAddress(out, n);
        else out << n;
    } else if (op == "SHOW" || op == "COL") {
        const StringWriter& view = writer;
        if (op == "SHOW") std::cout << '[' << view.Str() << "]\n";
        else std::cout << view.Column() << '\n';
    } else if (op == "PAIR") {
        std::uint32_t first, second;
        std::cin >> first >> second;
        TextWriter& continuation = out << Dec() << SetFill('0') << SetWidth(2) << first << ':';
        continuation << SetWidth(2) << second;
    } else if (op == "PRE") {
        std::uint32_t n;
        std::cin >> n;
        const TextWriter& view = writer;
        std::cout << '|' << view.Preview(n) << "|\n";
    }
}

int main() {
    std::string initial;
    int count;
    std::cin >> std::quoted(initial) >> count;
    std::optional<StringWriter> slots[2];
    slots[0].emplace(initial);
    slots[1].emplace();
    while (count-- > 0) {
        int index;
        std::string op;
        std::cin >> index >> op;
        if (op == "COPY") slots[1 - index].emplace(*slots[index]);
        else if (op == "ASSIGN") *slots[1 - index] = *slots[index];
        else if (op == "SELF") {
            const StringWriter& source = *slots[index];
            *slots[index] = source;
        } else RunCommand(*slots[index], op);
    }
}

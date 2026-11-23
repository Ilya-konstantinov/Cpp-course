#include <cstdint>
#include <iomanip>
#include <iostream>
#include <limits>
#include <string>
#include <vector>

// Готовое хранилище; доступно в подключаемом решении.
class StringStorage {
    std::string buffer_;
protected:
    explicit StringStorage(const std::string& initial) : buffer_(initial) {}
    std::string& Buffer() { return buffer_; }
    const std::string& Buffer() const { return buffer_; }
};

#include "func.h"

int main() {
    char mode;
    int size;
    std::cin >> mode >> size;
    std::vector<std::uint8_t> bytes(size);
    for (auto& byte : bytes) {
        unsigned value;
        std::cin >> value;
        byte = static_cast<std::uint8_t>(value);
    }
    std::string buffer = "dump:\n";
    TextWriter external(buffer);
    StringWriter owned(buffer);
    TextWriter& out = mode == 'B' ? external : static_cast<TextWriter&>(owned);
    out << Oct() << SetFill('.') << SetWidth(7);
    Dump(out, bytes);
    std::cout << (mode == 'B' ? buffer : owned.Str());
}

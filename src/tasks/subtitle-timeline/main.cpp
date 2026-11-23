#include <iomanip>
#include <iostream>
#include <string>
#include "func.h"

void PrintSubtitle(const Subtitle& item, const std::string& value) {
    std::cout << item.Start() << ' ' << item.Duration() << ' ' << End(item)
              << ' ' << std::quoted(value) << '\n';
}

int main() {
    int start, duration;
    std::string value;
    std::cin >> start >> duration >> std::quoted(value);
    TextSubtitle text(start, duration, value);
    std::cin >> start >> duration >> std::quoted(value);
    ImageSubtitle image(start, duration, value);
    std::cin >> start >> std::quoted(value);
    Marker marker(start, value);

    int count;
    std::cin >> count;
    while (count-- > 0) {
        std::string command;
        char kind;
        std::cin >> command >> kind;
        if (command == "SHIFT") {
            int delta;
            std::cin >> delta;
            if (kind == 'T') Shift(text, delta);
            else if (kind == 'I') Shift(image, delta);
            else Shift(marker, delta);
        } else if (command == "RENAME") {
            std::cin >> std::quoted(value);
            if (kind == 'T') Rename(text, value);
            else Rename(marker, value);
        } else if (kind == 'T') {
            PrintSubtitle(text, text.Text());
        } else if (kind == 'I') {
            PrintSubtitle(image, image.File());
        } else {
            std::cout << marker.Start() << ' ' << std::quoted(marker.Text()) << '\n';
        }
    }
}

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

// Здесь система проверки вставит ваш код. Функцию main писать не нужно.

int main() {
    static_assert(std::is_default_constructible_v<BatteryCell>);
    static_assert(std::is_constructible_v<BatteryCell, int>);
    static_assert(std::is_constructible_v<BatteryCell, int, int>);
    static_assert(std::is_copy_constructible_v<BatteryCell>);
    static_assert(std::is_copy_assignable_v<BatteryCell>);
    static_assert(!std::is_convertible_v<int, BatteryCell>);

    static_assert(std::is_same_v<decltype(&BatteryCell::GetCapacity), int (BatteryCell::*)() const>);
    static_assert(std::is_same_v<decltype(&BatteryCell::GetCharge), int (BatteryCell::*)() const>);
    static_assert(std::is_same_v<decltype(&BatteryCell::GetFreeSpace), int (BatteryCell::*)() const>);
    static_assert(std::is_same_v<decltype(&BatteryCell::TryCharge), bool (BatteryCell::*)(int)>);
    static_assert(std::is_same_v<decltype(&BatteryCell::TryConsume), bool (BatteryCell::*)(int)>);
    static_assert(std::is_same_v<decltype(&BatteryCell::operator+=), BatteryCell& (BatteryCell::*)(int)>);
    static_assert(std::is_same_v<decltype(&BatteryCell::operator-=), BatteryCell& (BatteryCell::*)(int)>);
    static_assert(std::is_same_v<decltype(&BatteryCell::operator==), bool (BatteryCell::*)(const BatteryCell&) const>);
    static_assert(std::is_same_v<decltype(&BatteryCell::operator!=), bool (BatteryCell::*)(const BatteryCell&) const>);

    {
        const BatteryCell initial;
        assert(initial.GetCapacity() == 100 && initial.GetCharge() == 0);
        BatteryCell a(10, 5), b(10, 5), c(11, 5);
        assert(a == b && !(a != b) && a != c);
        assert(!a.TryCharge(-1) && !a.TryConsume(-1) && a == b);
        assert(!a.TryCharge(6) && !a.TryConsume(6) && a == b);
        assert(a.TryCharge(0) && a.TryConsume(0) && a == b);
        assert(&(a += 5) == &a && a.GetCharge() == 10);
        bool caught = false;
        try { a += 1; } catch (const std::out_of_range&) { caught = true; }
        assert(caught && a.GetCharge() == 10);
        assert(&(a -= 10) == &a && a.GetCharge() == 0);
        caught = false;
        try { a -= 1; } catch (const std::out_of_range&) { caught = true; }
        assert(caught && a.GetCharge() == 0);
        for (int capacity : {0, -1, 1000001}) {
            caught = false;
            try { (void)BatteryCell(capacity, 0); } catch (const std::invalid_argument&) { caught = true; }
            assert(caught);
        }
        BatteryCell copy = b;
        copy -= 1;
        assert(b.GetCharge() == 5 && copy.GetCharge() == 4);
    }
    int capacity, charge, commands;
    if (!(std::cin >> capacity >> charge >> commands)) return 1;
    BatteryCell values[3] = {BatteryCell(capacity, charge), BatteryCell(capacity, charge), BatteryCell(capacity, charge)};
    for (int t = 0; t < commands; ++t) {
        std::string command;
        int id;
        if (!(std::cin >> command >> id) || id < 0 || id >= 3) return 1;
        auto& v = values[id];
        if (command == "show") std::cout << v.GetCapacity() << ' ' << v.GetCharge() << ' ' << v.GetFreeSpace() << '\n';
        else if (command == "charge" || command == "consume") {
            int amount; std::cin >> amount;
            std::cout << (command == "charge" ? v.TryCharge(amount) : v.TryConsume(amount)) << '\n';
        } else if (command == "add" || command == "sub") {
            int amount; std::cin >> amount;
            try {
                if (command == "add") v += amount;
                else v -= amount;
                std::cout << "OK\n";
            } catch (const std::out_of_range&) { std::cout << "ERROR\n"; }
        } else if (command == "make") {
            int c, q; std::cin >> c >> q;
            try { v = BatteryCell(c, q); std::cout << "OK\n"; }
            catch (const std::invalid_argument&) { std::cout << "ERROR\n"; }
        } else if (command == "copy") {
            int source; std::cin >> source; v = values[source];
        } else if (command == "equal") {
            int other; std::cin >> other;
            std::cout << (v == values[other]) << ' ' << (v != values[other]) << '\n';
        } else return 1;
        if (!std::cin) return 1;
    }
}

#include <cassert>
#include <cstddef>
#include <iostream>
#include <string>
#include <type_traits>
#include <utility>

/*
Сигнатуры основных методов

Перечислен публичный интерфейс классов, которые вы реализуете в своём файле.

BatteryCell:
    explicit BatteryCell(int capacity = 100, int charge = 0);
    int GetCapacity() const;
    int GetCharge() const;
    int GetFreeSpace() const;
    bool TryCharge(int amount);
    bool TryConsume(int amount);
    BatteryCell& operator+=(int amount);
    BatteryCell& operator-=(int amount);
    bool operator==(const BatteryCell& other) const;
    bool operator!=(const BatteryCell& other) const;

BatteryPack:
    explicit BatteryPack(int cell_capacity = 100, int charge = 0);
    int GetCapacity() const;
    int GetCharge() const;
    int GetFreeSpace() const;
    const BatteryCell& operator[](std::size_t index) const;
    bool TryCharge(int amount);
    bool TryConsume(int amount);
    BatteryPack& operator+=(int amount);
    BatteryPack& operator-=(int amount);
    BatteryPack operator+(int amount) const;
    BatteryPack operator-(int amount) const;
*/

// Здесь система проверки вставит ваш код. Функцию main писать не нужно.

int main() {
    {
        static_assert(std::is_default_constructible_v<BatteryPack>);
        static_assert(std::is_constructible_v<BatteryPack, int, int>);
        static_assert(std::is_copy_constructible_v<BatteryPack>);
        static_assert(std::is_copy_assignable_v<BatteryPack>);
        static_assert(!std::is_convertible_v<int, BatteryPack>);
        static_assert(std::is_same_v<decltype(&BatteryPack::GetCapacity), int (BatteryPack::*)() const>);
        static_assert(std::is_same_v<decltype(&BatteryPack::GetCharge), int (BatteryPack::*)() const>);
        static_assert(std::is_same_v<decltype(&BatteryPack::GetFreeSpace), int (BatteryPack::*)() const>);
        static_assert(std::is_same_v<decltype(&BatteryPack::TryCharge), bool (BatteryPack::*)(int)>);
        static_assert(std::is_same_v<decltype(&BatteryPack::TryConsume), bool (BatteryPack::*)(int)>);
        static_assert(std::is_same_v<decltype(&BatteryPack::operator-=), BatteryPack& (BatteryPack::*)(int)>);
        static_assert(std::is_same_v<decltype(&BatteryPack::operator-), BatteryPack (BatteryPack::*)(int) const>);

        static_assert(std::is_same_v<decltype(std::declval<BatteryPack&>()[0]), const BatteryCell&>);
        static_assert(std::is_same_v<decltype(std::declval<const BatteryPack&>()[0]), const BatteryCell&>);
        static_assert(std::is_same_v<decltype(std::declval<BatteryPack&>() += 1), BatteryPack&>);
        static_assert(std::is_same_v<decltype(std::declval<const BatteryPack&>() + 1), BatteryPack>);
        const BatteryPack initial;
        assert(initial.GetCapacity() == 200 && initial.GetCharge() == 0);
        BatteryPack pack(10, 5);
        assert(pack.GetCharge() == 5 && pack[0].GetCharge() == 3 && pack[1].GetCharge() == 2);
        assert(!pack.TryConsume(6) && !pack.TryCharge(16) && pack.GetCharge() == 5);
        const BatteryPack copy = pack + 3;
        assert(copy.GetCharge() == 8 && pack.GetCharge() == 5);
        assert(&(pack += 15) == &pack && pack[0].GetCharge() == 10 && pack[1].GetCharge() == 10);
        assert(&(pack -= 19) == &pack && pack[0].GetCharge() == 1 && pack[1].GetCharge() == 0);
        for (int total = 0; total <= 20; ++total) {
            const BatteryPack value(10, total);
            assert(value[0].GetCharge() == (total + 1) / 2 && value[1].GetCharge() == total / 2);
            assert(value.GetFreeSpace() == 20 - total);
        }
    }
    int capacity, charge, commands;
    if (!(std::cin >> capacity >> charge >> commands)) return 1;
    BatteryPack values[3] = {BatteryPack(capacity, charge), BatteryPack(capacity, charge), BatteryPack(capacity, charge)};
    for (int t = 0; t < commands; ++t) {
        std::string command;
        int id;
        if (!(std::cin >> command >> id) || id < 0 || id >= 3) return 1;
        auto& v = values[id];
        if (command == "show") std::cout << v.GetCapacity() << ' ' << v.GetCharge() << ' ' << v.GetFreeSpace() << ' ' << v[0].GetCharge() << ' ' << v[1].GetCharge() << '\n';
        else if (command == "charge" || command == "consume") {
            int amount; std::cin >> amount;
            std::cout << (command == "charge" ? v.TryCharge(amount) : v.TryConsume(amount)) << '\n';
        } else if (command == "add" || command == "sub") {
            int amount; std::cin >> amount;
            const bool allowed = amount >= 0 && amount <=
                (command == "add" ? v.GetFreeSpace() : v.GetCharge());
            if (command == "add") v += amount;
            else v -= amount;
            if (allowed) std::cout << "OK\n";
        } else if (command == "make") {
            int c, q; std::cin >> c >> q;
            if (c <= 0 || c > 1000000 || q < 0 || q > 2 * c) std::cout << "ERROR\n";
            else { v = BatteryPack(c, q); std::cout << "OK\n"; }
        } else if (command == "copy") {
            int source; std::cin >> source; v = values[source];
        } else if (command == "plus" || command == "minus") {
            int source, amount; std::cin >> source >> amount;
            const bool allowed = amount >= 0 && amount <=
                (command == "plus" ? values[source].GetFreeSpace() : values[source].GetCharge());
            const BatteryPack result = command == "plus" ? values[source] + amount : values[source] - amount;
            if (allowed) { v = result; std::cout << "OK\n"; }
        } else if (command == "cell") {
            std::size_t index; std::cin >> index;
            if (index >= 2) std::cout << "ERROR\n";
            else { const auto& item = v[index]; std::cout << item.GetCapacity() << ' ' << item.GetCharge() << '\n'; }
        } else return 1;
        if (!std::cin) return 1;
    }
}

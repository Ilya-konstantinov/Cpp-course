#include <cassert>
#include <cstddef>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <string>

#include <type_traits>
#include <utility>

// Здесь система проверки вставит ваш код. Функцию main писать не нужно.

int main() {
    static_assert(std::is_default_constructible_v<MyVector>);
    static_assert(std::is_constructible_v<MyVector, std::size_t, std::int32_t>);
    static_assert(std::is_copy_constructible_v<MyVector>);
    static_assert(std::is_copy_assignable_v<MyVector>);
    static_assert(!std::is_convertible_v<std::size_t, MyVector>);
    static_assert(std::is_same_v<decltype(&MyVector::Size), std::size_t (MyVector::*)() const>);
    static_assert(std::is_same_v<decltype(&MyVector::Capacity), std::size_t (MyVector::*)() const>);
    static_assert(std::is_same_v<decltype(&MyVector::Sum), long long (MyVector::*)() const>);
    static_assert(std::is_same_v<decltype(&MyVector::Mean), long double (MyVector::*)() const>);
    static_assert(std::is_same_v<decltype(&MyVector::Min), std::int32_t (MyVector::*)() const>);
    static_assert(std::is_same_v<decltype(&MyVector::Max), std::int32_t (MyVector::*)() const>);
    static_assert(std::is_same_v<decltype(&MyVector::PushBack), void (MyVector::*)(std::int32_t)>);
    static_assert(std::is_same_v<decltype(&MyVector::PopBack), std::int32_t (MyVector::*)()>);
    static_assert(std::is_same_v<decltype(&MyVector::Resize), void (MyVector::*)(std::size_t)>);
    static_assert(std::is_same_v<decltype(&MyVector::Reserve), void (MyVector::*)(std::size_t)>);
    static_assert(std::is_same_v<decltype(&MyVector::Read), void (MyVector::*)(std::size_t)>);
    static_assert(std::is_same_v<decltype(&MyVector::Insert), void (MyVector::*)(std::int32_t, std::size_t)>);
    static_assert(std::is_same_v<decltype(&MyVector::Erase), void (MyVector::*)(std::size_t)>);

    {
        MyVector empty;
        assert(empty.Size() == 0 && empty.Capacity() == 0 && empty.Sum() == 0);
        MyVector zeroes(3);
        assert(zeroes.Size() == 3 && zeroes.Sum() == 0);
        MyVector original(3, 7);
        MyVector copy(original);
        copy.Resize(1);
        copy.PushBack(9);
        assert(original.Size() == 3 && original.Sum() == 21);
        assert(copy.Size() == 2 && copy.Sum() == 16);
        copy = original;
        copy = copy;
        assert(copy.Size() == 3 && copy.Sum() == 21);
        copy.Resize(1);
        copy.PushBack(5);
        assert(original.Sum() == 21 && copy.Sum() == 12);
        const auto capacity = original.Capacity();
        original.Reserve(capacity + 10);
        assert(original.Capacity() >= capacity + 10 && original.Sum() == 21);
        original.Resize(5);
        assert(original.Size() == 5 && original.Sum() == 21);
        original.Clear();
        assert(original.Size() == 0 && original.Capacity() == 0 && original.Sum() == 0);
    }
    std::size_t size, commands;
    if (!(std::cin >> size >> commands)) return 1;
    MyVector values;
    values.Read(size);
    while (commands--) {
        char command;
        std::cin >> command;
        if (command == '+') {
            std::size_t index;
            std::int32_t value;
            std::cin >> index >> value;
            if (index == 0) values.PushBack(value);
            else values.Insert(value, index - 1);
        } else if (command == '-') {
            std::size_t index;
            std::cin >> index;
            if (index == 0) values.PopBack();
            else values.Erase(index - 1);
        } else if (command == '?') {
            std::cin >> command;
            if (command == '+') std::cout << values.Sum();
            else if (command == '/') std::cout << std::fixed << std::setprecision(4) << values.Mean();
            else if (command == '<') std::cout << values.Min();
            else if (command == '>') std::cout << values.Max();
            else return 1;
            std::cout << '\n';
        } else return 1;
        if (!std::cin) return 1;
    }
}

#include <cassert>
#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <map>
#include <string>
#include <type_traits>
#include <utility>

namespace HolderTest {
inline std::map<void*, bool> arrays;
inline std::size_t allocated = 0;
inline std::size_t freed = 0;
inline std::size_t LiveCount() { return allocated - freed; }
}
// The supplied harness checks ownership even without sanitizers.
void* operator new[](std::size_t bytes) {
    void* pointer = std::malloc(bytes == 0 ? 1 : bytes);
    if (pointer == nullptr) std::abort();
    HolderTest::arrays[pointer] = true;
    ++HolderTest::allocated;
    return pointer;
}
void operator delete[](void* pointer) noexcept {
    if (pointer == nullptr) return;
    auto it = HolderTest::arrays.find(pointer);
    if (it == HolderTest::arrays.end() || !it->second) std::abort();
    it->second = false;
    ++HolderTest::freed;
    std::free(pointer);
}
void operator delete[](void* pointer, std::size_t) noexcept { ::operator delete[](pointer); }

namespace HolderTest {
inline int* MakeArray(std::size_t size) { return new int[size](); }
// Volatile indirection keeps allocation-elision from bypassing the ownership audit.
inline int* (*volatile allocation_function)(std::size_t) = MakeArray;
inline int* NewArray(std::size_t size) { return allocation_function(size); }
}
/*
Сигнатуры основных методов

Перечислен публичный интерфейс классов, которые вы реализуете в своём файле.

ArrayHolder:
    explicit ArrayHolder(int* data = nullptr);
    ~ArrayHolder();
    ArrayHolder(const ArrayHolder&) = delete;
    ArrayHolder& operator=(const ArrayHolder&) = delete;
    int* Get() const;
    int& operator[](std::size_t index) const;
    explicit operator bool() const;
    int* Release();
    void Reset(int* data = nullptr);
    void Swap(ArrayHolder& other);
*/

// Здесь система проверки вставит ваш код. Функцию main писать не нужно.

int main() {
    {
        static_assert(std::is_same_v<decltype(&ArrayHolder::Get), int* (ArrayHolder::*)() const>);
        static_assert(std::is_same_v<decltype(&ArrayHolder::Release), int* (ArrayHolder::*)()>);
        static_assert(std::is_same_v<decltype(&ArrayHolder::Reset), void (ArrayHolder::*)(int*)>);
        static_assert(std::is_same_v<decltype(&ArrayHolder::Swap), void (ArrayHolder::*)(ArrayHolder&)>);
        static_assert(std::is_constructible_v<ArrayHolder, int*>);
        static_assert(!std::is_convertible_v<int*, ArrayHolder>);
        static_assert(sizeof(ArrayHolder) == sizeof(int*));

        static_assert(!std::is_copy_constructible_v<ArrayHolder>);
        static_assert(!std::is_copy_assignable_v<ArrayHolder>);
        static_assert(!std::is_move_constructible_v<ArrayHolder>);
        static_assert(!std::is_move_assignable_v<ArrayHolder>);
        static_assert(!std::is_convertible_v<ArrayHolder, bool>);
        static_assert(std::is_same_v<decltype(std::declval<const ArrayHolder&>()[0]), int&>);
        const auto initial = HolderTest::LiveCount();
        {
            ArrayHolder empty;
            assert(!empty && empty.Get() == nullptr && empty.Release() == nullptr);
            empty.Reset(); empty.Swap(empty);
            int* pointer = HolderTest::NewArray(3);
            pointer[0] = 1; pointer[1] = 2; pointer[2] = 3;
            ArrayHolder value(pointer);
            assert(value.Get() == pointer && static_cast<bool>(value));
            const ArrayHolder& constant = value;
            constant[1] = 7;
            assert(pointer[1] == 7);
            value.Reset(value.Get());
            assert(value.Get() == pointer && value[0] == 1 && HolderTest::LiveCount() == initial + 1);
            const auto allocated = HolderTest::allocated;
            value.Swap(empty);
            assert(!value && empty.Get() == pointer && HolderTest::allocated == allocated);
            empty.Swap(empty);
            int* released = empty.Release();
            assert(released == pointer && !empty && HolderTest::LiveCount() == initial + 1);
            delete[] released;
            value.Reset(HolderTest::NewArray(2));
            value[0] = 4; value[1] = 5;
            value.Reset(HolderTest::NewArray(1));
            value[0] = 9;
            assert(HolderTest::LiveCount() == initial + 1 && value[0] == 9);
        }
        assert(HolderTest::LiveCount() == initial);
        { ArrayHolder local(HolderTest::NewArray(2)); }
        assert(HolderTest::LiveCount() == initial);
    }
    assert(HolderTest::LiveCount() == 0);
    int commands;
    if (!(std::cin >> commands)) return 1;
    {
        ArrayHolder values[8];
        std::size_t sizes[8] = {};
        for (int t = 0; t < commands; ++t) {
            std::string command;
            std::cin >> command;
            if (command == "live") { std::cout << HolderTest::LiveCount() << '\n'; continue; }
            if (command == "scope") {
                int size; std::cin >> size;
                const auto before = HolderTest::LiveCount();
                { ArrayHolder local(HolderTest::NewArray(size)); }
                assert(HolderTest::LiveCount() == before);
                continue;
            }
            int id;
            if (!(std::cin >> id) || id < 0 || id >= 8) return 1;
            auto& value = values[id];
            if (command == "new") {
                std::size_t size; std::cin >> size;
                int* pointer = HolderTest::NewArray(size);
                for (std::size_t i = 0; i < size; ++i) std::cin >> pointer[i];
                value.Reset(pointer);
                assert(value.Get() == pointer);
                sizes[id] = size;
            } else if (command == "set" || command == "writeconst") {
                std::size_t index; int item; std::cin >> index >> item;
                const ArrayHolder& constant = value;
                if (command == "set") value[index] = item;
                else constant[index] = item;
            } else if (command == "get") {
                std::size_t index; std::cin >> index;
                const ArrayHolder& constant = value;
                std::cout << constant[index] << '\n';
            } else if (command == "show") {
                std::cout << static_cast<bool>(value) << ' ' << sizes[id];
                for (std::size_t i = 0; i < sizes[id]; ++i) std::cout << ' ' << value[i];
                std::cout << '\n';
            } else if (command == "release") {
                int* pointer = value.Release();
                assert(value.Get() == nullptr && !value);
                long long sum = 0;
                for (std::size_t i = 0; i < sizes[id]; ++i) sum += pointer[i];
                delete[] pointer;
                sizes[id] = 0;
                std::cout << sum << '\n';
            } else if (command == "clear") { value.Reset(); sizes[id] = 0; }
            else if (command == "self") value.Reset(value.Get());
            else if (command == "swap") {
                int other; std::cin >> other;
                const auto allocations = HolderTest::allocated;
                int* left = value.Get(); int* right = values[other].Get();
                value.Swap(values[other]);
                assert(value.Get() == right && values[other].Get() == left);
                assert(HolderTest::allocated == allocations);
                std::swap(sizes[id], sizes[other]);
            } else return 1;
            if (!std::cin) return 1;
        }
    }
    std::cout << "END " << HolderTest::LiveCount() << '\n';
}

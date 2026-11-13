#include <cassert>
#include <cstddef>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <type_traits>
#include <utility>

/*
Сигнатуры основных методов

Fraction:
    Fraction(std::int64_t numerator, std::int64_t denominator);
    std::int64_t GetNumerator() const;
    std::int64_t GetDenominator() const;

Fraction operator+(const Fraction& left, const Fraction& right);
Fraction operator-(const Fraction& left, const Fraction& right);

Бинарные арифметические операторы и сравнения можно реализовать свободными функциями
или методами класса, сохраняя показанные в main выражения и типы результатов.
*/

// Здесь система проверки вставит ваш код. Функцию main писать не нужно.

int main() {
    {
        static_assert(std::is_constructible_v<Fraction, std::int64_t, std::int64_t>);
        static_assert(std::is_copy_constructible_v<Fraction>);
        static_assert(std::is_copy_assignable_v<Fraction>);
        static_assert(std::is_same_v<decltype(&Fraction::GetNumerator), std::int64_t (Fraction::*)() const>);
        static_assert(std::is_same_v<decltype(&Fraction::GetDenominator), std::int64_t (Fraction::*)() const>);
        static_assert(std::is_same_v<decltype(std::declval<const Fraction&>() + std::declval<const Fraction&>()), Fraction>);
        static_assert(std::is_same_v<decltype(std::declval<const Fraction&>() - std::declval<const Fraction&>()), Fraction>);
    
        const Fraction half(-2, -4);
        assert(half.GetNumerator() == 1 && half.GetDenominator() == 2);
        const Fraction zero(0, -7);
        assert(zero.GetNumerator() == 0 && zero.GetDenominator() == 1);
        const Fraction third(1, 3);
        const Fraction sum = half + third;
        const Fraction difference = half - third;
        assert(sum.GetNumerator() == 5 && sum.GetDenominator() == 6);
        assert(difference.GetNumerator() == 1 && difference.GetDenominator() == 6);
        assert(half.GetNumerator() == 1 && half.GetDenominator() == 2);
        assert(third.GetNumerator() == 1 && third.GetDenominator() == 3);
    }
    std::int64_t numerator, denominator;
    char separator;
    if (!(std::cin >> numerator >> separator >> denominator) || separator != '/') return 1;
    Fraction value(numerator, denominator);
    char operation;
    while (std::cin >> operation) {
        if (!(std::cin >> numerator >> separator >> denominator) || separator != '/') return 1;
        const Fraction other(numerator, denominator);
        if (operation == '+') value = value + other;
        else if (operation == '-') value = value - other;
        else return 1;
    }
    std::cout << value.GetNumerator() << '/' << value.GetDenominator() << '\n';
}

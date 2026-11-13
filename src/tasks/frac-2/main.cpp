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
    Fraction();
    Fraction(std::int64_t numerator);
    Fraction& operator+=(const Fraction& other);
    Fraction& operator-=(const Fraction& other);
    Fraction& operator*=(const Fraction& other);
    Fraction& operator/=(const Fraction& other);
    Fraction operator+() const;
    Fraction operator-() const;
    explicit operator double() const;

Fraction operator+(const Fraction& left, const Fraction& right);
Fraction operator-(const Fraction& left, const Fraction& right);
Fraction operator*(const Fraction& left, const Fraction& right);
Fraction operator/(const Fraction& left, const Fraction& right);
bool operator==(const Fraction& left, const Fraction& right);
bool operator!=(const Fraction& left, const Fraction& right);
bool operator<(const Fraction& left, const Fraction& right);
bool operator>(const Fraction& left, const Fraction& right);
bool operator<=(const Fraction& left, const Fraction& right);
bool operator>=(const Fraction& left, const Fraction& right);
std::istream& operator>>(std::istream& in, Fraction& value);
std::ostream& operator<<(std::ostream& out, const Fraction& value);

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
        static_assert(std::is_default_constructible_v<Fraction>);
        static_assert(std::is_constructible_v<Fraction, std::int64_t>);
        static_assert(std::is_convertible_v<std::int64_t, Fraction>);
        static_assert(std::is_same_v<decltype(std::declval<const Fraction&>() * std::declval<const Fraction&>()), Fraction>);
        static_assert(std::is_same_v<decltype(std::declval<const Fraction&>() / std::declval<const Fraction&>()), Fraction>);
        static_assert(std::is_same_v<decltype(std::declval<Fraction&>() += std::declval<const Fraction&>()), Fraction&>);
        static_assert(std::is_same_v<decltype(std::declval<Fraction&>() -= std::declval<const Fraction&>()), Fraction&>);
        static_assert(std::is_same_v<decltype(std::declval<Fraction&>() *= std::declval<const Fraction&>()), Fraction&>);
        static_assert(std::is_same_v<decltype(std::declval<Fraction&>() /= std::declval<const Fraction&>()), Fraction&>);
        static_assert(std::is_same_v<decltype(std::declval<const Fraction&>() == std::declval<const Fraction&>()), bool>);
        static_assert(std::is_same_v<decltype(std::declval<const Fraction&>() != std::declval<const Fraction&>()), bool>);
        static_assert(std::is_same_v<decltype(std::declval<const Fraction&>() < std::declval<const Fraction&>()), bool>);
        static_assert(std::is_same_v<decltype(std::declval<const Fraction&>() > std::declval<const Fraction&>()), bool>);
        static_assert(std::is_same_v<decltype(std::declval<const Fraction&>() <= std::declval<const Fraction&>()), bool>);
        static_assert(std::is_same_v<decltype(std::declval<const Fraction&>() >= std::declval<const Fraction&>()), bool>);
        static_assert(std::is_same_v<decltype(+std::declval<const Fraction&>()), Fraction>);
        static_assert(std::is_same_v<decltype(-std::declval<const Fraction&>()), Fraction>);
        static_assert(std::is_same_v<decltype(static_cast<double>(std::declval<const Fraction&>())), double>);
        static_assert(std::is_same_v<decltype(std::declval<std::istream&>() >> std::declval<Fraction&>()), std::istream&>);
        static_assert(std::is_same_v<decltype(std::declval<std::ostream&>() << std::declval<const Fraction&>()), std::ostream&>);
    
        assert(Fraction() == Fraction(0, 1));
        assert(Fraction(-2, -4) == Fraction(1, 2));
        assert(Fraction(2, -4) == Fraction(-1, 2));
        assert(Fraction(0, -7).GetDenominator() == 1);
        assert(Fraction(-1, 2) + Fraction(1, 3) == Fraction(-1, 6));
        assert(Fraction(1, 2) - Fraction(1, 3) == Fraction(1, 6));
        assert(Fraction(-1, 2) < Fraction(0));
        assert(Fraction(-3, -4) > Fraction(1, 2));
        assert(Fraction(2147483646, 2147483647) + Fraction(1, 2147483647) == Fraction(1));
        const auto minimum = std::numeric_limits<std::int64_t>::min();
        const auto maximum = std::numeric_limits<std::int64_t>::max();
        assert(Fraction(minimum, minimum) == Fraction(1));
        assert(Fraction(2, minimum) == Fraction(-1, 4611686018427387904LL));
        assert(Fraction(maximum, 2) + Fraction(maximum, 2) == Fraction(maximum));
        assert(Fraction(maximum) * Fraction(1, maximum) == Fraction(1));
        assert(Fraction(minimum) < Fraction(maximum));
        assert(1 + Fraction(1, 2) == Fraction(3, 2));
        assert(1 - Fraction(1, 2) == Fraction(1, 2));
        assert(2 * Fraction(1, 2) == Fraction(1));
        assert(1 / Fraction(1, 2) == Fraction(2));
        Fraction value(1, 2);
        assert(&(value += Fraction(1, 2)) == &value);
        value -= Fraction(1, 2);
        value *= Fraction(3);
        value /= Fraction(-2);
        assert(value == Fraction(-3, 4));
        assert(+value == value && -value == Fraction(3, 4));
        assert(value <= Fraction(-3, 4) && value >= Fraction(-3, 4));
        assert(value != Fraction(0) && !(value > Fraction(0)));
        assert(static_cast<double>(value) == -0.75);
        std::stringstream stream("2/-4 3");
        stream >> value;
        assert(value == Fraction(-1, 2));
        stream >> value;
        assert(value == Fraction(3));
        std::istringstream integer_at_eof("2");
        integer_at_eof >> value;
        assert(value == Fraction(2));
        std::ostringstream output;
        output << Fraction(2) << ' ' << Fraction(2, -4);
        assert(output.str() == "2 -1/2");
    }
    Fraction value;
    if (!(std::cin >> value)) return 1;
    std::string command;
    while (std::cin >> command) {
        if (command == "exit" || command == "quit") break;
        if (command == "show" || command == "print") {
            std::cout << value << '\n';
            continue;
        }
        if (command == "double") {
            std::cout << std::fixed << std::setprecision(6) << static_cast<double>(value) << '\n';
            continue;
        }
        Fraction other;
        if (!(std::cin >> other)) return 1;
        if (command == "+") value = value + other;
        else if (command == "-") value = value - other;
        else if (command == "*") value = value * other;
        else if (command == "/") value = value / other;
        else if (command == "+=") value += other;
        else if (command == "-=") value -= other;
        else if (command == "*=") value *= other;
        else if (command == "/=") value /= other;
        else if (command == "=") value = other;
        else return 1;
    }
    std::cout << "[END]\n" << value << '\n' << (value < Fraction(1)) << '\n';
}

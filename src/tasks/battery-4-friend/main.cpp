#include <cassert>
#include <cstddef>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <type_traits>
#include <utility>

// Здесь система проверки вставит ваш код. Функцию main писать не нужно.

void ShowRobot(const Robot& robot) {
    std::cout << robot.GetPosition() << ' ' << robot.GetBattery().GetCharge() << ' '
              << robot.GetMotor().IsRunning() << ' ' << static_cast<bool>(robot) << ' '
              << robot.GetBattery()[0].GetCharge() << ' ' << robot.GetBattery()[1].GetCharge()
              << ' ' << robot.GetMotor().GetCost() << '\n';
}
int main() {
    {
        static_assert(std::is_same_v<decltype(std::declval<std::ostream&>() << std::declval<const BatteryCell&>()), std::ostream&>);
        static_assert(std::is_same_v<decltype(std::declval<std::istream&>() >> std::declval<BatteryCell&>()), std::istream&>);
        static_assert(std::is_same_v<decltype(std::declval<std::ostream&>() << std::declval<const BatteryPack&>()), std::ostream&>);
        static_assert(std::is_same_v<decltype(std::declval<std::istream&>() >> std::declval<BatteryPack&>()), std::istream&>);
        static_assert(std::is_same_v<decltype(std::declval<std::ostream&>() << std::declval<const Motor&>()), std::ostream&>);
        static_assert(std::is_same_v<decltype(std::declval<std::istream&>() >> std::declval<Motor&>()), std::istream&>);
        static_assert(std::is_same_v<decltype(std::declval<std::ostream&>() << std::declval<const Robot&>()), std::ostream&>);
        static_assert(std::is_same_v<decltype(std::declval<std::istream&>() >> std::declval<Robot&>()), std::istream&>);

        BatteryCell cell(10, 5);
        for (const std::string text : {"0 0", "10 11", "10 -1", "10", "x 1"}) {
            std::istringstream in(text); in >> cell;
            assert(in.fail() && cell.GetCapacity() == 10 && cell.GetCharge() == 5);
        }
        BatteryPack pack(10, 5);
        for (const std::string text : {"10 1 2", "10 5 1", "10 11 10", "10 -1 -1", "10 3"}) {
            std::istringstream in(text); in >> pack;
            assert(in.fail() && pack.GetCharge() == 5);
        }
        Motor motor(3);
        for (const std::string text : {"0 0", "3 2", "3"}) {
            std::istringstream in(text); in >> motor;
            assert(in.fail() && motor.GetCost() == 3 && !motor.IsRunning());
        }
        Robot robot(BatteryPack(10, 7), 3);
        robot.TryStart(); robot.TryMove(1);
        std::ostringstream saved; saved << static_cast<const Robot&>(robot);
        assert(saved.str() == "10 2 2 3 1 1");
        for (const std::string text : {"10 2 2 5 1 9", "10 2 2 3 1 -1", "10 2 2 3 1", "10 2 2 x 1 8"}) {
            std::istringstream in(text); in >> robot;
            std::ostringstream after; after << robot;
            assert(in.fail() && after.str() == saved.str());
        }
        std::istringstream in("8 4 3 2 1 17");
        in >> robot;
        assert(!in.fail() && robot.GetPosition() == 17 && robot.GetBattery().GetCharge() == 7);
        std::ostringstream output; output << robot;
        assert(output.str() == "8 4 3 2 1 17");
    }
    int capacity, charge, cost, commands;
    if (!(std::cin >> capacity >> charge >> cost >> commands)) return 1;
    const Robot initial(BatteryPack(capacity, charge), cost);
    Robot values[2] = {Robot(initial), Robot(initial)};
    for (int t = 0; t < commands; ++t) {
        std::string command;
        int id;
        if (!(std::cin >> command >> id) || id < 0 || id >= 2) return 1;
        auto& value = values[id];
        if (command == "show") ShowRobot(value);
        else if (command == "start") std::cout << value.TryStart() << '\n';
        else if (command == "stop") value.Stop();
        else if (command == "move" || command == "charge") {
            int amount; std::cin >> amount;
            std::cout << (command == "move" ? value.TryMove(amount) : value.TryCharge(amount)) << '\n';
        } else if (command == "copy") {
            int source; std::cin >> source; value = values[source];
        } else if (command == "pre") {
            const bool allowed = static_cast<bool>(value);
            Robot& result = ++value;
            assert(&result == &value);
            if (allowed) ShowRobot(result);
        } else if (command == "post") {
            const bool allowed = static_cast<bool>(value);
            const Robot previous = value++;
            if (allowed) ShowRobot(previous);
        } else if (command == "dump") std::cout << value << '\n';
        else if (command == "load") {
            std::string payload; std::getline(std::cin, payload);
            std::istringstream input(payload);
            input >> value;
            std::cout << !input.fail() << '\n';
        } else if (command == "roundtrip") {
            std::stringstream stream;
            stream << static_cast<const Robot&>(value);
            Robot restored;
            stream >> restored;
            std::cout << !stream.fail() << ' ' << restored << '\n';
        } else return 1;
        if (!std::cin) return 1;
    }
}

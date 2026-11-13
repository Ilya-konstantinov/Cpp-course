#include <cassert>
#include <cstddef>
#include <iostream>
#include <limits>
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
        static_assert(std::is_default_constructible_v<Robot>);
        static_assert(std::is_constructible_v<Robot, const BatteryPack&, int>);
        static_assert(std::is_copy_constructible_v<Robot>);
        static_assert(std::is_copy_assignable_v<Robot>);
        static_assert(!std::is_convertible_v<BatteryPack, Robot>);
        static_assert(std::is_same_v<decltype(&Robot::GetPosition), long long (Robot::*)() const>);
        static_assert(std::is_same_v<decltype(&Robot::GetBattery), const BatteryPack& (Robot::*)() const>);
        static_assert(std::is_same_v<decltype(&Robot::GetMotor), const Motor& (Robot::*)() const>);
        static_assert(std::is_same_v<decltype(&Robot::TryStart), bool (Robot::*)()>);
        static_assert(std::is_same_v<decltype(&Robot::Stop), void (Robot::*)()>);
        static_assert(std::is_same_v<decltype(&Robot::TryCharge), bool (Robot::*)(int)>);
        static_assert(std::is_same_v<decltype(&Robot::TryMove), bool (Robot::*)(int)>);

        static_assert(std::is_same_v<decltype(std::declval<Robot&>().GetBattery()), const BatteryPack&>);
        static_assert(std::is_same_v<decltype(std::declval<Robot&>().GetMotor()), const Motor&>);
        static_assert(std::is_same_v<decltype(++std::declval<Robot&>()), Robot&>);
        static_assert(std::is_same_v<decltype(std::declval<Robot&>()++), Robot>);
        static_assert(!std::is_convertible_v<Robot, bool>);
        const Robot initial;
        assert(initial.GetPosition() == 0 && initial.GetBattery().GetCapacity() == 200 && !initial);
        assert(initial.GetMotor().GetCost() == 1 && !initial.GetMotor().IsRunning());
        Robot robot(BatteryPack(10, 7), 3);
        assert(!robot && !robot.TryMove(1));
        assert(robot.TryStart() && static_cast<bool>(robot));
        assert(!robot.TryMove(3) && robot.GetPosition() == 0 && robot.GetBattery().GetCharge() == 7);
        const Robot old = robot++;
        assert(old.GetPosition() == 0 && old.GetBattery().GetCharge() == 7 && old.GetMotor().IsRunning());
        assert(robot.GetPosition() == 1 && robot.GetBattery().GetCharge() == 4 && static_cast<bool>(robot));
        assert(&(++robot) == &robot);
        assert(robot.GetPosition() == 2 && robot.GetBattery().GetCharge() == 1 && !robot.GetMotor().IsRunning());
        assert(robot.TryCharge(5) && !robot.GetMotor().IsRunning());
        assert(robot.TryStart());
        assert(!robot.TryMove(0) && !robot.TryMove(-1));
        Robot copy(robot);
        copy.TryMove(1);
        assert(robot.GetPosition() == 2 && robot.GetBattery().GetCharge() == 6);
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
        } else return 1;
        if (!std::cin) return 1;
    }
}

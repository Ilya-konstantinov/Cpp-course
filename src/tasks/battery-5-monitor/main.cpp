#include <algorithm>
#include <cassert>
#include <cstddef>
#include <iostream>
#include <limits>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

struct EnergyEvent { bool charged; int amount; };
struct Statistics {
    std::size_t operations = 0;
    long long charged = 0;
    long long spent = 0;
    int median_twice = 0;
};
inline std::size_t statistics_calculations = 0;
// Provided implementation. Do not reimplement it in the submitted class.
inline Statistics ComputeStatistics(const std::vector<EnergyEvent>& history) {
    ++statistics_calculations;
    Statistics result;
    result.operations = history.size();
    std::vector<int> spending;
    for (const EnergyEvent& event : history) {
        if (event.charged) result.charged += event.amount;
        else { result.spent += event.amount; spending.push_back(event.amount); }
    }
    if (!spending.empty()) {
        std::sort(spending.begin(), spending.end());
        const auto middle = spending.size() / 2;
        result.median_twice = spending.size() % 2 ? 2 * spending[middle]
            : spending[middle - 1] + spending[middle];
    }
    return result;
}
// Здесь система проверки вставит ваш код. Функцию main писать не нужно.

void ShowRobot(const Robot& robot) {
    std::cout << robot.GetPosition() << ' ' << robot.GetBattery().GetCharge() << ' '
              << robot.GetMotor().IsRunning() << ' ' << static_cast<bool>(robot) << ' '
              << robot.GetBattery()[0].GetCharge() << ' ' << robot.GetBattery()[1].GetCharge()
              << ' ' << robot.GetMotor().GetCost() << '\n';
}
int main() {
    {
        static_assert(std::is_default_constructible_v<RobotMonitor>);
        static_assert(std::is_constructible_v<RobotMonitor, const Robot&>);
        static_assert(std::is_copy_constructible_v<RobotMonitor>);
        static_assert(std::is_copy_assignable_v<RobotMonitor>);
        static_assert(!std::is_convertible_v<Robot, RobotMonitor>);
        static_assert(std::is_same_v<decltype(&RobotMonitor::GetOperationCount), std::size_t (RobotMonitor::*)() const>);
        static_assert(std::is_same_v<decltype(&RobotMonitor::GetRobot), const Robot& (RobotMonitor::*)() const>);
        static_assert(std::is_same_v<decltype(&RobotMonitor::GetStatistics), Statistics (RobotMonitor::*)() const>);
        static_assert(std::is_same_v<decltype(&RobotMonitor::TryStart), bool (RobotMonitor::*)()>);
        static_assert(std::is_same_v<decltype(&RobotMonitor::Stop), void (RobotMonitor::*)()>);
        static_assert(std::is_same_v<decltype(&RobotMonitor::TryCharge), bool (RobotMonitor::*)(int)>);
        static_assert(std::is_same_v<decltype(&RobotMonitor::TryMove), bool (RobotMonitor::*)(int)>);

        static_assert(std::is_same_v<decltype(std::declval<RobotMonitor&>().GetRobot()), const Robot&>);
        static_assert(std::is_same_v<decltype(std::declval<const RobotMonitor&>().GetStatistics()), Statistics>);
        const RobotMonitor initial;
        assert(initial.GetOperationCount() == 0 && initial.GetRobot().GetBattery().GetCharge() == 0);
        statistics_calculations = 0;
        RobotMonitor monitor(Robot(BatteryPack(10, 7), 3));
        const RobotMonitor& constant = monitor;
        auto statistics = constant.GetStatistics();
        assert(statistics.operations == 0 && statistics.median_twice == 0 && statistics_calculations == 1);
        constant.GetStatistics();
        assert(statistics_calculations == 1);
        assert(!monitor.TryMove(1) && !monitor.TryCharge(14));
        assert(monitor.TryCharge(0));
        constant.GetStatistics();
        assert(statistics_calculations == 1 && monitor.GetOperationCount() == 0);
        assert(monitor.TryStart());
        constant.GetStatistics();
        assert(statistics_calculations == 1);
        assert(monitor.TryMove(1));
        statistics = constant.GetStatistics();
        assert(statistics.operations == 1 && statistics.spent == 3 && statistics.median_twice == 6);
        assert(statistics_calculations == 2);
        monitor.Stop(); constant.GetStatistics();
        assert(statistics_calculations == 2);
        assert(monitor.TryCharge(2));
        statistics = constant.GetStatistics();
        assert(statistics.operations == 2 && statistics.charged == 2 && statistics.spent == 3);
        assert(statistics_calculations == 3 && constant.GetRobot().GetPosition() == 1);
        RobotMonitor copy(monitor);
        assert(copy.TryStart() && copy.TryMove(2));
        assert(monitor.GetRobot().GetPosition() == 1 && monitor.GetOperationCount() == 2);
        statistics = copy.GetStatistics();
        assert(statistics.spent == 9 && statistics.median_twice == 9);
    }
    statistics_calculations = 0;
    int capacity, charge, cost, commands;
    if (!(std::cin >> capacity >> charge >> cost >> commands)) return 1;
    const Robot initial(BatteryPack(capacity, charge), cost);
    RobotMonitor values[2] = {RobotMonitor(initial), RobotMonitor(initial)};
    for (int t = 0; t < commands; ++t) {
        std::string command;
        int id;
        if (!(std::cin >> command >> id) || id < 0 || id >= 2) return 1;
        auto& value = values[id];
        if (command == "show") ShowRobot(value.GetRobot());
        else if (command == "start") std::cout << value.TryStart() << '\n';
        else if (command == "stop") value.Stop();
        else if (command == "move" || command == "charge") {
            int amount; std::cin >> amount;
            std::cout << (command == "move" ? value.TryMove(amount) : value.TryCharge(amount)) << '\n';
        } else if (command == "copy") {
            int source; std::cin >> source; value = values[source];
        } else if (command == "stats") {
            const RobotMonitor& constant = value;
            const Statistics statistics = constant.GetStatistics();
            std::cout << statistics.operations << ' ' << statistics.charged << ' ' << statistics.spent
                      << ' ' << statistics.median_twice << ' ' << statistics_calculations << '\n';
        } else if (command == "count") { std::cout << value.GetOperationCount() << '\n';
        } else return 1;
        if (!std::cin) return 1;
    }
}

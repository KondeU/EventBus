// FIXME: Temporary test used!

#include <vector>
#include <memory>
#include <iostream>
#include "common/Action.hpp"

void func(int i)
{
    std::cout << "func " << i << std::endl;
}

void func1(std::string s)
{
    std::cout << "func1 " << s.c_str() << std::endl;
}

void func2(std::string& s)
{
    std::cout << "func2 " << s.c_str() << std::endl;
}

class Cls {
public:
    void funcs(std::string s)
    {
        std::cout << "funcs " << s.c_str() << std::endl;
    }
};

int main(int argc, char* argv[])
{
    Cls cls;

    std::string name1 = "name1";
    std::string name2 = "name2";
    std::string names = "names";

    int v = 12;

    std::vector<std::unique_ptr<TiBus::common::ActionExecutor>> actions;
    //actions.emplace_back(new TiBus::common::Action<int>{ func, 1 });
    //actions.emplace_back(new TiBus::common::Action<int>{ func, std::move(v) });
    actions.emplace_back(TiBus::common::MakeAction(func, 1));
    actions.emplace_back(TiBus::common::MakeAction(func, std::move(v)));
    actions.emplace_back(TiBus::common::MakeAction([&cls](std::string n) { cls.funcs(n); }, std::move(names)));
    actions.emplace_back(new TiBus::common::Action<std::string>{ func1, std::move(name1) });
    actions.emplace_back(new TiBus::common::Action<std::string&>{ func2, name2 });
    actions.emplace_back(new TiBus::common::Action<>{ []() {std::cout << "Nothing" << std::endl; } });

    for (auto& each : actions) {
        each->Execute();
    }

    return 0;
}


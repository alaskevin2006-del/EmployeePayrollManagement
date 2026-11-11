#pragma once
#include "Employee/Person.h"
#include <string>
#include <utility>
#include <vector>

enum class Page {
    MENU,
    INPUTINFO,
    ADDEMPLOYEE,
    DELETEEMPLOYEE,
    MONTHPAY,
    SHOWSALARY,
    EXIT
};

class UI {
public:
    void drawMenu() const;
    Page handleKeyboard() const;

    // 返回 false 表示取消批量录入
    bool drawInputInfo(
        std::vector<std::pair<std::string, int>>& data
    ) const;

    bool drawAddEmployee(
        std::string& name, int& level
    ) const;

    int drawDeleteEmployee() const;

    std::vector<std::pair<int, int>> drawMonthPay(
        const std::vector<Person*>& employees
    ) const;

    void drawShowSalary(
        const std::vector<Person*>& employees,
        long long total
    ) const;

    void showMessage(const std::string& message) const;
    void pause() const;
};
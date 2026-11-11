#include "UI.h"
#include <iostream>
#include <iomanip>
#include <limits>

using namespace std;

namespace {
    int readNumber(
        const string& prompt,
        int minValue,
        int maxValue
    ) {
        while (true) {
            cout << prompt;
            int n;

            if ((cin >> n) &&
                n >= minValue && n <= maxValue) {

                cin.ignore(
                    numeric_limits<streamsize>::max(), '\n'
                );
                return n;
            }

            cout << "输入无效，请重新输入。\n";
            cin.clear();
            cin.ignore(
                numeric_limits<streamsize>::max(), '\n'
            );
        }
    }

    string readName(const string& prompt) {
        cout << prompt;
        string name;
        cin >> name;
        cin.ignore(
            numeric_limits<streamsize>::max(), '\n'
        );
        return name;
    }
}

// 1. 主菜单
void UI::drawMenu() const {
    cout << "\n******** 公司职工薪酬管理系统 ********\n"
         << "1. 录入职工资料（覆盖原数据）\n"
         << "2. 增加职工\n"
         << "3. 删除职工\n"
         << "4. 月薪发放\n"
         << "5. 显示工资表\n"
         << "0. 退出系统\n"
         << "************************************\n";
}

// 读取用户选择
Page UI::handleKeyboard() const {
    int choice = readNumber("请选择：", 0, 5);

    switch (choice) {
        case 1: return Page::INPUTINFO;
        case 2: return Page::ADDEMPLOYEE;
        case 3: return Page::DELETEEMPLOYEE;
        case 4: return Page::MONTHPAY;
        case 5: return Page::SHOWSALARY;
        default: return Page::EXIT;
    }
}

// 2. 批量录入
bool UI::drawInputInfo(
    vector<pair<string, int>>& data
) const {
    string confirm = readName(
        "覆盖原有职工资料，确认继续？(Y/N)："
    );

    if (confirm != "Y" && confirm != "y") {
        return false;
    }

    const char* titles[] = {
        "经理", "技术员", "销售经理", "推销员"
    };
    const int levels[] = {4, 3, 2, 1};

    for (int i = 0; i < 4; ++i) {
        cout << "\n录入" << titles[i]
             << "姓名（输入 @ 结束本组）：\n";

        while (true) {
            string name = readName("姓名：");

            if (name == "@") break;

            data.push_back({name, levels[i]});
        }
    }

    return true;
}

// 3. 增加员工
bool UI::drawAddEmployee(
    string& name, int& level
) const {
    name = readName("输入职工姓名（@ 取消）：");

    if (name == "@") return false;

    level = readNumber(
        "输入级别（4经理 3技术员 2销售经理 1推销员）：",
        1, 4
    );

    return true;
}

// 4. 删除员工
int UI::drawDeleteEmployee() const {
    return readNumber(
        "输入要删除的职工编号：",
        2001, 2147483647
    );
}

// 5. 录入本月工作数据
vector<pair<int, int>> UI::drawMonthPay(
    const vector<Person*>& employees
) const {
    vector<pair<int, int>> data;

    cout << "\n录入本月工作数据：\n";

    for (Person* p : employees) {
        if (p->getLevel() == 3) {
            int hours = readNumber(
                "技术员 " + p->getName() +
                "（编号 " + to_string(p->getId()) +
                "）工作小时数：",
                0, 214748
            );

            data.push_back({p->getId(), hours});
        }
        else if (p->getLevel() == 1) {
            int sales = readNumber(
                "推销员 " + p->getName() +
                "（编号 " + to_string(p->getId()) +
                "）销售额（元）：",
                0, 21474836
            );

            data.push_back({p->getId(), sales});
        }
        else if (p->getLevel() == 2) {
            int sales = readNumber(
                "销售经理 " + p->getName() +
                "（编号 " + to_string(p->getId()) +
                "）部门销售总额（元）：",
                0, 21474836
            );

            data.push_back({p->getId(), sales});
        }
    }

    cout << "经理按固定工资；销售经理按全部推销员"
         << "销售总额计算提成。\n";

    return data;
}

// 6. 显示工资表
void UI::drawShowSalary(
    const vector<Person*>& employees,
    long long total
) const {
    cout << "\n职工薪酬表\n";

    cout << left
         << setw(12) << "工号"
         << setw(20) << "姓名"
         << setw(10) << "级别"
         << "月工资（元）\n";

    cout << fixed << setprecision(2);

    for (Person* p : employees) {
        cout << left
             << setw(12) << p->getId()
             << setw(20) << p->getName()
             << setw(10) << p->getLevel()
             << p->getMonthlySalary() << '\n';
    }

    cout << "合计：" << total / 100.0 << " 元\n";
}

void UI::showMessage(const string& message) const {
    cout << message << '\n';
}

void UI::pause() const {
    cout << "按回车键返回主菜单...";
    cin.get();
}
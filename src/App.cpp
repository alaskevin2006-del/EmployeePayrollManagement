#include "App.h"
#include <string>
#include <sstream>
#include <iomanip>
#include <utility>
#include <vector>

using namespace std;

void App::run() {
    const string filename = "employees.txt";

    // 启动时读取已有数据
    // 第一次启动没有文件是正常情况
    service.loadFromFile(filename);

    bool running = true;

    while (running) {
        ui.drawMenu();
        Page choice = ui.handleKeyboard();

        switch (choice) {

            // 1. 批量录入
            case Page::INPUTINFO: {
                vector<pair<string, int>> data;

                if (!ui.drawInputInfo(data)) {
                    ui.showMessage("已取消录入。");
                }
                else if (service.inputInfo(data)) {
                    ui.showMessage("录入成功。");

                    if (!service.saveToFile(filename)) {
                        ui.showMessage("警告：文件保存失败。");
                    }
                }
                else {
                    ui.showMessage("录入失败。");
                }

                break;
            }

            // 2. 增加员工
            case Page::ADDEMPLOYEE: {
                string name;
                int level = 0;

                if (ui.drawAddEmployee(name, level)) {
                    int id = service.addEmployee(name, level);

                    if (id > 0) {
                        ui.showMessage(
                            "添加成功，职工编号：" + to_string(id)
                        );

                        if (!service.saveToFile(filename)) {
                            ui.showMessage("警告：文件保存失败。");
                        }
                    }
                    else {
                        ui.showMessage("添加失败。");
                    }
                }

                break;
            }

            // 3. 删除员工
            case Page::DELETEEMPLOYEE: {
                int id = ui.drawDeleteEmployee();
                bool ok = service.deleteEmployee(id);

                ui.showMessage(
                    ok ? "删除成功。" : "未找到该职工。"
                );

                if (ok && !service.saveToFile(filename)) {
                    ui.showMessage("警告：文件保存失败。");
                }

                break;
            }

            // 4. 月薪发放
            case Page::MONTHPAY: {
                auto data = ui.drawMonthPay(
                    service.showSalary()
                );

                long long total = service.monthPay(data);

                if (total < 0) {
                    ui.showMessage(
                        "工资计算失败：数据不合法。"
                    );
                }
                else {
                    ostringstream out;
                    out << fixed << setprecision(2)
                        << total / 100.0;

                    ui.showMessage(
                        "工资总额：" + out.str() + " 元"
                    );

                    if (!service.saveToFile(filename)) {
                        ui.showMessage("警告：文件保存失败。");
                    }
                }

                break;
            }

            // 5. 显示工资表
            case Page::SHOWSALARY: {
                long long total = service.monthPay();

                ui.drawShowSalary(
                    service.showSalary(),
                    total
                );

                break;
            }

            // 0. 退出系统
            case Page::EXIT:
                running = false;
                break;

            default:
                break;
        }

        // 完成业务后，返回主菜单
        if (running) {
            ui.pause();
        }
    }
}
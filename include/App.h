#pragma once

#include <string>
#include <vector>

#include "Person.h"

// 公司人员与薪酬管理系统的控制层。
// 负责菜单交互、职工对象的增删、每月付薪、工资表输出，
// 并把职工数据同步到硬盘文件，业务计算仍由各 Person 子类的虚函数完成。
class App {
 public:
  App() = default;
  ~App();

  // 基类 Person 为多态类型，App 独占其对象，禁止拷贝。
  App(const App&) = delete;
  App& operator=(const App&) = delete;

  // 程序入口：载入硬盘数据后进入菜单循环，直到用户选择退出。
  void run();

 private:
  // 主菜单与通用交互
  void showMenu() const;
  int readChoice() const;
  void pause() const;

  // 菜单对应的五项业务功能
  void inputInfo();       // 1. 录入职工资料（覆盖原有数据）
  void addEmployee();     // 2. 增加职工
  void deleteEmployee();  // 3. 删除职工
  void monthPay();        // 4. 月薪发放
  void showSalary();      // 5. 显示工资表

  // 按级别批量录入姓名，输入 @ 表示结束。
  void inputNames(const std::string& title, const std::string& label,
                  int level);

  // 按级别创建职工对象，级别取值 1~4，非法级别返回 nullptr。
  Person* createEmployee(int level, const std::string& name) const;

  // 数据文件读写
  void loadFromFile();
  void saveToFile();
  void clearEmployees();

  std::vector<Person*> employees;
};

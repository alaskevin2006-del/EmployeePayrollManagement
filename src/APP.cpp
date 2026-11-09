// windows.h 必须放在本文件最前面：Person.h 中的 using namespace std 会让
// Windows SDK 里的 byte 与 std::byte 冲突，先把系统头文件解析完即可避开。
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#endif

#include "App.h"

#include <cstddef>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>

#include "Manager.h"
#include "Saleman.h"
#include "Salemanager.h"
#include "Technicant.h"

namespace {

// 数据文件：纯文本，每行一名职工，格式为 级别 姓名 月度数据。
const char kDataFile[] = "salary.txt";

// 工资表与花名册的分隔线宽度。
const int kLineWidth = 42;

// 读取一个数字，输入非法时提示重输，避免 cin 进入错误状态后死循环。
double readNumber(const std::string& prompt) {
  double value = 0;
  while (true) {
    std::cout << prompt;
    if (std::cin >> value) {
      std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
      return value;
    }
    if (std::cin.eof()) {
      return 0;
    }
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cout << "输入有误，请输入数字！" << std::endl;
  }
}

// 统计字符串在控制台上的显示宽度：UTF-8 下一个汉字占两列。
int displayWidth(const std::string& text) {
  int width = 0;
  for (std::size_t i = 0; i < text.size(); ++i) {
    unsigned char ch = static_cast<unsigned char>(text[i]);
    if ((ch & 0xC0) == 0x80) {
      continue;  // 多字节字符的后续字节，不重复计数
    }
    width += (ch >= 0xE0) ? 2 : 1;
  }
  return width;
}

// 按显示宽度右补空格，保证中文与数字混排时列对齐。
std::string padRight(const std::string& text, int width) {
  int pad = width - displayWidth(text);
  return pad > 0 ? text + std::string(pad, ' ') : text;
}

// 金额输出：整元不带小数，否则保留两位。
std::string formatMoney(double value) {
  std::ostringstream out;
  if (value == static_cast<double>(static_cast<long long>(value))) {
    out << static_cast<long long>(value);
  } else {
    out << std::fixed << std::setprecision(2) << value;
  }
  return out.str();
}

// 职工类型名，用于月薪发放与工资表的提示文字。
std::string typeName(Person* person) {
  if (dynamic_cast<Salemanager*>(person) != nullptr) {
    return "销售经理";
  }
  if (dynamic_cast<Manager*>(person) != nullptr) {
    return "经理";
  }
  if (dynamic_cast<Technicant*>(person) != nullptr) {
    return "技术员";
  }
  if (dynamic_cast<Saleman*>(person) != nullptr) {
    return "推销员";
  }
  return "职工";
}

// 把月度数据写回职工对象：技术员为工时，推销员为销售额，
// 销售经理为部门销售额，经理无月度数据。
void applyMonthlyFigure(Person* person, double figure) {
  if (Technicant* technician = dynamic_cast<Technicant*>(person)) {
    technician->setWorkHours(figure);
  } else if (Salemanager* salesManager =
                 dynamic_cast<Salemanager*>(person)) {
    salesManager->setDepartmentSales(figure);
  } else if (Saleman* salesman = dynamic_cast<Saleman*>(person)) {
    salesman->setMonthlySales(figure);
  }
}

// 取出职工的月度数据，经理返回 0。
double getMonthlyFigure(Person* person) {
  if (Technicant* technician = dynamic_cast<Technicant*>(person)) {
    return technician->getWorkHours();
  }
  if (Salemanager* salesManager = dynamic_cast<Salemanager*>(person)) {
    return salesManager->getDepartmentSales();
  }
  if (Saleman* salesman = dynamic_cast<Saleman*>(person)) {
    return salesman->getMonthlySales();
  }
  return 0;
}

}  // namespace

App::~App() { clearEmployees(); }

void App::run() {
#ifdef _WIN32
  // 源码与运行时字符串均为 UTF-8，把控制台切到 UTF-8 代码页避免中文乱码。
  SetConsoleOutputCP(CP_UTF8);
#endif
  loadFromFile();

  while (true) {
    showMenu();
    int choice = readChoice();
    switch (choice) {
      case 1:
        inputInfo();
        break;
      case 2:
        addEmployee();
        break;
      case 3:
        deleteEmployee();
        break;
      case 4:
        monthPay();
        break;
      case 5:
        showSalary();
        break;
      case 0:
        saveToFile();
        std::cout << "数据已保存，欢迎再次使用！" << std::endl;
        return;
      default:
        std::cout << "无效的选择，请重新输入！" << std::endl;
        continue;  // 选错时不暂停，直接回到菜单
    }
    pause();
  }
}

void App::showMenu() const {
  std::cout << "\n        公司职工薪酬管理系统\n"
            << "********************************\n"
            << "  1.   录入职工资料\n"
            << "  2.   增加职工\n"
            << "  3.   删除职工\n"
            << "  4.   月薪发放\n"
            << "  5.   显示工资表\n"
            << "  0.   退出系统\n"
            << "********************************\n";
}

int App::readChoice() const {
  std::cout << "请选择：";
  int choice = -1;
  if (!(std::cin >> choice)) {
    if (std::cin.eof()) {
      return 0;  // 输入流已结束，按退出处理
    }
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return choice;
  }
  std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
  return choice;
}

void App::pause() const {
  std::cout << "按任意键继续 ......" << std::endl;
  std::cin.clear();
  std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

void App::inputInfo() {
  std::cout << "录入职工资料会覆盖原先数据，请慎重！！！" << std::endl;
  std::cout << "继续该操作吗？(Y/y)：";
  std::string answer;
  std::getline(std::cin >> std::ws, answer);
  if (answer.empty() || (answer[0] != 'y' && answer[0] != 'Y')) {
    std::cout << "已取消本次录入。" << std::endl;
    return;
  }

  clearEmployees();
  std::cout << "输入职工花名册\n"
            << std::string(kLineWidth, '-') << std::endl;

  std::cout << " 经理姓名：";
  std::string managerName;
  std::getline(std::cin >> std::ws, managerName);
  if (!managerName.empty()) {
    employees.push_back(createEmployee(4, managerName));
  }

  inputNames("输入技术人员名字，若输入@表示结束", "    技术人员姓名：", 3);
  inputNames("输入销售经理名字，若输入@表示结束", "    销售经理姓名：", 2);
  inputNames("输入推销人员名字，若输入@表示结束", "    推销人员姓名：", 1);

  std::cout << "共录入 " << employees.size() << " 名职工资料。" << std::endl;
  saveToFile();
}

void App::inputNames(const std::string& title, const std::string& label,
                     int level) {
  std::cout << " " << title << std::endl;
  while (true) {
    std::cout << label;
    std::string name;
    std::getline(std::cin >> std::ws, name);
    if (name.empty() || name == "@") {
      break;
    }
    employees.push_back(createEmployee(level, name));
  }
}

void App::addEmployee() {
  std::cout << "请输入职工姓名和级别"
               "（4 经理 3 技术员 2 销售经理 1 推销员）：";
  std::string name;
  int level = 0;
  if (!(std::cin >> name >> level)) {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cout << "输入格式有误，操作已取消！" << std::endl;
    return;
  }
  std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

  Person* person = createEmployee(level, name);
  if (person == nullptr) {
    std::cout << "级别必须是 1~4，操作已取消！" << std::endl;
    return;
  }

  employees.push_back(person);
  std::cout << "  已增加职工，编号：" << person->getId()
            << "，姓名：" << person->getName()
            << "，级别：" << person->getLevel() << std::endl;
  saveToFile();
}

void App::deleteEmployee() {
  if (employees.empty()) {
    std::cout << "当前没有职工资料，请先录入！" << std::endl;
    return;
  }

  int id = static_cast<int>(readNumber("请输入需删除的职工编号："));
  for (std::size_t i = 0; i < employees.size(); ++i) {
    if (employees[i]->getId() != id) {
      continue;
    }
    std::string name = employees[i]->getName();
    delete employees[i];
    employees.erase(employees.begin() + i);
    std::cout << "  编号为：" << id << "，姓名：" << name
              << " 的职工已经被删除！" << std::endl;
    saveToFile();
    return;
  }
  std::cout << "  无此 " << id << " 编号的职工！" << std::endl;
}

void App::monthPay() {
  if (employees.empty()) {
    std::cout << "当前没有职工资料，请先录入！" << std::endl;
    return;
  }

  double total = 0;
  for (Person* person : employees) {
    std::string head = padRight(typeName(person), 10) + "职工编号：" +
                       std::to_string(person->getId()) +
                       "，姓名：" + person->getName();

    if (Technicant* technician = dynamic_cast<Technicant*>(person)) {
      technician->setWorkHours(readNumber(head + "，月工作小时："));
    } else if (Salemanager* salesManager =
                   dynamic_cast<Salemanager*>(person)) {
      salesManager->setDepartmentSales(
          readNumber(head + "，月部门销售金额："));
    } else if (Saleman* salesman = dynamic_cast<Saleman*>(person)) {
      salesman->setMonthlySales(readNumber(head + "，月销售金额："));
    } else {
      // 经理固定月薪，无需录入月度数据
      std::cout << head << "，固定月薪为 "
                << formatMoney(person->getMonthlySalary()) << "！"
                << std::endl;
    }
    total += person->getMonthlySalary();
  }

  std::cout << "\n本月应发工资总额：" << formatMoney(total) << std::endl;
  saveToFile();
}

void App::showSalary() {
  if (employees.empty()) {
    std::cout << "当前没有职工资料，请先录入！" << std::endl;
    return;
  }

  std::cout << "\n            职工薪酬表\n\n";
  std::cout << padRight("职工编号", 12) << padRight("姓名", 12)
            << padRight("级别", 8) << "月工资\n";
  std::cout << std::string(kLineWidth, '-') << std::endl;

  double total = 0;
  for (Person* person : employees) {
    double salary = person->getMonthlySalary();
    total += salary;
    std::cout << padRight(std::to_string(person->getId()), 12)
              << padRight(person->getName(), 12)
              << padRight(std::to_string(person->getLevel()), 8)
              << formatMoney(salary) << std::endl;
  }

  std::cout << std::string(kLineWidth, '-') << std::endl;
  std::cout << padRight("合计", 32) << formatMoney(total) << std::endl;
}

Person* App::createEmployee(int level, const std::string& name) const {
  switch (level) {
    case 4:
      return new Manager(name);
    case 3:
      return new Technicant(0, name);
    case 2:
      return new Salemanager(0, name);
    case 1:
      return new Saleman(0, name);
    default:
      return nullptr;
  }
}

void App::loadFromFile() {
  std::ifstream in(kDataFile);
  if (!in) {
    return;  // 首次运行没有数据文件
  }

  int level = 0;
  std::string name;
  double figure = 0;
  int loaded = 0;
  while (in >> level >> name >> figure) {
    Person* person = createEmployee(level, name);
    if (person == nullptr) {
      continue;
    }
    applyMonthlyFigure(person, figure);
    employees.push_back(person);
    ++loaded;
  }

  if (loaded > 0) {
    std::cout << "已从 " << kDataFile << " 载入 " << loaded
              << " 名职工资料。" << std::endl;
  }
}

void App::saveToFile() {
  std::ofstream out(kDataFile);
  if (!out) {
    std::cout << "警告：无法写入数据文件 " << kDataFile << "！" << std::endl;
    return;
  }
  for (Person* person : employees) {
    out << person->getLevel() << ' ' << person->getName() << ' '
        << formatMoney(getMonthlyFigure(person)) << '\n';
  }
}

void App::clearEmployees() {
  for (Person* person : employees) {
    delete person;
  }
  employees.clear();
}

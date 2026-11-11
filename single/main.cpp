#include <algorithm>
#include <climits>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

using namespace std;

class Service;

class Person {
    friend class Service;

private:
    static int nextId;

    int id = 0;

    string name = "";

    int level = 0;

protected:
    int salary = 0;

    virtual void pay() = 0;

public:
    Person(string name, int level)
        : id(nextId++), name(name), level(level) {}

    virtual ~Person() = default;

    int getId() const {
        return id;
    }

    string getName() const {
        return name;
    }

    int getLevel() const {
        return level;
    }

    int getSalary() const {
        return salary;
    }

    double getMonthlySalary() const {
        return salary / 100.0;
    }

    void calculateSalary() {
        pay();
    }
};

class Manager : virtual public Person {
    private:
        void pay() override {
            salary = 1800000;
        }

    public:
        Manager(string name = "unknown manager")
            : Person(name, 4) {}

        ~Manager() = default;
};

class Technician : public Person {
private:
    int workMinutes = 0;

    void pay() override {
        salary = static_cast<int>(1LL * workMinutes * 10000 / 60);
    }

public:
    Technician(string name = "unknown", int workMinutes = 0)
        : Person(name, 3), workMinutes(workMinutes) {}

    int getWorkMinutes() const {
        return workMinutes;
    }

    bool setWorkMinutes(int minutes) {
        if (minutes < 0) {
            return false;
        }
        workMinutes = minutes;
        return true;
    }
};

class Saleman : virtual public Person {
private:
    int sales = 0;

    void pay() override {
        salary = static_cast<int>(1LL * sales * 4 / 100);
    }

public:
    Saleman(string name = "unknown", int sales = 0)
        : Person(name, 1), sales(sales) {}

    int getSales() const {
        return sales;
    }

    bool setSales(int value) {
        if (value < 0) {
            return false;
        }
        sales = value;
        return true;
    }
};

class SaleManager : public Saleman, public Manager {
private:
    void pay() override {
        salary = 500000 + static_cast<int>(1LL * getSales() * 2 / 100);
    }

public:
    SaleManager(string name = "unknown")
        : Person(name, 2), Saleman(name), Manager(name) {}
};

int Person::nextId = 2001;

class Service {
    private:
        vector<Person*> employees;

        Person* findById(int id) const;

        Person* createEmployee(const string& name, int level);

    public:
        Service() = default;
        ~Service();

        Service(const Service&) = delete;
        Service& operator=(const Service&) = delete;

        bool inputInfo(const vector<pair<string, int>>& data);

        int addEmployee(const string& name, int level);

        bool deleteEmployee(int id);

        long long monthPay(const vector<pair<int, int>>& data = {});

        const vector<Person*>& showSalary() const;

        bool saveToFile(const string& file) const;

        bool loadFromFile(const string& file);
};

Person* Service::createEmployee(const string& name, int level) {
    if (name.empty() || name == "@") {
        return nullptr;
    }

    switch (level) {
        case 4: return new Manager(name);
        case 3: return new Technician(name);
        case 2: return new SaleManager(name);
        case 1: return new Saleman(name);
        default: return nullptr;
    }
}

Service::~Service() {
    for (Person* p : employees) delete p;
}

bool Service::inputInfo(const vector<pair<string, int>>& data) {
    int oldNext = Person::nextId;
    vector<Person*> fresh;

    for (const auto& item : data) {
        Person* p = createEmployee(item.first, item.second);

        if (!p) {
            for (Person* old : fresh) delete old;
            Person::nextId = oldNext;
            return false;
        }

        fresh.push_back(p);
    }

    for (Person* p : employees) delete p;
    employees.clear();
    employees.swap(fresh);

    return true;
}

int Service::addEmployee(const string& name, int level) {
    Person* p = createEmployee(name, level);

    if (!p) return 0;

    employees.push_back(p);
    return p->getId();
}

bool Service::deleteEmployee(int id) {
    for (auto it = employees.begin(); it != employees.end(); ++it) {
        if ((*it)->getId() == id) {
            delete *it;
            employees.erase(it);
            return true;
        }
    }

    return false;
}

Person* Service::findById(int id) const {
    for (Person* p : employees) {
        if (p->getId() == id) return p;
    }
    return nullptr;
}

long long Service::monthPay(const vector<pair<int, int>>& data) {
    for (size_t i = 0; i < data.size(); ++i) {
        int id = data[i].first;
        int value = data[i].second;
        Person* p = findById(id);

        if (!p || value < 0) return -1;

        if (p->getLevel() == 3) {
            if (value > INT_MAX / 10000) return -1;
        }
        else if (p->getLevel() == 1 ||
                 p->getLevel() == 2) {
            if (value > INT_MAX / 100) return -1;
        }
        else {
            return -1;
        }

        for (size_t j = 0; j < i; ++j) {
            if (data[j].first == id) return -1;
        }
    }

    for (const auto& item : data) {
        Person* p = findById(item.first);

        if (p->getLevel() == 3) {
            dynamic_cast<Technician*>(p)
                ->setWorkMinutes(item.second * 60);
        }
        else {
            dynamic_cast<Saleman*>(p)
                ->setSales(item.second * 100);
        }
    }

    long long total = 0;

    for (Person* p : employees) {
        p->calculateSalary();
        total += p->getSalary();
    }

    return total;
}

const vector<Person*>& Service::showSalary() const {
    return employees;
}

bool Service::saveToFile(const string& file) const {
    ofstream out(file);
    if (!out) return false;

    out << employees.size() << ' '
        << Person::nextId << '\n';

    for (Person* p : employees) {
        int value = 0;

        switch (p->getLevel()) {
            case 4:
                value = 0;
                break;

            case 3:
                value = dynamic_cast<Technician*>(p)
                    ->getWorkMinutes();
                break;

            case 2:
                value = dynamic_cast<SaleManager*>(p)
                    ->getSales();
                break;

            case 1:
                value = dynamic_cast<Saleman*>(p)
                    ->getSales();
                break;

            default:
                return false;
        }

        out << p->getId() << ' '
            << p->getLevel() << ' '
            << p->getName() << ' '
            << value << '\n';
    }

    return static_cast<bool>(out);
}

bool Service::loadFromFile(const string& file) {
    ifstream in(file);
    if (!in) return false;

    int count, savedNext;

    if (!(in >> count >> savedNext) ||
        count < 0 || count > 100000 ||
        savedNext < 2001) {
        return false;
    }

    int oldNext = Person::nextId;
    int maxId = 2000;
    vector<Person*> fresh;

    Person::nextId = 2001;

    for (int i = 0; i < count; ++i) {
        int id, level, value;
        string name;

        if (!(in >> id >> level >> name >> value) ||
            id < 2001 || id >= savedNext ||
            level < 1 || level > 4 ||
            value < 0 ||
            (level == 3 &&
             1LL * value * 10000 / 60 > INT_MAX)) {

            for (Person* p : fresh) delete p;
            Person::nextId = oldNext;
            return false;
        }

        bool duplicate = false;

        for (Person* p : fresh) {
            if (p->getId() == id) {
                duplicate = true;
                break;
            }
        }

        if (duplicate) {
            for (Person* p : fresh) delete p;
            Person::nextId = oldNext;
            return false;
        }

        Person* p = createEmployee(name, level);

        if (!p) {
            for (Person* old : fresh) delete old;
            Person::nextId = oldNext;
            return false;
        }

        switch (level) {
            case 4:
                break;

            case 3:
                dynamic_cast<Technician*>(p)
                    ->setWorkMinutes(value);
                break;

            case 2:
                dynamic_cast<SaleManager*>(p)
                    ->setSales(value);
                break;

            case 1:
                dynamic_cast<Saleman*>(p)
                    ->setSales(value);
                break;
        }

        p->id = id;

        maxId = max(maxId, id);
        fresh.push_back(p);
    }

    for (Person* p : employees) {
        delete p;
    }

    employees.clear();
    employees.swap(fresh);

    Person::nextId = max(savedNext, maxId + 1);

    monthPay();

    return true;
}

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

    bool drawInputInfo(
        vector<pair<string, int>>& data
    ) const;

    bool drawAddEmployee(
        string& name, int& level
    ) const;

    int drawDeleteEmployee() const;

    vector<pair<int, int>> drawMonthPay(
        const vector<Person*>& employees
    ) const;

    void drawShowSalary(
        const vector<Person*>& employees,
        long long total
    ) const;

    void showMessage(const string& message) const;
    void pause() const;
};

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

int UI::drawDeleteEmployee() const {
    return readNumber(
        "输入要删除的职工编号：",
        2001, 2147483647
    );
}

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

class App {
private:
    Service service;

    UI ui;

public:
    void run();
};

void App::run() {
    const string filename = "employees.txt";

    service.loadFromFile(filename);

    bool running = true;

    while (running) {
        ui.drawMenu();
        Page choice = ui.handleKeyboard();

        switch (choice) {

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

            case Page::SHOWSALARY: {
                long long total = service.monthPay();

                ui.drawShowSalary(
                    service.showSalary(),
                    total
                );

                break;
            }

            case Page::EXIT:
                running = false;
                break;

            default:
                break;
        }

        if (running) {
            ui.pause();
        }
    }
}

int main() {
    App app;
    app.run();
    return 0;
}

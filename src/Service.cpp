#include "Service.h"
#include "Employee/Manager.h"
#include "Employee/Technician.h"
#include "Employee/Saleman.h"
#include "Employee/SaleManager.h"
#include <algorithm>
#include <fstream>
#include <climits>

using namespace std;

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

// 1. 批量录入：覆盖原有数据
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

// 2. 增加员工
int Service::addEmployee(const string& name, int level) {
    Person* p = createEmployee(name, level);

    if (!p) return 0;

    employees.push_back(p);
    return p->getId();
}

// 3. 删除员工
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

// 按工号查找
Person* Service::findById(int id) const {
    for (Person* p : employees) {
        if (p->getId() == id) return p;
    }
    return nullptr;
}

// 4. 月薪发放
long long Service::monthPay(const vector<pair<int, int>>& data) {
    // 先验证所有输入
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

    // 分别设置各员工的本月工作数据
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

    // 分别计算每名员工工资，再汇总
    long long total = 0;

    for (Person* p : employees) {
        p->calculateSalary();
        total += p->getSalary();
    }

    return total;
}

// 5. 显示工资表的数据
const vector<Person*>& Service::showSalary() const {
    return employees;
}

// 6. 保存数据
bool Service::saveToFile(const string& file) const {
    ofstream out(file);
    if (!out) return false;

    out << employees.size() << ' '
        << Person::nextId << '\n';

    for (Person* p : employees) {
        int value = 0;

        switch (p->getLevel()) {
            case 4: // 经理：固定工资
                value = 0;
                break;

            case 3: // 技术员：工作分钟数
                value = dynamic_cast<Technician*>(p)
                    ->getWorkMinutes();
                break;

            case 2: // 销售经理：部门销售额（分）
                value = dynamic_cast<SaleManager*>(p)
                    ->getSales();
                break;

            case 1: // 推销员：个人销售额（分）
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

// 7. 读取数据

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

    // 读取前暂时重置工号计数器
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

        // 检查工号是否重复
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

        // 按职位创建员工对象
        Person* p = createEmployee(name, level);

        if (!p) {
            for (Person* old : fresh) delete old;
            Person::nextId = oldNext;
            return false;
        }

        // 恢复不同职位的工作数据
        switch (level) {
            case 4: // 经理：没有额外工作数据
                break;

            case 3: // 技术员：工作分钟数
                dynamic_cast<Technician*>(p)
                    ->setWorkMinutes(value);
                break;

            case 2: // 销售经理：部门销售额
                dynamic_cast<SaleManager*>(p)
                    ->setSales(value);
                break;

            case 1: // 推销员：个人销售额
                dynamic_cast<Saleman*>(p)
                    ->setSales(value);
                break;
        }

        // 恢复原来的工号
        p->id = id;

        maxId = max(maxId, id);
        fresh.push_back(p);
    }

    // 读取成功后替换旧数据
    for (Person* p : employees) {
        delete p;
    }

    employees.clear();
    employees.swap(fresh);

    // 恢复下一工号，避免重复分配
    Person::nextId = max(savedNext, maxId + 1);

    // 根据读取的数据重新计算工资
    monthPay();

    return true;
}
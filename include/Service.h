#pragma once
#include "Employee/Person.h"
#include <string>
#include <utility>
#include <vector>

class Service {
    private:
        std::vector<Person*> employees;

        Person* findById(int id) const;
    
        Person* createEmployee(const std::string& name, int level);

    public:
        Service() = default;
        ~Service();

        Service(const Service&) = delete;
        Service& operator=(const Service&) = delete;

        // 批量覆盖：每条数据为 {姓名, 级别}
        bool inputInfo(const std::vector<std::pair<std::string, int>>& data);

        int addEmployee(const std::string& name, int level);
        
        bool deleteEmployee(int id);

        // 月薪输入：{工号, 数值}
        // 技术员：小时；推销员：销售额（元）
        // 返回工资总额（分），-1 代表输入不合法
        long long monthPay(const std::vector<std::pair<int, int>>& data = {});

        const std::vector<Person*>& showSalary() const;

        bool saveToFile(const std::string& file) const;
        
        bool loadFromFile(const std::string& file);
};
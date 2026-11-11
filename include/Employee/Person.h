#pragma once
#include <string>

class Service;

class Person {
    friend class Service;  // 仅供读取文件时恢复原工号

private:
    static int nextId;

    int id = 0;

    std::string name = "";
    
    int level = 0;

protected:
    int salary = 0;  // 单位：分

    virtual void pay() = 0;

public:
    Person(std::string name, int level)
        : id(nextId++), name(name), level(level) {}

    virtual ~Person() = default;

    int getId() const { 
        return id;
    }

    std::string getName() const { 
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
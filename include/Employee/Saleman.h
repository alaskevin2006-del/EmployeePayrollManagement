#pragma once
#include "Person.h"

class Saleman : virtual public Person {
private:
    int sales = 0;  // 单位：分

    void pay() override {
        salary = sales * 4 / 100;
    }

public:
    Saleman(std::string name = "unknown", int sales = 0)
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
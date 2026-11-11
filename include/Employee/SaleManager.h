#pragma once
#include "Saleman.h"
#include "Manager.h"

class SaleManager : public Saleman, public Manager {
private:
    void pay() override {
        // getSales() 存放所属部门的销售总额，单位：分
        salary = 500000 + static_cast<int>(1LL * getSales() * 2 / 100);
    }

public:
    SaleManager(std::string name = "unknown")
        : Person(name, 2), Saleman(name), Manager(name) {}
};
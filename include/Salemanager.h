#pragma once

#include "Person.h"
#include "Manager.h"
#include "Saleman.h"

class Salemanager : public Manager, public Saleman {
    private:
        double departmentSales = 0;

        void pay() {
            monthlySalary = 5000 + departmentSales * 0.02;
        }

    public:
        Salemanager(double departmentSales = 0, string name = "unknown", int level = 2, double monthlySalary = 5000)
            : departmentSales(departmentSales), Person(name, 2, monthlySalary){}

        ~Salemanager(){}

        double getDepartmentSales() {
            return departmentSales;
        }

        void setDepartmentSales(double departmentSales) {
            this->departmentSales = departmentSales;
        }

        double getMonthlySalary() final {
            pay();
            return monthlySalary;
        }
};
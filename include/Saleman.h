#pragma once

#include "Person.h"

class Saleman : virtual public Person {
    private:
        double monthlySales = 0;

        void pay() override {
            monthlySalary = monthlySales * 0.04;
        }

    public:
        Saleman(double monthlySales = 0, string name = "unknown", int level = 1, double monthlySalary = 0)
            : monthlySales(monthlySales), Person(name, 1, monthlySalary){}
        
        double getMonthlySales() {
            return monthlySales;
        }

        void setMonthlySales(double monthlySales) {
            this->monthlySales = monthlySales;
        }

        double getMonthlySalary() override {
            pay();
            return monthlySalary;
        }
};
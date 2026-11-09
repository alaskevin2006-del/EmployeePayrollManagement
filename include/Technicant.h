#pragma once

#include "Person.h"

class Technicant : public Person {
    private:
        double workHours = 0;
        void pay() override {
            monthlySalary = workHours * 100;
        }

    public:
        Technicant(double workHours = 0, string name = "unknown", int level = 3, double monthlySalary = 18000)
            : workHours(workHours), Person(name, 3, 18000){}

        ~Technicant(){}
        
        double getMonthlySalary() override {
            pay();
            return monthlySalary;
        }
};
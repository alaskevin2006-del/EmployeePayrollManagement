#pragma once

#include "Person.h"

class Manager : virtual public Person {
    private:
        void pay() override {
            monthlySalary = 18000;
        }

    public:
        Manager(string name = "unknown", int level = 4, double monthlySalary = 18000)
            : Person(name, 4, 18000){}
        
        ~Manager(){}
        
        double getMonthlySalary() override {
            pay();
            return monthlySalary;
        }
};
#pragma once

#include "Person.h"

class Technicant : public Person {
    private:
        double workHours = 0;
        void pay() override {
            monthlySalary = workHours * 100;
        }

    public:
        Technicant(double workHours = 0, string name = "unknown")
            : workHours(workHours), Person(name, 3, 0) {}

        ~Technicant(){}

        double getWorkHours() {
            return workHours;
        }

        void setWorkHours(double workHours) {
            this->workHours = workHours;
        }
        
        double getMonthlySalary() override {
            pay();
            return monthlySalary;
        }
};
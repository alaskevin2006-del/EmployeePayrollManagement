#pragma once
#include "Person.h"

class Technician : public Person {
private:
    int workMinutes = 0;

    void pay() override {
        salary = workMinutes * 10000 / 60;
    }

public:
    Technician(std::string name = "unknown", int workMinutes = 0)
        : Person(name, 3), workMinutes(workMinutes) {}

    int getWorkMinutes() const { 
        return workMinutes;
    }

    bool setWorkMinutes(int minutes) {
        if (minutes < 0) {
            return false;
        }
        workMinutes = minutes;
        return true;
    }
};
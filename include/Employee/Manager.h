#pragma once
#include "Person.h"

class Manager : virtual public Person {
    private:
        void pay() override {
            salary = 1800000;
        }
    
    public:
        Manager(std::string name = "unknown manager")
            : Person(name, 4) {}

        ~Manager() = default;
};
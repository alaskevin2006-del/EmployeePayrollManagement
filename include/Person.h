#pragma once

#include <string>
using namespace std;

class Person {
    private:
        static int count;

        int id = 2000;
        
        string name = "unknown";
        
        int level = 0;
        
        virtual void pay() = 0;

    protected:
        double monthlySalary = 0;
    
    public:
        Person(string name = "unknown", int level = 0, double monthlySalary = 0)
            : name(name), level(level), monthlySalary(monthlySalary) {
                count++;
                id = 2000 + count;
        }

        virtual ~Person() = default;

        bool setLevel(int level) {
            this->level = level;
            return this->level >= 1 && this->level <=4;
        }

        int getId() {
            return id;
        }

        string getName() {
            return name;
        }

        int getLevel() {
            return level;
        }

        virtual double getMonthlySalary() = 0;
};
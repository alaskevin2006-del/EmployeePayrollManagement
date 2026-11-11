#pragma once
#include "Service.h"
#include "UI.h"

class App {
private:
    Service service;
    
    UI ui;

public:
    void run();
};
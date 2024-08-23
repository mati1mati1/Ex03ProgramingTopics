#pragma once
#include <string>
class Simulator{
    public:
        virtual ~Simulator() = default;
        virtual void run(std::string name) = 0;
};
#pragma once
class Simulator{
    public:
        virtual ~Simulator() = default;
        virtual void run() = 0;
};
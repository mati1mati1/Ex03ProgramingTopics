#pragma once
#include "enums.h"

inline std::ostream& operator<<(std::ostream& os, const Step& step) {
    switch (step) {
        case Step::North: os << "N"; break;
        case Step::East: os << "E"; break;
        case Step::South: os << "S"; break;
        case Step::West: os << "W"; break;
        case Step::Stay: os << "s"; break;
        case Step::Finish: os << "F"; break;
    }
    return os;
}
inline Step operator~(Step dir) {
    switch (dir) {
        case Step::North: return Step::South;
        case Step::East: return Step::West;
        case Step::South: return Step::North;
        case Step::West: return Step::East;
        case Step::Stay: return Step::Stay;
        case Step::Finish: return Step::Finish;
    }
    return Step::Finish;
}
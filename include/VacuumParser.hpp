#pragma once
#include "VacuumPayload.hpp"
#include <string>
#include <filesystem>
#include <memory>
class VacuumParser
{
    public:
        VacuumParser() { };
        std::unique_ptr<VacuumPayload> parse(const std::filesystem::path& fileInputpath);
        private:
            std::vector<std::string> readlines(const std::filesystem::path& fileInputpath);
            std::optional<uint32_t> parseNumberFromLine(const std::string& line, const std::string& name);
            std::optional<std::vector<std::vector<HouseLocation>>> getHouseLocations(std::vector<std::string> houseLines);
};
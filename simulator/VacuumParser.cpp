#include "VacuumParser.hpp"
#include <fstream>
#include <iostream>
#include <vector>
#include <filesystem>
#include <memory>
#include <regex>

int extractNumberFromString(const std::string& line,const std::string& name) {
    std::string pattern = name + R"(\s*=\s*(\d+))";
    std::regex regexPattern(pattern);
    std::smatch match;
    if (std::regex_search(line, match, regexPattern)) {
        try{
            return std::stoi(match.str(1));
        }
        catch (const std::exception& e) {
            std::cerr << "Failed Parsing for " << line << " Reason: " << e.what() << '\n';
            return -1;
        }
    }
    return -1;
}
std::vector<std::string> VacuumParser::readlines(const std::filesystem::path& fileInputpath) {
    std::ifstream file(fileInputpath);
    std::string line;
    if (!file.is_open()) {
        return {};
    }
    std::vector<std::string> lines;
    while (std::getline(file, line)) {
        lines.push_back(line);
    }
    return lines;
}

std::optional<uint32_t> VacuumParser::parseNumberFromLine(const std::string& line, const std::string& name) {

    int parsed = extractNumberFromString(line, name);
    if (parsed == -1) {
        std::cerr << "Failed Parsing for " << line << " Reason: No Number Found" << '\n';
        return std::nullopt;
    }
    if (parsed < 0) {
        std::cerr << "Failed Parsing for " << line << " Reason: Negative Value" << '\n';
        return std::nullopt;
    }
    return parsed;
}
std::unique_ptr<VacuumPayload> VacuumParser::parse(const std::filesystem::path& fileInputpath)
{
    std::vector<std::string> lines = readlines(fileInputpath);
    if (lines.empty()) {
        std::cerr << "Failed to read file: " << fileInputpath << std::endl;
        return nullptr;
    }
    if (lines.size() < 6) {
        std::string error = "Failed to parse VacuumParser from file: " + fileInputpath.string() + ". Not enough lines couldn't possibly be a valid configuration";
        std::cerr << error <<std::endl ;
        return nullptr;
    }

    std::optional<uint32_t> maxSteps = parseNumberFromLine(lines[1], "MaxSteps");
    std::optional<uint32_t> maxBatterySteps = parseNumberFromLine(lines[2], "MaxBattery");
    std::optional<uint32_t> rows = parseNumberFromLine(lines[3],"Rows");
    std::optional<uint32_t> cols = parseNumberFromLine(lines[4],"Cols");

    if (!(maxSteps.has_value() && maxBatterySteps.has_value() && rows.has_value() && cols.has_value())) {
        std::string error = "Error: Failed to parse VacuumParser from file: " + fileInputpath.string() + ". Invalid values.";
        std::cerr << error << std::endl;
        return nullptr;
    }
    auto houseLines = std::vector<std::string>(lines.begin() + 5, lines.end()); // Skip the first 5 lines, which contains everything that isn't the house representation
    try {
        VacuumHouse house(houseLines, *rows, *cols);
        MeteredVacuumBattery battery(*maxBatterySteps,*maxBatterySteps);
        VacuumPayload payload(house, battery, *maxSteps);
        return std::make_unique<VacuumPayload>(payload);

    } catch (const std::exception& e) {
        std::string error = "Error: Failed to create House or VacuumSimulator. Reason: " + std::string(e.what());
        std::cerr << error << std::endl;
        return nullptr;
    }
    return nullptr;
}
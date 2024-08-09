#pragma once
#include "CleaningRecord.hpp"
#include "VacuumPayload.hpp"
#include "VacuumParser.hpp"
#include "OutFileWriter.hpp"
#include "AbstractAlgorithm.h"
#include "Simulator.hpp"
#include <filesystem>

class VacuumSimulator : public Simulator
{
public:
    VacuumSimulator() {};
    void run(std::string algorithmName) override;
    std::shared_ptr<CleaningRecord> calculate();
    void setAlgorithm(std::unique_ptr<AbstractAlgorithm> algorithm);
    void readHouseFile(const std::filesystem::path &fileInputpath);

private:
    std::optional<CleaningRecordStep> applyStep(VacuumPayload &payload, Step step);
    bool canRun() { return payload != nullptr && algorithm != nullptr; }
    void cleanCurrentLocation();

private:
    std::unique_ptr<VacuumPayload> payload = nullptr;
    std::unique_ptr<AbstractAlgorithm> algorithm = nullptr;
    std::filesystem::path fileInputpath;
};

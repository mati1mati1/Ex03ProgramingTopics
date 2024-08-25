#pragma once
#include "CleaningRecord.hpp"
#include "VacuumPayload.hpp"
#include "AbstractAlgorithm.h"
#include "Simulator.hpp"
#include <filesystem>
#include <memory>

class VacuumSimulator : public Simulator
{
public:
    VacuumSimulator() {};
    void run() override;
    void setAlgorithm(std::unique_ptr<AbstractAlgorithm> algorithm);
    void readHouseFile(const std::filesystem::path &fileInputpath);
    auto getMaxTime() const { return payload->getMaxTime(); }
    void timeout() { timedOut = true; };
    std::filesystem::path exportRecord(std::string algorithmName);
    std::filesystem::path exportSummary(std::string algorithmName,bool errored);
    friend class SpecificAlgorithmTest;
    friend class VacuumSimulatorTest;
private:
    std::optional<CleaningRecordStep> applyStep(VacuumPayload &payload, Step step);
    std::shared_ptr<CleaningRecord> calculate();
    bool canRun() { return payload != nullptr && algorithm != nullptr; }
    void cleanCurrentLocation();
    void canExport();
    void writeSummary(std::string houseName,std::filesystem::path outputPath,std::string algorithmName,bool errored);
    void writeOutFile(std::ofstream &writeStream);

private:
    std::shared_ptr<CleaningRecord> record = nullptr;
    std::unique_ptr<VacuumPayload> payload = nullptr;
    std::unique_ptr<AbstractAlgorithm> algorithm = nullptr;
    std::filesystem::path fileInputpath;
    bool timedOut = false;
};

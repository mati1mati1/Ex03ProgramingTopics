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
    void run(std::string algorithmName) override;
    std::filesystem::path exportRecord(const std::shared_ptr<CleaningRecord> record, const std::filesystem::path &fileOutputpath, bool timedOut);
    void setAlgorithm(std::unique_ptr<AbstractAlgorithm> algorithm);
    void readHouseFile(const std::filesystem::path &fileInputpath);
    auto getMaxTime() const { return payload->getMaxTime(); }
    std::filesystem::path exportRecord(bool timedOut);
    std::filesystem::path exportSummary(bool timedOut);
    friend class SpecificAlgorithmTest;
private:
    std::optional<CleaningRecordStep> applyStep(VacuumPayload &payload, Step step);
    std::shared_ptr<CleaningRecord> calculate();
    bool canRun() { return payload != nullptr && algorithm != nullptr; }
    void cleanCurrentLocation();
    void canExport();
    void writeSummary(std::string houseName,std::filesystem::path outputPath,bool timedOut);
    void writeOutFile(std::ofstream &writeStream,bool timedOut);

private:
    std::shared_ptr<CleaningRecord> record = nullptr;
    std::unique_ptr<VacuumPayload> payload = nullptr;
    std::unique_ptr<AbstractAlgorithm> algorithm = nullptr;
    std::filesystem::path fileInputpath;
    std::string algorithmName;
};

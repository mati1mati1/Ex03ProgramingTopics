#pragma once
#include "CleaningRecord.hpp"
#include "AlgorithmConfig.hpp"
#include <filesystem> 
#include "VacuumPayload.hpp"
#include "Simulator.hpp"

class VacuumSimulator : public Simulator{
    public:
        VacuumSimulator() { };
        void run() override;
        std::shared_ptr<CleaningRecord> calculate();
        void setAlgorithm(const std::shared_ptr<AlgorithmConfig> config);
        void readHouseFile(const std::filesystem::path& fileInputpath);
        
    private:
        std::optional<CleaningRecordStep> applyStep(VacuumPayload& payload, Step step);
        bool canRun() {return config !=nullptr && payload!=nullptr;}
        void cleanCurrentLocation();
    private:
        std::unique_ptr<VacuumPayload> payload = nullptr;
        std::shared_ptr<AlgorithmConfig> config = nullptr;
        std::filesystem::path fileInputpath;
};


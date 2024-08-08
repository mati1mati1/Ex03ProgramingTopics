#include "VacuumSimulator.hpp"
#include "VacuumParser.hpp"
#include <OutFileWriter.hpp>
#include <Logger.hpp>
#include "AlgorithmFactory.hpp"
bool isAtDockingStation(std::shared_ptr<CleaningRecord> record) {return record->size() == 0 || record->last()->getLocationType() == LocationType::CHARGING_STATION;}
bool isStuck(std::shared_ptr<CleaningRecord> record) {return record->size() != 0 && record->last()->getBatteryLevel() == 0 && !isAtDockingStation(record);}
bool isAtMaxSteps(std::shared_ptr<CleaningRecord> record,uint32_t maxSteps) {return record->size() == maxSteps + 1;}
bool shouldTerminate(std::shared_ptr<CleaningRecord> record,uint32_t maxSteps) {return isAtMaxSteps(record,maxSteps);};
bool isMissionSuccessful(std::shared_ptr<CleaningRecord> record);

void VacuumSimulator::run(){

    auto record = calculate();
    if (record == nullptr){
        Logger::getInstance().log("Error: Failed to calculate record");
        std::cerr << "cannot run!" << std::endl;
        throw std::runtime_error("cannot run!");
        return;
    }
    Logger::getInstance().log("Writing output file");
    OutFileWriter writer;
    writer.write(fileInputpath, record, config->getAlgorithmName());
    Logger::getInstance().log("Output file written");

}
std::shared_ptr<CleaningRecord> VacuumSimulator::calculate(){
    if (!canRun()){
        Logger::getInstance().log("Warn Cannot run, missing configuration");
        std::cerr << "Cannot run, missing configuration" << std::endl;
        return nullptr;
    }
    auto algorithm = AlgorithmFactory::createAlgorithm(*config);
    auto runPayload = VacuumPayload(*this->payload);
    algorithm->setBatteryMeter(runPayload.getBattery());
    algorithm->setWallsSensor(runPayload.getHouse());
    algorithm->setDirtSensor(runPayload.getHouse());
    algorithm->setMaxSteps(runPayload.getMaxSteps());
    auto record = std::make_shared<CleaningRecord>(CleaningRecordStep(LocationType::CHARGING_STATION, Step::Stay,runPayload.getBattery().getBatteryState(),runPayload.getHouse().getTotalDirt()),runPayload.getMaxSteps());
    Logger::getInstance().log("Running VacuumSimulator");
    while(!shouldTerminate(record,runPayload.getMaxSteps())) {
        Logger::getInstance().log("Algorithm step:");
        Logger::getInstance().log(std::to_string(record->size()));
        auto step = algorithm->nextStep();
        if (record->size() == 1510){
            Logger::getInstance().log("Initial house state:");
            Logger::getInstance().log(runPayload.getHouse());
        }
        if (step == Step::Finish)
        {
            Logger::getInstance().log("Algorithm finished");
            record->add(CleaningRecordStep(LocationType::CHARGING_STATION, Step::Finish,runPayload.getBattery().getBatteryState(),runPayload.getHouse().getTotalDirt()));
            break;
        }
        auto nextMove = applyStep(runPayload,step);
        if(!nextMove.has_value()) {
            std::cerr << "Invalid step returned by algorithm, terminating execution" << std::endl;
            Logger::getInstance().log("Invalid step returned by algorithm, terminating execution");
            break;
        }
        Logger::getInstance().log(nextMove.value());
        Logger::getInstance().log("The house representation after the move:");
        Logger::getInstance().log(runPayload.getHouse());
        record->add(nextMove.value());
    }
    Logger::getInstance().log("VacuumSimulator finished");
    return record;
}

bool isMissionSuccessful(std::shared_ptr<CleaningRecord> record){
    if (record->size() == 0){
        return record->getInitialStep()->getDirtLevel() == 0;
    }
    auto dirtLevel = record->last()->getDirtLevel();
    bool success = dirtLevel == 0 && isAtDockingStation(record);
    Logger::getInstance().log("Checking mission success: Last step dirt level is " + std::to_string(dirtLevel) + 
                              ", at docking station: " + (isAtDockingStation(record) == 0 ? "yes" : "no" ) + ", success: " + (success == 0 ? "yes" : "no" ));
    return success;
}

std::optional<CleaningRecordStep> VacuumSimulator::applyStep(VacuumPayload& payload, Step step) {

    auto& house = payload.getHouse();
    auto& battery = payload.getBattery();

    bool is_valid = house.is_move(step);
    if(!is_valid) {
        Logger::getInstance().log("Invalid move, ignoring");
        return std::nullopt;
    }
    if(step != Step::Stay) {
        if(!battery.try_activate()) {
            Logger::getInstance().log("VacuumSimulator not activated, move ignored");
            return std::nullopt;
        }
        house.move(DirectionTools::reduceStepToDirection(step));
        auto location = house.getCurrentLocation();
        return CleaningRecordStep(location.getLocationType(), step,battery.getBatteryState(),house.getTotalDirt());
    }
    auto location = house.getCurrentLocation();
    auto locationType = location.getLocationType();
    if(locationType == LocationType::CHARGING_STATION) {
        Logger::getInstance().log("Charging vacuum at charging station");
        battery.charge();
    } else if(locationType == LocationType::HOUSE_TILE) {
        bool isActivated = battery.try_activate();
        if(!isActivated) {
            Logger::getInstance().log("VacuumSimulator not activated, move ignored");
            // VacuumSimulator is not activated, ignore
            return std::nullopt;
        }    
        house.cleanCurrentLocation();
    }
    return CleaningRecordStep(locationType,step,battery.getBatteryState(), house.getTotalDirt());
}

void VacuumSimulator::setAlgorithm(const std::shared_ptr<AlgorithmConfig> config)
{
    this->config = config;
}
void VacuumSimulator::readHouseFile(const std::filesystem::path& fileInputpath)
{
    VacuumParser parser;
    std::unique_ptr<VacuumPayload> parsedPayload = parser.parse(fileInputpath);
    if (parsedPayload == nullptr) {
        Logger::getInstance().log("Error: Failed to parse house file: " + fileInputpath.string());
        std::cerr << "Failed to parse house file: " << fileInputpath << std::endl;
        return;
    }
    payload = std::move(parsedPayload);
    this->fileInputpath = fileInputpath;
}
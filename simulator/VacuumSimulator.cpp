#include "VacuumSimulator.hpp"

bool isAtDockingStation(std::shared_ptr<CleaningRecord> record) { return record->size() == 0 || record->last()->getLocationType() == LocationType::CHARGING_STATION; }
bool isStuck(std::shared_ptr<CleaningRecord> record) { return record->size() != 0 && record->last()->getBatteryLevel() == 0 && !isAtDockingStation(record); }
bool isAtMaxSteps(std::shared_ptr<CleaningRecord> record, uint32_t maxSteps) { return record->size() == maxSteps + 1; }
bool shouldTerminate(std::shared_ptr<CleaningRecord> record, uint32_t maxSteps) { return isAtMaxSteps(record, maxSteps); };
bool isMissionSuccessful(std::shared_ptr<CleaningRecord> record);
void VacuumSimulator::setAlgorithm(std::unique_ptr<AbstractAlgorithm> algorithm)
{
    this->algorithm = std::move(algorithm);
}
void VacuumSimulator::run(std::string algorithmName)
{

    auto record = calculate();
    if (record == nullptr)
    {
        std::cerr << "cannot run!" << std::endl;
        throw std::runtime_error("cannot run!");
        return;
    }
    OutFileWriter writer;
    writer.write(fileInputpath, record, algorithmName);
}
std::shared_ptr<CleaningRecord> VacuumSimulator::calculate()
{
    if (!canRun())
    {
        std::cerr << "Cannot run, missing configuration" << std::endl;
        return nullptr;
    }
    auto runPayload = VacuumPayload(*this->payload);
    algorithm->setBatteryMeter(runPayload.getBattery());
    algorithm->setWallsSensor(runPayload.getHouse());
    algorithm->setDirtSensor(runPayload.getHouse());
    algorithm->setMaxSteps(runPayload.getMaxSteps());
    auto record = std::make_shared<CleaningRecord>(CleaningRecordStep(LocationType::CHARGING_STATION, Step::Stay, runPayload.getBattery().getBatteryState(), runPayload.getHouse().getTotalDirt()), runPayload.getMaxSteps());
    while (!shouldTerminate(record, runPayload.getMaxSteps()))
    {
        auto step = algorithm->nextStep();
        if (step == Step::Finish)
        {
            record->add(CleaningRecordStep(LocationType::CHARGING_STATION, Step::Finish, runPayload.getBattery().getBatteryState(), runPayload.getHouse().getTotalDirt()));
            break;
        }
        auto nextMove = applyStep(runPayload, step);
        if (!nextMove.has_value())
        {
            std::cerr << "Invalid step returned by algorithm, terminating execution" << std::endl;
            break;
        }
        record->add(nextMove.value());
    }
    return record;
}

bool isMissionSuccessful(std::shared_ptr<CleaningRecord> record)
{
    if (record->size() == 0)
    {
        return record->getInitialStep()->getDirtLevel() == 0;
    }
    auto dirtLevel = record->last()->getDirtLevel();
    bool success = dirtLevel == 0 && isAtDockingStation(record);
    return success;
}

std::optional<CleaningRecordStep> VacuumSimulator::applyStep(VacuumPayload &payload, Step step)
{

    auto &house = payload.getHouse();
    auto &battery = payload.getBattery();

    bool is_valid = house.is_move(step);
    if (!is_valid)
    {
        return std::nullopt;
    }
    if (step != Step::Stay)
    {
        if (!battery.try_activate())
        {
            return std::nullopt;
        }
        house.move(DirectionTools::reduceStepToDirection(step));
        auto location = house.getCurrentLocation();
        return CleaningRecordStep(location.getLocationType(), step, battery.getBatteryState(), house.getTotalDirt());
    }
    auto location = house.getCurrentLocation();
    auto locationType = location.getLocationType();
    if (locationType == LocationType::CHARGING_STATION)
    {
        battery.charge();
    }
    else if (locationType == LocationType::HOUSE_TILE)
    {
        bool isActivated = battery.try_activate();
        if (!isActivated)
        {
            // VacuumSimulator is not activated, ignore
            return std::nullopt;
        }
        house.cleanCurrentLocation();
    }
    return CleaningRecordStep(locationType, step, battery.getBatteryState(), house.getTotalDirt());
}

void VacuumSimulator::readHouseFile(const std::filesystem::path &fileInputpath)
{
    VacuumParser parser;
    std::unique_ptr<VacuumPayload> parsedPayload = parser.parse(fileInputpath);
    if (parsedPayload == nullptr)
    {
        std::cerr << "Failed to parse house file: " << fileInputpath << std::endl;
        throw std::invalid_argument("Failed to parse house file");
    }
    payload = std::move(parsedPayload);
    this->fileInputpath = fileInputpath;
}
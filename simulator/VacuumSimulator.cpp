#include "VacuumSimulator.hpp"
#include "ScoreCalculator.hpp"
#include "VacuumParser.hpp"
#include <filesystem>
#include <fstream>
#include <algorithm>

void VacuumSimulator::setAlgorithm(std::unique_ptr<AbstractAlgorithm> algorithm)
{
    this->algorithm = std::move(algorithm);
}

const std::filesystem::path getOutFilePath(const std::filesystem::path &fileOutputPath, const std::string &algorithmName)
{
    return std::filesystem::current_path() / (fileOutputPath.stem().string() + "-" + algorithmName + ".txt");
}
const std::filesystem::path getSummaryFilePath()
{
    return std::filesystem::current_path() / "summary.csv";
}
void VacuumSimulator::run()
{
    this->timedOut = false;
    calculate();
    if (record == nullptr)
    {
        std::cerr << "run failed" << std::endl;
        throw std::runtime_error("run failed");
        return;
    }

}

void VacuumSimulator::canExport()
{
    if (record)
    {
        return;
    }
    std::cerr << "No results to add to summary." << std::endl;
    throw std::runtime_error("No results.");
    
}

std::filesystem::path VacuumSimulator::exportRecord(std::string algorithmName)
{
    auto fileOutputpath = getOutFilePath(fileInputpath, algorithmName);
    std::ofstream writeStream(fileOutputpath);
    canExport();
    if (!writeStream.is_open())
    {
        std::cerr << "Unable to open file." << std::endl;    
        throw std::runtime_error("Unable to open file.");
    }
    writeOutFile(writeStream);
    return fileOutputpath;
    
}
std::filesystem::path VacuumSimulator::exportSummary(std::string algorithmName,bool errored)
{
    auto fileOutputpath = getSummaryFilePath();
    std::string houseName = fileInputpath.stem().string();
    canExport();
    writeSummary(houseName,fileOutputpath,algorithmName,errored);
    return fileOutputpath;
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
    record = std::make_shared<CleaningRecord>(CleaningRecordStep(LocationType::CHARGING_STATION, Step::Stay, runPayload.getBattery().getBatteryState(), runPayload.getHouse().getTotalDirt()), runPayload.getMaxSteps());
    while (record->getMaxSteps() >= record->size() && !timedOut)
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
    this->record = nullptr;
}

void VacuumSimulator::writeSummary(std::string houseName, std::filesystem::path path, std::string algorithmName,bool errored)
{
    std::ifstream inFile(path);
    std::ofstream outFile;
    std::vector<std::string> houseNames;
    std::vector<std::string> algorithmNames;
    std::vector<std::vector<std::string>> tableData;
    std::string line;

    uint32_t score;
    if(errored){
        score = UINT32_MAX;
    }
    else{
        score = VacuumScoreCalculator().calculateScore(record, timedOut);
    }

    if (inFile.is_open())
    {
        bool firstLine = true;
        while (std::getline(inFile, line))
        {
            std::stringstream ss(line);
            std::string cell;

            if (firstLine)
            {
                while (std::getline(ss, cell, ','))
                {
                    if (!cell.empty())
                    {
                        houseNames.push_back(cell);
                    }
                }
                firstLine = false;
            }
            else
            {
                std::vector<std::string> rowData;
                bool hasData = false;

                while (std::getline(ss, cell, ','))
                {
                    rowData.push_back(cell);
                    if (!cell.empty())
                    {
                        hasData = true;
                    }
                }

                if (hasData)
                {
                    algorithmNames.push_back(rowData[0]);
                    tableData.push_back(rowData);
                }
            }
        }
        inFile.close();
    }

    bool newHouse = std::find(houseNames.begin(), houseNames.end(), houseName) == houseNames.end();
    bool newAlgorithm = true;

    size_t algoIndex = 0;
    for (size_t i = 0; i < algorithmNames.size(); ++i)
    {
        if (algorithmNames[i] == algorithmName)
        {
            newAlgorithm = false;
            algoIndex = i;
            break;
        }
    }

    if (newHouse)
    {
        houseNames.push_back(houseName);
    }

    if (newAlgorithm)
    {
        algorithmNames.push_back(algorithmName);
        tableData.push_back(std::vector<std::string>(houseNames.size() + 1, "")); 
        tableData.back()[0] = algorithmName; 
        algoIndex = tableData.size() - 1;
    }

    size_t houseIndex = std::distance(houseNames.begin(), std::find(houseNames.begin(), houseNames.end(), houseName));

    std::string scoreStr = std::to_string(score);
    if (errored)
    {
        scoreStr = "";
    }
    if (tableData[algoIndex].size() <= houseIndex + 1)
    {
        tableData[algoIndex].resize(houseIndex + 2, ""); 
    }

    tableData[algoIndex][houseIndex+1] = scoreStr;


    outFile.open(path, std::ios_base::out | std::ios_base::trunc);

    if (outFile.is_open())
    {
        houseNames.insert(houseNames.begin(), "");
        for (const auto &house : houseNames)
        {
            outFile << house << ",";
        }
        outFile << "\n";

        for (const auto &row : tableData)
        {
            for (const auto &cell : row)
            {
                outFile << cell << ",";
            }
            outFile << "\n";
        }
        outFile.close();
    }
}


void VacuumSimulator::writeOutFile(std::ofstream &writeStream)
{
    auto recordLast = record->last();
    auto inDock = recordLast->isAtDockingStation(); 
    auto score = VacuumScoreCalculator().calculateScore(record, timedOut);
    writeStream << "NumSteps = " << record->size() << std::endl;
    writeStream << "DirtLeft = " << recordLast->getDirtLevel() << std::endl;
    writeStream << "Status = " << record->getStatus() << std::endl;
    writeStream << "InDock = " << (inDock ? "TRUE" : "FALSE") << std::endl;
    writeStream << "Score = " << score << std::endl;
    writeStream << "Steps: \n"
            << *record << std::endl;
    writeStream.close();
    
}
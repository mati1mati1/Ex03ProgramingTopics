#include "OutFileWriter.hpp"
#include "Logger.hpp"
#include <iostream>
#include <fstream>
#include <filesystem>

std::filesystem::path OutFileWriter::write(const std::filesystem::path& inputFileName, const std::shared_ptr<CleaningRecord> record, const std::string& algorithmName) {
    if (!record) {
        std::cerr << "No results to save." << std::endl;
        Logger::getInstance().log("Error: No results to save.");
        return std::filesystem::path();
    }
    const std::filesystem::path fileOutputpath = getFileName(inputFileName, algorithmName);
    Logger::getInstance().log("Saving results to: " + fileOutputpath.string());
    createDirectoryIfNotExists(inputFileName.parent_path());
    std::ofstream outFile(fileOutputpath);
    writeOutfile(outFile, fileOutputpath, record);
    return fileOutputpath;
}
const std::filesystem::path OutFileWriter::getFileName(const std::filesystem::path &fileOutputPath, const std::string &algorithmName)
{
   return fileOutputPath.parent_path() / (fileOutputPath.stem().string() + "-" + algorithmName + ".txt");
}
void OutFileWriter::writeOutfile(std::ofstream &outFile, const std::filesystem::path &fileOutputpath, const std::shared_ptr<CleaningRecord> record)
{
    if (outFile.is_open())
    {
        
        auto recordLast = record->last();
        auto status = getStatusString(recordLast);
        auto inDock = recordLast->getLocationType() == LocationType::CHARGING_STATION ;
        auto score = getScore(status, record, inDock);
        outFile << "NumSteps = " << record->size() << std::endl;
        outFile << "DirtLeft = " << recordLast->getDirtLevel() << std::endl;
        outFile << "Status = " << status << std::endl;
        outFile << "InDock = " << (inDock ? "TRUE" : "FALSE") << std::endl;
        outFile << "Score = " << score << std::endl;
        outFile << "Steps: \n" << *record << std::endl;
        outFile.close();
        Logger::getInstance().log("Results successfully saved to file: " + fileOutputpath.string());
    }
    else
    {
        std::cerr << "Unable to open file." << std::endl;
        Logger::getInstance().log("Error: Unable to open file: " + fileOutputpath.string());
    }
}
void OutFileWriter::createDirectoryIfNotExists(const std::filesystem::path &fileOutputDirectory)
{
    if (!std::filesystem::is_directory(fileOutputDirectory))
    {
        Logger::getInstance().log("Directory Not found, creating" + fileOutputDirectory.string());
        try{
            std::filesystem::create_directories(fileOutputDirectory);
        }catch(const std::exception& e){
            Logger::getInstance().log("Error creating directory: " + fileOutputDirectory.string());
        }
        
    }
}

std::string OutFileWriter::getStatusString(const std::shared_ptr<CleaningRecordStep>& step) {
    if (step->getStep() == Step::Finish) {
        return "FINISHED";
    }
    if (step->getLocationType() != LocationType::CHARGING_STATION && step->getBatteryLevel() == 0) {
        return "DEAD";
    }
    return "WORKING";
}

uint32_t OutFileWriter::getScore(const std::string status, const std::shared_ptr<CleaningRecord> record, bool inDock) {
        if (status == "DEAD"){
            return record->getMaxSteps() + baseScore(record) + 2000;
        }

        if (status == "FINISHED" && !inDock)
        {
            return record->getMaxSteps() + baseScore(record) + 3000;
        }

        return record->size() + baseScore(record) + (inDock ? 0 : 1000);
}
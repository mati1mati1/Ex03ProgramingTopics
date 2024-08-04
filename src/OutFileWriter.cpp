#include "OutFileWriter.hpp"
#include "Logger.hpp"
#include <iostream>
#include <fstream>
#include <filesystem>

std::filesystem::path OutFileWriter::write(const std::filesystem::path& inputFileName, const std::shared_ptr<CleaningRecord> record) {
    if (!record) {
        std::cerr << "No results to save." << std::endl;
        Logger::getInstance().log("Error: No results to save.");
        return std::filesystem::path();
    }
    const std::filesystem::path fileOutputpath = getFileName(inputFileName);
    Logger::getInstance().log("Saving results to: " + fileOutputpath.string());
    createDirectoryIfNotExists(inputFileName.parent_path());
    std::ofstream outFile(fileOutputpath);
    writeOutfile(outFile, fileOutputpath, record);
    return fileOutputpath;
}
const std::filesystem::path OutFileWriter::getFileName(const std::filesystem::path &fileOutputPath)
{
   return fileOutputPath.parent_path() / (std::string("output_") + fileOutputPath.filename().c_str());
}
void OutFileWriter::writeOutfile(std::ofstream &outFile, const std::filesystem::path &fileOutputpath, const std::shared_ptr<CleaningRecord> record)
{
    if (outFile.is_open())
    {
        
        auto recordLast = record->last();

        outFile << "NumSteps = " << record->size() << std::endl;
        outFile << "DirtLeft = " << recordLast->getDirtLevel() << std::endl;

        if (recordLast->getStep() == Step::Finish)
        {
            outFile << "Status = FINISHED" << std::endl;
        }
        else if (recordLast->getLocationType() != LocationType::CHARGING_STATION && recordLast->getBatteryLevel() == 0)
        {
            outFile << "Status = DEAD" << std::endl;
        }
        else
        {
            outFile << "Status = WORKING" << std::endl;
        }

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
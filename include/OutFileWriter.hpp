#pragma once
#include <filesystem>
#include <string>
#include "CleaningRecord.hpp"
class OutFileWriter {
public:
    OutFileWriter() { };
    std::filesystem::path write(const std::filesystem::path& inputFileName, const std::shared_ptr<CleaningRecord> record);
private:
    const std::filesystem::path getFileName(const std::filesystem::path &fileOutputDirectory);
    void writeOutfile(std::ofstream &outFile, const std::filesystem::path &fileOutputpath, const std::shared_ptr<CleaningRecord> record);
    void createDirectoryIfNotExists(const std::filesystem::path &fileOutputDirectory);
};
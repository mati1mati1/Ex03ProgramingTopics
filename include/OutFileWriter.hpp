#pragma once
#include <filesystem>
#include <string>
#include "CleaningRecord.hpp"
class OutFileWriter {
public:
    OutFileWriter() { };
    std::filesystem::path write(const std::filesystem::path& inputFileName, const std::shared_ptr<CleaningRecord> record, const std::string& algorithmName);
private:
    const std::filesystem::path getFileName(const std::filesystem::path &fileOutputPath, const std::string &algorithmName);
    void writeOutfile(std::ofstream &outFile, const std::filesystem::path &fileOutputpath, const std::shared_ptr<CleaningRecord> record);
    void createDirectoryIfNotExists(const std::filesystem::path &fileOutputDirectory);
    static std::string getStatusString(const std::shared_ptr<CleaningRecordStep>& step);
    uint32_t getScore(const std::string status, const std::shared_ptr<CleaningRecord> record, bool inDock);
    uint32_t baseScore(const std::shared_ptr<CleaningRecord> record){return record->last()->getDirtLevel() * 300;};
};
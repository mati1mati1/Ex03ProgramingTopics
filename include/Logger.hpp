#pragma once
#include <fstream>
#include <string>
#include <iostream>
#include <sstream>
#include <filesystem>

class Logger {
public:
    static Logger& getInstance() {
        static Logger instance;
        return instance;
    }
    template <typename T>
    void log(const T& obj) {
        std::ostringstream oss;
        oss << obj;
        *logStream << oss.str() << "\n" ;
    }

    void setLogStream(std::ostream* stream) {
        logStream = stream;
    }

    void resetLogStream() {
        logStream = &std::cerr;
    }

    void setLogFile(const std::string& filePath) {
        std::filesystem::path path(filePath);
        try{
            if (!std::filesystem::exists(path.parent_path())) {
                std::filesystem::create_directories(path.parent_path());
            }
        }
        catch (const std::exception& e) {
            std::cerr << "Failed to create log directory:" << std::endl;
            return;
        }

        setFileStream(filePath);
    }

    void setFileStream(const std::string &filePath)
    {
        if (fileStream.is_open())
        {
            fileStream.close();
        }
        fileStream.open(filePath, std::ios::out | std::ios::app);
        if (fileStream.is_open())
        {
            logStream = &fileStream;
        }
        else
        {
            std::cerr << "Failed to open log file: " + filePath << std::endl;
        }
    }

private:
    Logger()
    {
        setFileStream("/dev/null");
    }

    ~Logger() {
        if (fileStream.is_open()) {
            fileStream.close();
        }
    }

    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    std::ostream* logStream;
    std::ofstream fileStream;
};

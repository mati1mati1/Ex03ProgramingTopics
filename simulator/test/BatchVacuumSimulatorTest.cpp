#include "BatchVacuumSimulator.hpp"
#include "SimulationArguments.hpp"
#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem; 

class BatchVacuumSimulatorTest : public ::testing::Test {
protected:
    void SetUp() override {
        std::cout << std::filesystem::current_path() << std::endl;
        for (const auto& entry : fs::directory_iterator(fs::current_path())) {
            std::string filename = entry.path().filename().string();
            if (filename_not_contains(filename, "CMake") && 
                (entry.path().extension() == ".txt" || entry.path().extension() == ".error" || filename == "summary.csv")) {
                fs::remove(entry.path());
            }
        }
    }
    bool filename_not_contains(const std::string& filename, const std::string& substring) {
        return filename.find(substring) == std::string::npos;
    }
    void TearDown() override {
    }

    bool fileExists(const std::string& path) {
        return fs::exists(path);
    }
std::string generateOutputFileName(const std::string& houseFileName, const std::string& algoFileName) {
    std::string algoStem = fs::path(algoFileName).stem().string();
    const std::string libPrefix = "lib";
    if (algoStem.find(libPrefix) == 0) {
        algoStem.erase(0, libPrefix.length());
    }

    return fs::path(houseFileName).stem().string() + "-" + algoStem + ".txt";
}
    void insertFilesWithExtension(const fs::path &path, std::vector<fs::path> &files, const std::string &extension)
    {
        auto iterator = fs::directory_iterator(path);
        std::copy_if(iterator, fs::directory_iterator(), std::back_inserter(files), [&](const fs::path &path) { return path.extension() == extension; });
        if (files.empty())
        {
            throw std::invalid_argument("No "+ extension +"files found in directory: " + path.string());
        }
    }
    std::vector<std::filesystem::path> houseFiles;
    std::vector<std::filesystem::path> algoFiles;
};

TEST_F(BatchVacuumSimulatorTest, ValidHousesAndAlgorithms) {
    const char* housePath = "../test/examples/cleaningTest";
    const char* algoPath = "../lib";

    std::string house_arg = std::string("-house_path=") + housePath;
    std::string algo_arg = std::string("-algo_path=") + algoPath;

    const char* argv[] = {"myrobot", house_arg.c_str(), algo_arg.c_str()};
    int argc = sizeof(argv) / sizeof(char*);
    EXPECT_NO_THROW({
        SimulationArguments args(argc, const_cast<char**>(argv));
        BatchVacuumSimulator simulator;
        simulator.run(args);
    });
    insertFilesWithExtension(housePath, houseFiles, ".house");
    insertFilesWithExtension(algoPath, algoFiles, ".so");
    for (const auto& houseFile : houseFiles) {
        std::string houseErrorFile = fs::path(houseFile.stem().string()).string() + ".error";    
        ASSERT_FALSE(fileExists(houseErrorFile)) << "Error file exists for " << houseFile;
        for (const auto& algoFile : algoFiles) {
            std::string outputFileName = generateOutputFileName(houseFile, algoFile);
            ASSERT_TRUE(fileExists(outputFileName)) << "Output file " << outputFileName << " does not exist!";

        }
    }

    for (const auto& algoFile : algoFiles) {
        std::string algoErrorFile = fs::path(algoFile.stem().string()).string() + ".error";    
        ASSERT_FALSE(fileExists(algoErrorFile)) << "Error file exists for " << algoFile;
    }
    ASSERT_TRUE(fileExists("summary.csv")) << "summary file dont exist";

       
}
TEST_F(BatchVacuumSimulatorTest, ValidHousesAndInvalidHouses) {
    const char* housePath = "../test/examples/mixFailerAndSuccesHouse";
    const char* algoPath = "../lib";

    std::string house_arg = std::string("-house_path=") + housePath;
    std::string algo_arg = std::string("-algo_path=") + algoPath;

    const char* argv[] = {"myrobot", house_arg.c_str(), algo_arg.c_str()};
    int argc = sizeof(argv) / sizeof(char*);
    EXPECT_NO_THROW({
        SimulationArguments args(argc, const_cast<char**>(argv));
        BatchVacuumSimulator simulator;
        simulator.run(args);
    });
    insertFilesWithExtension(housePath, houseFiles, ".house");
    insertFilesWithExtension(algoPath, algoFiles, ".so");
    for (const auto& houseFile : houseFiles) {
        std::string houseErrorFile = fs::path(houseFile.stem().string()).string() + ".error"; 
        if (houseFile.stem().string().rfind("faild", 0) == 0) {
            ASSERT_TRUE(fileExists(houseErrorFile)) << "Error file exists for " << houseFile;
        } else {
            ASSERT_FALSE(fileExists(houseErrorFile)) << "Error file exists for " << houseFile;
            for (const auto& algoFile : algoFiles) {
                std::string outputFileName = generateOutputFileName(houseFile, algoFile);
                ASSERT_TRUE(fileExists(outputFileName)) << "Output file " << outputFileName << " does not exist!";
            }
        }  
    }
    for (const auto& algoFile : algoFiles) {
        std::string algoErrorFile = fs::path(algoFile.stem().string()).string() + ".error";    
        ASSERT_FALSE(fileExists(algoErrorFile)) << "Error file exists for " << algoFile;
    }
    ASSERT_TRUE(fileExists("summary.csv")) << "summary file dont exist"; 
}

TEST_F(BatchVacuumSimulatorTest, InvalidHouseFile) {
    const char* housePath = "../test/examples/failtests";
    const char* algoPath = "../lib";

    std::string house_arg = std::string("-house_path=") + housePath;
    std::string algo_arg = std::string("-algo_path=") + algoPath;

    const char* argv[] = {"myrobot", house_arg.c_str(), algo_arg.c_str()};
    int argc = sizeof(argv) / sizeof(char*);
    EXPECT_NO_THROW({
        SimulationArguments args(argc, const_cast<char**>(argv));
        BatchVacuumSimulator simulator;
        simulator.run(args);
    });
    insertFilesWithExtension(housePath, houseFiles, ".house");
    insertFilesWithExtension(algoPath, algoFiles, ".so");
    for (const auto& houseFile : houseFiles) {
        std::string houseErrorFile = fs::path(houseFile.stem().string()).string() + ".error";    
        ASSERT_TRUE(fileExists(houseErrorFile)) << "Error file exists for " << houseFile;
    }

    for (const auto& algoFile : algoFiles) {
        std::string algoErrorFile = fs::path(algoFile.stem().string()).string() + ".error";    
        ASSERT_FALSE(fileExists(algoErrorFile)) << "Error file exists for " << algoFile;
    }

}
TEST_F(BatchVacuumSimulatorTest, SummaryOnlyOption) {
    const char* housePath = "../test/examples/cleaningTest";
    const char* algoPath = "../lib";

    std::string house_arg = std::string("-house_path=") + housePath;
    std::string algo_arg = std::string("-algo_path=") + algoPath;

    const char* argv[] = {"myrobot", house_arg.c_str(), algo_arg.c_str(), "-summary_only"};
    int argc = sizeof(argv) / sizeof(char*);
    EXPECT_NO_THROW({
        SimulationArguments args(argc, const_cast<char**>(argv));
        BatchVacuumSimulator simulator;
        simulator.run(args);
    });
    insertFilesWithExtension(housePath, houseFiles, ".house");
    insertFilesWithExtension(algoPath, algoFiles, ".so");
    for (const auto& houseFile : houseFiles) {
        std::string houseErrorFile = fs::path(houseFile.stem().string()).string() + ".error";    
        ASSERT_FALSE(fileExists(houseErrorFile)) << "Error file exists for " << houseFile;
        for (const auto& algoFile : algoFiles) {
            std::string outputFileName = generateOutputFileName(houseFile, algoFile);
            ASSERT_FALSE(fileExists(outputFileName)) << "Output file " << outputFileName << " does not exist!";
        }
    }
    for (const auto& algoFile : algoFiles) {
        std::string algoErrorFile = fs::path(algoFile.stem().string()).string() + ".error";    
        ASSERT_FALSE(fileExists(algoErrorFile)) << "Error file exists for " << algoFile;
    }
    ASSERT_TRUE(fileExists("summary.csv")) << "summary file dont exist";

}
TEST_F(BatchVacuumSimulatorTest, ErrorFileForInvalidAlgorithm) {
    const char* housePath = "../test/examples/cleaningTest";
    const char* algoPath = "../badLib";

    std::string house_arg = std::string("-house_path=") + housePath;
    std::string algo_arg = std::string("-algo_path=") + algoPath;

    const char* argv[] = {"myrobot", house_arg.c_str(), algo_arg.c_str(), "-summary_only"};
    int argc = sizeof(argv) / sizeof(char*);
    EXPECT_NO_THROW({
        SimulationArguments args(argc, const_cast<char**>(argv));
        BatchVacuumSimulator simulator;
        simulator.run(args);
    });
    insertFilesWithExtension(housePath, houseFiles, ".house");
    insertFilesWithExtension(algoPath, algoFiles, ".so");
    for (const auto& houseFile : houseFiles) {
        std::string houseErrorFile = fs::path(houseFile.stem().string()).string() + ".error";    
        ASSERT_FALSE(fileExists(houseErrorFile)) << "Error file exists for " << houseFile;
        for (const auto& algoFile : algoFiles) {
            std::string outputFileName = generateOutputFileName(houseFile, algoFile);
            ASSERT_FALSE(fileExists(outputFileName)) << "Output file " << outputFileName << " does not exist!";
        }
    }
    for (const auto& algoFile : algoFiles) {
        std::string algoErrorFile = fs::path(algoFile.stem().string()).string() + ".error";    
        ASSERT_TRUE(fileExists(algoErrorFile)) << "Error file exists for " << algoFile;
    }
    ASSERT_FALSE(fileExists("summary.csv")) << "summary file dont exist";
}

TEST_F(BatchVacuumSimulatorTest, ValidAndInvalidAlgorithm) {
    const char* housePath = "../test/examples/cleaningTest";
    const char* algoPath = "../badAndGoodLib";

    std::string house_arg = std::string("-house_path=") + housePath;
    std::string algo_arg = std::string("-algo_path=") + algoPath;

    const char* argv[] = {"myrobot", house_arg.c_str(), algo_arg.c_str()};
    int argc = sizeof(argv) / sizeof(char*);
    EXPECT_NO_THROW({
        SimulationArguments args(argc, const_cast<char**>(argv));
        BatchVacuumSimulator simulator;
        simulator.run(args);
    });
    insertFilesWithExtension(housePath, houseFiles, ".house");
    insertFilesWithExtension(algoPath, algoFiles, ".so");
    for (const auto& houseFile : houseFiles) {
        std::string houseErrorFile = fs::path(houseFile.stem().string()).string() + ".error";    
        ASSERT_FALSE(fileExists(houseErrorFile)) << "Error file exists for " << houseFile;
        for (const auto& algoFile : algoFiles) {
            std::string algoName = fs::path(algoFile.stem().string()).string(); 
            std::string outputFileName = generateOutputFileName(houseFile, algoFile);   
            if (algoName != "libAlgo_323012971_315441972_Simultaneous" && algoName != "libAlgo_323012971_315441972_Orignal") {
                ASSERT_FALSE(fileExists(outputFileName)) << "Output file " << outputFileName << " does exist!";
            }
            else
            {
                ASSERT_TRUE(fileExists(outputFileName)) << "Output file " << outputFileName << " does not exist!";  
            }
            
        }
    }
    for (const auto& algoFile : algoFiles) {
        std::string algoName = fs::path(algoFile.stem().string()).string();    
        std::string algoErrorFile = fs::path(algoFile.stem().string()).string() + ".error";    
        if(algoName != "libAlgo_323012971_315441972_Simultaneous" && algoName != "libAlgo_323012971_315441972_Orignal"){
            ASSERT_TRUE(fileExists(algoErrorFile)) << "Error file dont exists for " << algoFile;
        }
        else{
            ASSERT_FALSE(fileExists(algoErrorFile)) << "Error file exists for " << algoFile;
        }
    }
    ASSERT_TRUE(fileExists("summary.csv")) << "summary file dont exist";
}

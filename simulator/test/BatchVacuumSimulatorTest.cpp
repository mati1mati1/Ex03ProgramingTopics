#include "BatchVacuumSimulator.hpp"
#include "SimulationArguments.hpp"
#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include <iostream>

namespace fs = std::filesystem;

struct TestParams {
    fs::path housePath;
    fs::path algoPath;
    bool summaryOnly;
    bool shouldCsv;
    std::vector<std::string> validAlgorithms;
};
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
    
    bool isCSVRectangular(const std::filesystem::path& csvFilePath) {
        std::ifstream file(csvFilePath);
        if (!file.is_open()) {
            std::cerr << "Error: Unable to open file " << csvFilePath << std::endl;
            return false;
        }

        std::string line;
        size_t columnCount = 0;
        size_t lineNumber = 0;

        while (std::getline(file, line)) {
            ++lineNumber;
            std::stringstream lineStream(line);
            std::string cell;
            std::vector<std::string> cells;

            while (std::getline(lineStream, cell, ',')) {
                cells.push_back(cell);
            }

            if (lineNumber == 1) {
                columnCount = cells.size();
            } else if (cells.size() != columnCount) {
                return false;
            }
        }

        return true;
    }
    void insertFilesWithExtension(const fs::path& path, std::vector<fs::path>& files, const std::string& extension) {
        auto iterator = fs::directory_iterator(path);
        std::copy_if(iterator, fs::directory_iterator(), std::back_inserter(files), [&](const fs::path& path) { return path.extension() == extension; });
        if (files.empty()) {
            throw std::invalid_argument("No " + extension + " files found in directory: " + path.string());
        }
    }
    void loadRun(const TestParams& params)
    {
        std::string house_arg = std::string("-house_path=") + params.housePath.string();
        std::string algo_arg = std::string("-algo_path=") + params.algoPath.string();

        std::vector<const char*> argv = {"Simulator", house_arg.c_str(), algo_arg.c_str()};
        if (params.summaryOnly) {
            argv.push_back("-summary_only");
        }
        int argc = argv.size();

        EXPECT_NO_THROW({
            SimulationArguments args(argc, const_cast<char**>(argv.data()));
            BatchVacuumSimulator simulator;
            simulator.run(args);
        });        
        insertFilesWithExtension(params.housePath, houseFiles, ".house");
        insertFilesWithExtension(params.algoPath, algoFiles, ".so");
        if(params.shouldCsv) {
            ASSERT_TRUE(isCSVRectangular("summary.csv"));
        }
        else {
            ASSERT_FALSE(fileExists("summary.csv"));
        }
        assertNotBothErrorAndOutput();
    }
    double timedOutRatio(const TestParams& params)
    {
        int maxTimeouts = params.validAlgorithms.size() * houseFiles.size();
        int timeouts = 0;
        for (const auto& algorithm : params.validAlgorithms) {
            for (const auto& houseFile : houseFiles) {
                std::string outputFileName = generateOutputFileName(houseFile, algorithm);
                if(wasTimedOut(outputFileName)) {
                    timeouts++;
                }
            }
        }
        double ratio = (double)timeouts / maxTimeouts;
        return ratio;
    }

    double erroredOutFileRatio(const TestParams& params)
    {
        int maxErrors = params.validAlgorithms.size() * houseFiles.size();
        int error = 0;
        for (const auto& algorithm : params.validAlgorithms) {
            for (const auto& houseFile : houseFiles) {
                std::string outputFileName = generateOutputFileName(houseFile, algorithm);
                if(!fileExists(outputFileName)) {
                    error++;
                }
            }
        }
        double ratio = (double)error / maxErrors;
        return ratio;
    }
    bool wasTimedOut(const std::string& outputFileName) {
        std::ifstream outputFileStream(outputFileName);
        std::string line;
        std::getline (outputFileStream, line);
        std::getline (outputFileStream, line);
        std::getline (outputFileStream, line);
        bool isTimeout = line.find("WORKING") != std::string::npos || line.find("DEAD") != std::string::npos;
        return isTimeout;
    }
    std::string getOutputFileErrorName(const std::filesystem::path& houseFile)
    {
        std::string houseErrorFile = fs::path(houseFile.stem().string()).string() + ".error";
        return houseErrorFile;
    }
    void assertNotBothErrorAndOutput()
    {
        for (const auto& houseFile : houseFiles) {
            std::string houseErrorFile = getOutputFileErrorName(houseFile);
            std::string houseFileName = houseFile.filename().string();
            ASSERT_FALSE(fileExists(houseErrorFile) && fileExists(houseFileName)) << "Both error and output files exist for " << houseFile;
        }
        for (const auto& algoFile : algoFiles) {
            std::string algoErrorFile =getOutputFileErrorName(algoFile);
            ASSERT_FALSE(fileExists(algoErrorFile) && fileExists(algoFile.filename())) << "Both error and output files exist for " << algoFile;
        }
    }
    void assertCorrectErrorFilesCreated(const TestParams& params)
    {
        for (const auto& houseFile : houseFiles) {
            std::string houseErrorFile = getOutputFileErrorName(houseFile);
            bool isFailedHouse = houseFile.stem().string().find("failed") != std::string::npos;

            if (isFailedHouse) {
                ASSERT_TRUE(fileExists(houseErrorFile)) << "Error file does not exist for " << houseFile;
            } else {
                ASSERT_FALSE(fileExists(houseErrorFile)) << "Error file exists for " << houseFile;
                for (const auto& algoFile : algoFiles) {
                    std::string outputFileName = generateOutputFileName(houseFile, algoFile);
                    bool isValidAlgo = std::find(params.validAlgorithms.begin(), params.validAlgorithms.end(), fs::path(algoFile).stem().string()) != params.validAlgorithms.end();
                    if (params.summaryOnly || !isValidAlgo) {
                        ASSERT_FALSE(fileExists(outputFileName)) << "Output file " << outputFileName << " exists!";
                    } else {
                        ASSERT_TRUE(fileExists(outputFileName)) << "Output file " << outputFileName << " does not exist!";
                    }
                }
            }
        }
    }
    void assertCorrectAlgorithmErrorFilesCreated(const TestParams& params)
    {
        for (const auto& algoFile : algoFiles) {
            std::string algoErrorFile = getOutputFileErrorName(algoFile);
            bool isValidAlgo = std::find(params.validAlgorithms.begin(), params.validAlgorithms.end(), fs::path(algoFile).stem().string()) != params.validAlgorithms.end();

            if (!isValidAlgo) {
                ASSERT_TRUE(fileExists(algoErrorFile)) << "Error file does not exist for " << algoFile;
            } else {
                ASSERT_FALSE(fileExists(algoErrorFile)) << "Error file exists for " << algoFile;
            }
        }
    }
    std::vector<std::filesystem::path> houseFiles;
    std::vector<std::filesystem::path> algoFiles;
    public:
        inline static const fs::path LIBPATH = "../../lib";
        inline static const fs::path ALLLIBS = "../../allLib";
        inline static const fs::path CLEANINGTEST = "../../simulator/test/examples/cleaningTest";
        inline static const fs::path MIXFAILERANDSUCCESHOUSE = "../../simulator/test/examples/mixFailerAndSuccesHouse";
        inline static const fs::path FAILTESTS = "../../simulator/test/examples/failtests";
        inline static const fs::path BADLIB = "../../badLib";
        inline static const fs::path RUNTIMEBADLIB = "../../runtimeBadLib";
        inline static const fs::path BADANDGOODLIB = "../../badAndGoodLib";
        inline static const fs::path TIMEOUTFAULTYALGORITHMS = "../../timeoutfaultyAlgorithms";
        inline static const fs::path LIBFAULTY = "../../semifaultyAlgorithms";
        inline static const fs::path SOMETIMESTIMEOUTLIB = "../../sometimetimeout";
        inline static const fs::path TIMEOUTTEST = "../../simulator/test/examples/timeoutTests";
};


class BatchVacuumSimulatorParameterizedTest : public BatchVacuumSimulatorTest, public ::testing::WithParamInterface<TestParams> {
};

TEST_P(BatchVacuumSimulatorParameterizedTest, RunSimulation) {
    const auto& params = GetParam();
    loadRun(params);
    assertCorrectErrorFilesCreated(params);
    assertCorrectAlgorithmErrorFilesCreated(params);

}

INSTANTIATE_TEST_SUITE_P(
    BatchVacuumSimulatorTests,
    BatchVacuumSimulatorParameterizedTest,
    ::testing::Values(
        TestParams{BatchVacuumSimulatorTest::CLEANINGTEST, BatchVacuumSimulatorTest::LIBPATH, false,true, {"libAlgo_323012971_315441972_Simultaneous", "libAlgo_323012971_315441972_Orignal"}},
        TestParams{BatchVacuumSimulatorTest::MIXFAILERANDSUCCESHOUSE, BatchVacuumSimulatorTest::LIBPATH, false,true, {"libAlgo_323012971_315441972_Simultaneous", "libAlgo_323012971_315441972_Orignal"}},
        TestParams{BatchVacuumSimulatorTest::FAILTESTS, BatchVacuumSimulatorTest::LIBPATH, false,false, {"libAlgo_323012971_315441972_Simultaneous", "libAlgo_323012971_315441972_Orignal"}},
        TestParams{BatchVacuumSimulatorTest::CLEANINGTEST, BatchVacuumSimulatorTest::LIBPATH, true,true, {"libAlgo_323012971_315441972_Simultaneous", "libAlgo_323012971_315441972_Orignal"}},
        TestParams{BatchVacuumSimulatorTest::CLEANINGTEST, BatchVacuumSimulatorTest::BADLIB, true,false, {}},
        TestParams{BatchVacuumSimulatorTest::CLEANINGTEST, BatchVacuumSimulatorTest::BADANDGOODLIB, false, true,{"libAlgo_323012971_315441972_Simultaneous", "libAlgo_323012971_315441972_Orignal"}}
    )
);

TEST_F(BatchVacuumSimulatorTest, RunSimulationWithRuntimeError) {
    const auto& params = TestParams{TIMEOUTTEST, RUNTIMEBADLIB, false, true,{"libtimingOut"}};
    loadRun(params);
    assertCorrectAlgorithmErrorFilesCreated(params);
    ASSERT_EQ(timedOutRatio(params), 1.0);
}


TEST_F(BatchVacuumSimulatorTest, RunSimulationWithSometimesTimeoutMultipleAlgorithms) {
    const auto& params = TestParams{TIMEOUTTEST, SOMETIMESTIMEOUTLIB, false,true, {"libtimingOutSometimes"}};
    loadRun(params);
    assertCorrectAlgorithmErrorFilesCreated(params);
    ASSERT_LT(timedOutRatio(params), 1);
    ASSERT_GT(timedOutRatio(params), 0);

}
TEST_F(BatchVacuumSimulatorTest, FaultyRuntimeAlgorithm) {
    const auto& params = TestParams{ CLEANINGTEST,LIBFAULTY, false,true, {"libfaultyAlgorithm"}};
    loadRun(params);
    assertCorrectAlgorithmErrorFilesCreated(params);
    ASSERT_EQ(erroredOutFileRatio(params), 1.0);
}
TEST_F(BatchVacuumSimulatorTest, RunTimeFaultyOrTimeoutOrGoodAlgorithm) {
    const auto& params = TestParams{ TIMEOUTTEST,TIMEOUTFAULTYALGORITHMS, false, true,{"libtimingOutSometimesFaultySometimes"}};
    loadRun(params);
    assertCorrectAlgorithmErrorFilesCreated(params);
    ASSERT_LT(timedOutRatio(params), 1);
    ASSERT_GT(timedOutRatio(params), 0);
    ASSERT_LT(erroredOutFileRatio(params), 1);
    ASSERT_GT(erroredOutFileRatio(params), 0);
}
TEST_F(BatchVacuumSimulatorTest, AllAlgorithmsMixedResults)
{
    const auto& params = TestParams{ MIXFAILERANDSUCCESHOUSE,ALLLIBS, false, true,
    {"libtimingOutSometimesFaultySometimes","libtimingOut","libtimingOutSometimes","libfaultyAlgorithm","libAlgo_323012971_315441972_Simultaneous", "libAlgo_323012971_315441972_Orignal"}};
    loadRun(params);
    assertCorrectAlgorithmErrorFilesCreated(params);
    ASSERT_LT(timedOutRatio(params), 1);
    ASSERT_GT(timedOutRatio(params), 0);
    ASSERT_LT(erroredOutFileRatio(params), 1);
    ASSERT_GT(erroredOutFileRatio(params), 0);
    
}
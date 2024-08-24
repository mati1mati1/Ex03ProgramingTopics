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

    void insertFilesWithExtension(const fs::path& path, std::vector<fs::path>& files, const std::string& extension) {
        auto iterator = fs::directory_iterator(path);
        std::copy_if(iterator, fs::directory_iterator(), std::back_inserter(files), [&](const fs::path& path) { return path.extension() == extension; });
        if (files.empty()) {
            throw std::invalid_argument("No " + extension + " files found in directory: " + path.string());
        }
    }

    std::vector<std::filesystem::path> houseFiles;
    std::vector<std::filesystem::path> algoFiles;
    public:
        inline static const fs::path LIBPATH = "../../lib";
        inline static const fs::path CLEANINGTEST = "../test/examples/cleaningTest";
        inline static const fs::path MIXFAILERANDSUCCESHOUSE = "../test/examples/mixFailerAndSuccesHouse";
        inline static const fs::path FAILTESTS = "../test/examples/failtests";
        inline static const fs::path BADLIB = "../../badLib";
        inline static const fs::path BADANDGOODLIB = "../../badAndGoodLib";
};

struct TestParams {
    fs::path housePath;
    fs::path algoPath;
    bool summaryOnly;
    std::vector<std::string> validAlgorithms;
};

class BatchVacuumSimulatorParameterizedTest : public BatchVacuumSimulatorTest, public ::testing::WithParamInterface<TestParams> {
};

TEST_P(BatchVacuumSimulatorParameterizedTest, RunSimulation) {
    const auto& params = GetParam();

    std::string house_arg = std::string("-house_path=") + params.housePath.string();
    std::string algo_arg = std::string("-algo_path=") + params.algoPath.string();

    std::vector<const char*> argv = {"myrobot", house_arg.c_str(), algo_arg.c_str()};
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

    for (const auto& houseFile : houseFiles) {
        std::string houseErrorFile = fs::path(houseFile.stem().string()).string() + ".error";
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

    for (const auto& algoFile : algoFiles) {
        std::string algoErrorFile = fs::path(algoFile.stem().string()).string() + ".error";
        bool isValidAlgo = std::find(params.validAlgorithms.begin(), params.validAlgorithms.end(), fs::path(algoFile).stem().string()) != params.validAlgorithms.end();

        if (!isValidAlgo) {
            ASSERT_TRUE(fileExists(algoErrorFile)) << "Error file does not exist for " << algoFile;
        } else {
            ASSERT_FALSE(fileExists(algoErrorFile)) << "Error file exists for " << algoFile;
        }
    }
}

INSTANTIATE_TEST_SUITE_P(
    BatchVacuumSimulatorTests,
    BatchVacuumSimulatorParameterizedTest,
    ::testing::Values(
        TestParams{BatchVacuumSimulatorTest::CLEANINGTEST, BatchVacuumSimulatorTest::LIBPATH, false, {"libAlgo_323012971_315441972_Simultaneous", "libAlgo_323012971_315441972_Orignal"}},
        TestParams{BatchVacuumSimulatorTest::MIXFAILERANDSUCCESHOUSE, BatchVacuumSimulatorTest::LIBPATH, false, {"libAlgo_323012971_315441972_Simultaneous", "libAlgo_323012971_315441972_Orignal"}},
        TestParams{BatchVacuumSimulatorTest::FAILTESTS, BatchVacuumSimulatorTest::LIBPATH, false, {"libAlgo_323012971_315441972_Simultaneous", "libAlgo_323012971_315441972_Orignal"}},
        TestParams{BatchVacuumSimulatorTest::CLEANINGTEST, BatchVacuumSimulatorTest::LIBPATH, true, {"libAlgo_323012971_315441972_Simultaneous", "libAlgo_323012971_315441972_Orignal"}},
        TestParams{BatchVacuumSimulatorTest::CLEANINGTEST, BatchVacuumSimulatorTest::BADLIB, true, {}},
        TestParams{BatchVacuumSimulatorTest::CLEANINGTEST, BatchVacuumSimulatorTest::BADANDGOODLIB, false, {"libAlgo_323012971_315441972_Simultaneous", "libAlgo_323012971_315441972_Orignal"}}
    )
);
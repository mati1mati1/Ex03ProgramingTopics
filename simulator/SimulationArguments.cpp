#include "SimulationArguments.hpp"
#include <boost/program_options.hpp>
#include <iostream>

namespace po = boost::program_options;
namespace fs = std::filesystem;
bool SimulationArguments::isValidDirectory(const std::string& pathStr)
{
    try
    {
        fs::path path(pathStr);
        if (fs::exists(path) && fs::is_directory(path))
        {
            return true;
        }
    }
    catch (const fs::filesystem_error& e)
    {
        std::cerr << "Filesystem error: " << e.what() << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cerr << "General exception: " << e.what() << std::endl;
    }

    return false;
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
SimulationArguments::SimulationArguments(int argc, char** argv)
{
    std::string housePath;
    std::string algoPath;
    uint32_t numThreads;
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "produce help message")
        ("summary_only","create only summary csv file")
        ("house_path", po::value<std::string>(&housePath), "set house files path")
        ("algo_path", po::value<std::string>(&algoPath), "set algorithm files path")
        ("num_threads", po::value<uint32_t>(&numThreads)->default_value(10), "set number of threads");

    po::variables_map vm;
    try
    {
        po::store(po::parse_command_line(argc, argv, desc, po::command_line_style::allow_dash_for_short | po::command_line_style::long_allow_adjacent | po::command_line_style::allow_long_disguise), vm);
        po::notify(vm);
    }
    catch (const po::error &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        throw std::invalid_argument(e.what());
    }
    if (housePath.empty())
    {
        housePath = fs::current_path().string();
    }
    if (algoPath.empty())
    {
        algoPath = fs::current_path().string();
    }
    if (!isValidDirectory(housePath))
    {
        throw std::invalid_argument("Invalid house path: " + housePath);
    }
    if (!isValidDirectory(algoPath))
    {
        throw std::invalid_argument("Invalid algorithm path: " + algoPath);
    }
    if (numThreads == 0)
    {
        throw std::invalid_argument("Number of threads must be greater than 0");
    }
    insertFilesWithExtension(housePath, houseFiles, ".house");
    insertFilesWithExtension(algoPath, algoFiles, ".so");
    this->summaryOnly = vm.count("summary_only");
    this->numThreads = numThreads;
}


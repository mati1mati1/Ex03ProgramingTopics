#pragma once
#include "abstract_algorithm.h"
#include "AlgorithmConfig.hpp"
#include <memory>
#include "MappingAlgorithm/MappingAlgorithm.hpp"
#include "SimultaneousMappingCleaningAlgorithm.hpp"
class AlgorithmFactory {
    public:
        static std::unique_ptr<AbstractAlgorithm> createAlgorithm(const AlgorithmConfig& config) {
            switch (config.getType()) {
                case AlgorithmType::Mapping:
                    return std::make_unique<MappingAlgorithm>();
                case AlgorithmType::Simultaneous:
                    return std::make_unique<SimultaneousMappingCleaningAlgorithm>();
                default:
                    throw std::runtime_error("Invalid algorithm type");
            }
        }
};
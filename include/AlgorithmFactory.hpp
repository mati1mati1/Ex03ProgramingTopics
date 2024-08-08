#pragma once
#include "abstract_algorithm.h"
#include "AlgorithmConfig.hpp"
#include <memory>
#include "MappingAlgorithm/BFSSimultaneousMappingAndCleaningAlgorithm.hpp"
#include "MappingAlgorithm/BFSCleaingAfterMappingAlgorithm.hpp"
class AlgorithmFactory {
    public:
        static std::unique_ptr<AbstractAlgorithm> createAlgorithm(const AlgorithmConfig& config) {
            switch (config.getType()) {
                case AlgorithmType::BFSCleaingAfterMappingAlgorithm:
                    return std::make_unique<BFSCleaingAfterMappingAlgorithm>();
                case AlgorithmType::Simultaneous:
                    return std::make_unique<BFSSimultaneousMappingAndCleaningAlgorithm>();
                default:
                    throw std::runtime_error("Invalid algorithm type");
            }
        }
};
#pragma once
#include "abstract_algorithm.h"
#include "AlgorithmConfig.hpp"
#include <memory>
#include "MappingAlgorithm/MappingAlgorithm.hpp"
class AlgorithmFactory {
    public:
        static std::unique_ptr<AbstractAlgorithm> createAlgorithm(const AlgorithmConfig& config) {
            switch (config.getType()) {
                case AlgorithmType::Mapping:
                    return std::make_unique<MappingAlgorithm>();
                default:
                    throw std::runtime_error("Invalid algorithm type");
            }
        }
};
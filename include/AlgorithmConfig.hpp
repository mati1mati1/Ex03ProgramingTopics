#pragma once
enum class AlgorithmType {
    Mapping
};
class AlgorithmConfig{
    public:
        virtual AlgorithmType getType() const = 0 ;
        virtual ~AlgorithmConfig() = default;
};
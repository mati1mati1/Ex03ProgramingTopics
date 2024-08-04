#pragma once
enum class AlgorithmType {
    Mapping,
    Simultaneous
};
class AlgorithmConfig{
    public:
        virtual AlgorithmType getType() const = 0 ;
        virtual ~AlgorithmConfig() = default;
        virtual std::string getAlgorithmName() const = 0;
};
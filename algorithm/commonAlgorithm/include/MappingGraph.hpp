#pragma once
#include "Coordinate.hpp"
#include "HouseLocationMapping.hpp"
#include <functional>
#include <memory>
#include <optional>
#include <unordered_map>
#include <vector>
#include <set>
typedef uint32_t vertexUID;
class BFSNode;
class MappingGraph;
class BFSResult
{
public:
    BFSResult(uint32_t distance, std::optional<Coordinate<int32_t>> parent) : distance(distance), parent(parent) {};
    [[nodiscard]] uint32_t getDistance() const { return distance; }
    [[nodiscard]] const std::optional<Coordinate<int32_t>> &getParent() const { return parent; }

private:
    uint32_t distance;
    std::optional<Coordinate<int32_t>> parent;
};
class Edge
{
public:
    Edge(vertexUID v, vertexUID w) : v(v), w(w) {};
    vertexUID getStart() const { return v; }
    vertexUID getEnd() const { return w; }

private:
    vertexUID v;
    vertexUID w;
};
class MappingGraphEdge
{
public:
    MappingGraphEdge(Coordinate<int32_t> start, Coordinate<int32_t> end) : start(start), end(end) {};
    [[nodiscard]] Coordinate<int32_t> getStart() const { return start; }
    [[nodiscard]] Coordinate<int32_t> getEnd() const { return end; }

private:
    Coordinate<int32_t> start;
    Coordinate<int32_t> end;
};
class MappingGraph
{
public:
    MappingGraph() {};
    constexpr static vertexUID NO_VERTEX = UINT32_MAX;
    void addEdge(Coordinate<int32_t> v, Direction direction);
    void addVertex(const HouseLocationMapping &location);
    HouseLocationMapping &getVertex(Coordinate<int32_t> location);
    std::vector<MappingGraphEdge> getEdges(Coordinate<int32_t> v) const { return const_cast<MappingGraph *>(this)->iGetEdges(v); }
    bool isVertex(Coordinate<int32_t> location) const;
    const HouseLocationMapping &getVertex(Coordinate<int32_t> location) const { return const_cast<MappingGraph *>(this)->iGetVertex(location); }
    const std::vector<HouseLocationMapping> getMappings() const { return locations; }
    uint32_t size() const { return adj.size(); }
    std::shared_ptr<std::unordered_map<Coordinate<int32_t>, BFSResult>> bfs(Coordinate<int32_t> start) const;
    std::pair<std::shared_ptr<std::unordered_map<Coordinate<int32_t>, BFSResult>>, std::unordered_map<Coordinate<int32_t>, BFSResult>::const_iterator> bfs_find_first(Coordinate<int32_t> startCoordinate, std::function<bool(const Coordinate<int32_t> &, const BFSResult &)> predicate) const;
    ~MappingGraph() = default;

private:
    mutable std::unordered_map<Coordinate<int32_t>, std::shared_ptr<std::unordered_map<Coordinate<int32_t>, BFSResult>>> cache =
        std::unordered_map<Coordinate<int32_t>, std::shared_ptr<std::unordered_map<Coordinate<int32_t>, BFSResult>>>();
    void invalidateCache() const { cache.clear(); }
    std::vector<std::set<vertexUID>> adj = std::vector<std::set<vertexUID>>();
    std::unordered_map<Coordinate<int32_t>, vertexUID> locationToVertex = std::unordered_map<Coordinate<int32_t>, vertexUID>();
    std::vector<HouseLocationMapping> locations = std::vector<HouseLocationMapping>();
    std::unordered_map<Coordinate<int32_t>, BFSResult>::const_iterator bfsInternal(vertexUID start, std::unordered_map<Coordinate<int32_t>, BFSResult> &outMap, std::optional<std::function<bool(const Coordinate<int32_t> &, const BFSResult &)>> predicate) const;
    std::shared_ptr<std::unordered_map<Coordinate<int32_t>, BFSResult>> bfsToResults(const std::vector<BFSNode> &bfsNodes) const;
    HouseLocationMapping &iGetVertex(Coordinate<int32_t> location);
    std::vector<MappingGraphEdge> iGetEdges(Coordinate<int32_t> v);
    std::pair<Coordinate<int32_t>, BFSResult> getBFSResult(const BFSNode &node) const;
};

class BFSNode
{
public:
    enum class Color
    {
        WHITE,
        GRAY,
        BLACK
    };
    void setColor(Color color) { this->color = color; }
    void setDistance(uint32_t distance) { this->distance = distance; }
    void setParent(vertexUID parent) { this->parent = parent; }
    [[nodiscard]] Color getColor() const { return color; }
    [[nodiscard]] vertexUID getVertex() const { return vertex; }
    [[nodiscard]] uint32_t getDistance() const { return distance; }
    [[nodiscard]] vertexUID getParent() const { return parent; }
    BFSNode(vertexUID vertex) : vertex(vertex) {};

private:
    const vertexUID vertex;
    Color color = Color::WHITE;
    uint32_t distance = UINT32_MAX;
    vertexUID parent = MappingGraph::NO_VERTEX;
};

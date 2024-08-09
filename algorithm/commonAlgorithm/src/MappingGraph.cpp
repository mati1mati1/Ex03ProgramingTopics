#include "MappingGraph.hpp"
#include "Coordinate.hpp"
#include "Direction.hpp"
#include <cassert>
#include <memory>
#include <optional>
#include <queue>
#include <sys/types.h>
#include <unordered_map>
#include <utility>
#include <vector>

void MappingGraph::addEdge(Coordinate<int32_t> v, Direction direction)
{
    invalidateCache();
    Coordinate<int32_t> w = v.getDirection(direction);
    vertexUID vertexUidv = locationToVertex.at(v);
    vertexUID vertexUidw = locationToVertex.at(w);
    adj.at(vertexUidv).insert(vertexUidw);
    adj.at(vertexUidw).insert(vertexUidv);
}
HouseLocationMapping &MappingGraph::getVertex(Coordinate<int32_t> location)
{
    invalidateCache();
    return locations.at(locationToVertex.at(location));
}

HouseLocationMapping &MappingGraph::iGetVertex(Coordinate<int32_t> location)
{
    return locations.at(locationToVertex.at(location));
}
void MappingGraph::addVertex(const HouseLocationMapping &location)
{
    invalidateCache();
    uint32_t vertexUid = size();
    locations.push_back(location);
    adj.push_back(std::set<vertexUID>());
    Coordinate<int32_t> locationCoordinate = location.getRelativeToCharger();
    locationToVertex.emplace(locationCoordinate, vertexUid);
}
std::vector<MappingGraphEdge> MappingGraph::iGetEdges(Coordinate<int32_t> v)
{
    std::vector<MappingGraphEdge> edges = std::vector<MappingGraphEdge>();
    for (const vertexUID &target : adj.at(locationToVertex.at(v)))
    {
        edges.push_back(MappingGraphEdge(v, locations[target].getRelativeToCharger()));
    }
    return edges;
}

bool MappingGraph::isVertex(Coordinate<int32_t> location) const
{
    for (auto const &pair : locationToVertex)
    {
        if (pair.first == location)
        {
            return true;
        }
    }
    return false;
}
std::unordered_map<Coordinate<int32_t>, BFSResult>::const_iterator MappingGraph::bfsInternal(vertexUID start, std::unordered_map<Coordinate<int32_t>, BFSResult> &outMap, std::optional<std::function<bool(const Coordinate<int32_t> &, const BFSResult &)>> predicate = std::nullopt) const
{
    std::vector<BFSNode> nodes;
    for (uint32_t i = 0; i < size(); i++)
    {
        nodes.push_back(BFSNode(i));
        if (i == start)
        {
            nodes.at(start).setDistance(0);
            nodes.at(start).setColor(BFSNode::Color::GRAY);
            auto [coordinate, bfsResult] = getBFSResult(nodes.at(start));
            auto result = outMap.emplace(coordinate, bfsResult);
            if (predicate.has_value())
            {
                if (predicate.value()(coordinate, bfsResult))
                {
                    return result.first;
                }
            }
        }
    }
    std::queue<vertexUID> queue;
    queue.push(start);
    while (!queue.empty())
    {
        vertexUID u = queue.front();
        queue.pop();
        BFSNode &unode = nodes.at(u);
        for (const vertexUID &target : adj.at(u))
        {
            BFSNode &wnode = nodes.at(target);

            if (wnode.getColor() == BFSNode::Color::WHITE)
            {
                wnode.setColor(BFSNode::Color::GRAY);
                wnode.setDistance(unode.getDistance() + 1);
                wnode.setParent(u);
                queue.push(target);

                auto [coordinate, bfsResult] = getBFSResult(wnode);
                auto result = outMap.emplace(coordinate, bfsResult);
                if (predicate.has_value())
                {
                    if (predicate.value()(coordinate, bfsResult))
                    {
                        return result.first;
                    }
                }
            }
        }
        unode.setColor(BFSNode::Color::BLACK);
    }
    return outMap.cend();
}
std::pair<Coordinate<int32_t>, BFSResult> MappingGraph::getBFSResult(const BFSNode &node) const
{
    std::optional<Coordinate<int32_t>> parent = std::nullopt;
    if (node.getParent() != NO_VERTEX)
    {
        parent = locations.at(node.getParent()).getRelativeToCharger();
    }
    auto result = BFSResult(
        node.getDistance(),
        parent);
    auto coordinate = locations.at(node.getVertex()).getRelativeToCharger();
    return std::pair<Coordinate<int32_t>, BFSResult>(coordinate, result);
}
std::pair<std::shared_ptr<std::unordered_map<Coordinate<int32_t>, BFSResult>>, std::unordered_map<Coordinate<int32_t>, BFSResult>::const_iterator> MappingGraph::bfs_find_first(Coordinate<int32_t> startCoordinate, std::function<bool(const Coordinate<int32_t> &, const BFSResult &)> predicate) const
{
    vertexUID start = locationToVertex.at(startCoordinate);
    auto bfsResults = std::make_shared<std::unordered_map<Coordinate<int32_t>, BFSResult>>();
    auto iterator = bfsInternal(start, *bfsResults, predicate);
    if (iterator == bfsResults->cend())
    {
        return std::make_pair(bfsResults, iterator);
    }
    return std::make_pair(bfsResults, iterator);
}
std::shared_ptr<std::unordered_map<Coordinate<int32_t>, BFSResult>> MappingGraph::bfs(Coordinate<int32_t> startCoordinate) const
{
    if (cache.find(startCoordinate) != cache.end())
    {
        return cache.at(startCoordinate);
    }
    vertexUID start = locationToVertex.at(startCoordinate);
    auto bfsResults = std::make_shared<std::unordered_map<Coordinate<int32_t>, BFSResult>>();
    bfsInternal(start, *bfsResults);
    assert(bfsResults->size() == size());
    cache.emplace(startCoordinate, bfsResults);
    return bfsResults;
}
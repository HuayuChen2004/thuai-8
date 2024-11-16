#include "path_finding.hpp"

#include <algorithm>
#include <array>
#include <cstdint>
#include <cstdlib>
#include <functional>
#include <queue>
#include <ranges>
#include <span>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "agent/environment_info.hpp"
#include "agent/position.hpp"

constexpr std::uint8_t kNeighborCount{8};
constexpr std::uint16_t kMapSize{100};

namespace {

constexpr auto ManhattanDistance(const thuai8_agent::Position<int>& start,
                                 const thuai8_agent::Position<int>& end)
    -> int {
  return std::abs(start.x - end.x) + std::abs(start.y - end.y);
}

inline auto GetNeighbors(const thuai8_agent::Position<int>& pos,
                         std::span<const thuai8_agent::Wall> walls,
                         std::span<const thuai8_agent::Fence> fences) {
  std::array<thuai8_agent::Position<int>, kNeighborCount> neighbors{
      thuai8_agent::Position<int>{.x = pos.x - 1, .y = pos.y - 1},
      thuai8_agent::Position<int>{.x = pos.x - 1, .y = pos.y},
      thuai8_agent::Position<int>{.x = pos.x - 1, .y = pos.y + 1},
      thuai8_agent::Position<int>{.x = pos.x, .y = pos.y - 1},
      thuai8_agent::Position<int>{.x = pos.x, .y = pos.y + 1},
      thuai8_agent::Position<int>{.x = pos.x + 1, .y = pos.y - 1},
      thuai8_agent::Position<int>{.x = pos.x + 1, .y = pos.y},
      thuai8_agent::Position<int>{.x = pos.x + 1, .y = pos.y + 1}};

  std::vector<thuai8_agent::Position<int>> obstacles;
  for (const auto& wall : walls) {
    if (ManhattanDistance(pos, wall.position) <= 2) {
      obstacles.push_back(wall.position);
    }
  }
  for (const auto& fence : fences) {
    if (ManhattanDistance(pos, fence.position) <= 2) {
      obstacles.push_back(fence.position);
    }
  }

  auto valid_neighbors{
      neighbors |
      std::views::filter(
          [](const thuai8_agent::Position<int>& neighbor) -> bool {
            return neighbor.x >= 0 && neighbor.x < kMapSize &&
                   neighbor.y >= 0 && neighbor.y < kMapSize;
          }) |
      std::views::filter(
          [&obstacles](const thuai8_agent::Position<int>& neighbor) {
            return std::ranges::find(obstacles, neighbor) == obstacles.end();
          })};

  return valid_neighbors;
}

}  // namespace

auto FindPathBFS(const thuai8_agent::Position<int>& start,
                 const thuai8_agent::Position<int>& end,
                 std::span<const thuai8_agent::Wall> walls,
                 std::span<const thuai8_agent::Fence> fences)
    -> std::vector<thuai8_agent::Position<int>> {
  if (std::ranges::contains(walls, end, &thuai8_agent::Wall::position) ||
      std::ranges::contains(fences, end, &thuai8_agent::Fence::position)) {
    return {};
  }

  auto cmp = [&end](const thuai8_agent::Position<int>& lhs,
                    const thuai8_agent::Position<int>& rhs) {
    return ManhattanDistance(lhs, end) < ManhattanDistance(rhs, end);
  };
  auto hash = [](const thuai8_agent::Position<int>& pos) {
    return std::hash<int>{}(pos.x) ^ std::hash<int>{}(pos.y);
  };

  std::priority_queue<thuai8_agent::Position<int>,
                      std::vector<thuai8_agent::Position<int>>, decltype(cmp)>
      frontier(cmp);
  std::unordered_map<thuai8_agent::Position<int>, thuai8_agent::Position<int>,
                     decltype(hash)>
      parents(0, hash);
  std::unordered_set<thuai8_agent::Position<int>, decltype(hash)> visited(0,
                                                                          hash);
  frontier.push(start);
  visited.insert(start);

  bool is_found = false;

  while (!frontier.empty() && !is_found) {
    auto current = frontier.top();
    frontier.pop();

    if (current == end) {
      is_found = true;
      break;
    }

    for (const auto& neighbor : GetNeighbors(current, walls, fences)) {
      if (visited.find(neighbor) != visited.end()) {
        continue;
      }
      frontier.push(neighbor);
      visited.insert(neighbor);
      parents[neighbor] = current;
    }
  }

  if (!is_found) {
    return {};
  }

  std::vector<thuai8_agent::Position<int>> path;
  for (auto current = end; current != start; current = parents[current]) {
    path.push_back(current);
  }
  path.push_back(start);

  return path;
}

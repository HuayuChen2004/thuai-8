#pragma once
#ifndef _THUAI8_ENVIRONMENT_INFO_HPP_
#define _THuAI8_ENVIRONMENT_INFO_HPP_

#include <vector>

#include "agent/position.hpp"

namespace thuai8_agent {

struct Wall {
  Position position{};
};

struct Fence {
  unsigned int health{};
  Position position{};
};

struct Bullet {
  Position position{};
  double speed{};
  double damage{};
  double traveledDistance{};  // 子弹已经过路程
};

struct EnvironmentInfo {
  std::vector<Wall> walls;
  std::vector<Fence> fences;
  std::vector<Bullet> bullets;
};

}  // namespace thuai8_agent

#endif  // _THUAI8_ENVIRONMENT_INFO_HPP_
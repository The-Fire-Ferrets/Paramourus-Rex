#include "AI.h"
#include "LevelView.h"
#include "InputComponent.h" // InputComponent::cardinals
#include "PhysicsComponent.h" // Obstacle detection
#include "CollectorComponent.h" // Player detection

#include <cassert>
#include <vector>

/** Constructor
 ** sets the position of the NPC so it can be remembered for path finding
 **/
AI::AI(sf::Vector2f pos) {
  this->setNPCPosition(pos);
}

/** Sets the position of the NPC so it can be remembered for path finding
 **/
void AI::setNPCPosition(const sf::Vector2f& pos) {
  // sanity check
  assert(pos.x >= 0.f && pos.y >= 0.f);

  npc_pos = pos;
}

/** Returns the position of the NPC used internally for path finding
 **/
sf::Vector2f AI::getNPCPosition(void) const {
  return npc_pos;
}


/** Looks over all the flowers in the current level, and returns the one closest 
 ** the NPC
 **/
sf::Vector2f AI::findClosestFlower(void) const {
  // find all flowers
  std::vector<sf::Vector2f> flower_locations;
  for (auto it = LevelView::actorList.begin(); it != LevelView::actorList.end(); ++it) {
    // is this actor a flower?
    if ((*it)->hasComponent(CollectableComponent::id)) {
      std::shared_ptr<ActorComponent> ac;
      std::shared_ptr<CollectableComponent> cc;

      ac = (*it)->components[CollectableComponent::id];
      cc = std::dynamic_pointer_cast<CollectableComponent>(ac);

      // and one that hasn't been picked up?
      if (!cc->collector) {
        flower_locations.push_back((*it)->getPosition());
      }
    }
  }

  // no flowers found
  if (flower_locations.empty())
    return sf::Vector2f(-1.f, -1.f);

  // determine which flower is closest to NPC
  sf::Vector2f closest = flower_locations.front();
  for (sf::Vector2f flower_pos : flower_locations) {
    if (distance(flower_pos, npc_pos) < distance(closest, npc_pos)) {
      closest = flower_pos;
    }
  }

  return closest;
}

sf::Vector2f AI::findPlayer(void) const {
  return LevelView::player->getPosition();
}

/** Determines which direction to move it to head toward the destination dest
 **/
sf::Vector2f AI::chooseDirection(const sf::Vector2f dest) const {
  sf::Vector2f gap = npc_pos - dest;
  sf::Vector2f direction;
  bool obstacle = this->isObstacleInPath(dest);

  // move in the direction that closest the longer distance, *unless* there is 
  // an obstacle
  // TODO: is there a better way to organize this?  only way i can see obstacles 
  // being an issue is if abs(gap.x) == abs(gap.y) (ie, npc and obstacle are 
  // opposite corners on a square
  if (std::abs(gap.x) > std::abs(gap.y)) {
    if (gap.x < FLT_EPSILON && !obstacle) {
      direction = EAST;
    }
    else if (gap.x < FLT_EPSILON) {
      if (gap.y < FLT_EPSILON) {
        direction = SOUTH;
      }
      else direction = NORTH;
    }
    else {
      direction = WEST;
    }
  }
  else {
    if (gap.y < FLT_EPSILON && !obstacle) {
      direction = SOUTH;
    }
    else if (gap.y < FLT_EPSILON) {
      if (gap.x < FLT_EPSILON) {
        direction = EAST;
      }
      else direction = WEST;
    }
    else {
      direction = NORTH;
    }
  }

  return direction;
}

/** Determines whether an obastacle exists along the path to the destination 
 ** dest.
**/
bool AI::isObstacleInPath(const sf::Vector2f& dest) const {
  // find all obstacles
  std::vector<sf::Vector2f> obstacle_locations;
  for (auto it = LevelView::actorList.begin(); it != LevelView::actorList.end(); ++it) {
    // is interaction with this actor possible
    if ((*it)->hasComponent(PhysicsComponent::id)) {
      std::shared_ptr<ActorComponent> ac;
      std::shared_ptr<PhysicsComponent> pc;

      ac = (*it)->components[PhysicsComponent::id];
      pc = std::dynamic_pointer_cast<PhysicsComponent>(ac);

      // is it an obstacle?
      if (pc->getType() == "Opaque") {
        obstacle_locations.push_back((*it)->getPosition());
        obstacle_locations.push_back((*it)->getPosition()+(*it)->getSize()); // hacky, but whatever
      }
    }
  }

  // no flowers found
  if (obstacle_locations.empty())
    return false;

  // for each obstacle
  for (auto it = obstacle_locations.begin(); it != obstacle_locations.end(); ++it) {
    // get slope between npc and obstacle AND npc and destination
    float m1, m2;
    bool m1defined, m2defined;

    m1defined = slope((*it), npc_pos, m1);
    m2defined = slope(dest, npc_pos, m2);

    // if slopes are the same, just see if if the coordinate of the obstacle 
    // falls in the right range
    if ((!m1defined && !m2defined) || std::abs(m1-m2) < 2.5f) {
      return (npc_pos.x <= (*it).x && (*it).x <= dest.x && npc_pos.y <= (*it).y && (*it).y <= dest.y)
        || (dest.x <= (*it).x && (*it).x <= npc_pos.x && dest.y <= (*it).y && (*it).y <= npc_pos.y);
    }
  }

  return false;
}

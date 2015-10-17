#include "AI.h"
#include "LevelView.h"
#include "InputComponent.h" // InputComponent::cardinals
#include "PhysicsComponent.h" // Obstacle detection
#include "CollectorComponent.h" // Player detection

#include <cassert> //  assert()
#include <algorithm> // std::find()

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
	//assert(pos.x >= 0.f && pos.y >= 0.f);
	
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
	sf::Vector2f distance = dest - npc_pos;;
	sf::Vector2f direction;
	std::vector<sf::Vector2f> excluded_dirs;

	// try each direction heuristically such that we can move there without
	// hitting something
	do {
		direction = this->selectNextDirection(distance, excluded_dirs);
		excluded_dirs.push_back(direction);
	} while (this->hasCollisionOnPath(direction) && excluded_dirs.size() <= 3);

	return direction;
}

/** Determines whether an obastacle exists along the path to the destination
 ** dest.
 **/
bool AI::hasCollisionOnPath(const sf::Vector2f& dir) const {
	// find all obstacles
	sf::Vector2f new_pos = npc_pos + dir;
	for (auto it = LevelView::actorList.begin(); it != LevelView::actorList.end(); ++it) {
		// is interaction with this actor possible
		if ((*it)->hasComponent(PhysicsComponent::id)) {
			std::shared_ptr<ActorComponent> ac;
			std::shared_ptr<PhysicsComponent> pc;

			ac = (*it)->components[PhysicsComponent::id];
			pc = std::dynamic_pointer_cast<PhysicsComponent>(ac);

			// is it an obstacle?
			if (pc->getType() == "Opaque") {
				sf::Vector2f obstacle_pos = (*it)->getPosition();

				// if moving in this direction would place us closer to the
				// obstacle, it's probably a bad idea
				if (distance(npc_pos, obstacle_pos) < 20.f && distance(new_pos, obstacle_pos) < distance(npc_pos, obstacle_pos)) {
					return true;
				}
			}
		}
	}

	// no collisions with any obstacle
	return false;
}

/** Chooses a new direction based upon the distance between the npc and the
 ** destination, potentially excluding certain direction.
 **/
sf::Vector2f AI::selectNextDirection(const sf::Vector2f& distance, const std::vector<sf::Vector2f>& excluded) const {
	std::cerr << distance.x << " " << distance.y << " " << excluded.size() << std::endl;
	// do we have further to travel in the x direction?
	if (std::abs(distance.x) > std::abs(distance.y)) {
		// going east?
		if (distance.x > FLT_EPSILON) {
			// try moving east
			if (std::find(excluded.begin(), excluded.end(), EAST) == excluded.end()) {
				return EAST;
			}

			// going south?
			else if (distance.y > FLT_EPSILON) {
				if (std::find(excluded.begin(), excluded.end(), SOUTH) == excluded.end()) {
					return SOUTH;
				}
				else if (std::find(excluded.begin(), excluded.end(), NORTH) == excluded.end()) {
					return NORTH;
				}
				else return WEST;
			}

			// going north
			else {
				if (std::find(excluded.begin(), excluded.end(), NORTH) == excluded.end()) {
					return NORTH;
				}
				else if (std::find(excluded.begin(), excluded.end(), SOUTH) == excluded.end()) {
					return SOUTH;
				}
				else return WEST;
			}

		}

		// going west
		else {
			// try moving west
			if (std::find(excluded.begin(), excluded.end(), WEST) == excluded.end()) {
				return WEST;
			}

			// going south?
			else if (distance.y > FLT_EPSILON) {
				if (std::find(excluded.begin(), excluded.end(), SOUTH) == excluded.end()) {
					return SOUTH;
				}
				else if (std::find(excluded.begin(), excluded.end(), NORTH) == excluded.end()) {
					return NORTH;
				}
				else return EAST;
			}

			// going north
			else {
				if (std::find(excluded.begin(), excluded.end(), NORTH) == excluded.end()) {
					return NORTH;
				}
				else if (std::find(excluded.begin(), excluded.end(), SOUTH) == excluded.end()) {
					return SOUTH;
				}
				else return EAST;
			}
		}
	}
	// we have further to travel in the y direction
	else {
		// going south?
		if (distance.y > FLT_EPSILON) {
			// try moving east
			if (std::find(excluded.begin(), excluded.end(), SOUTH) == excluded.end()) {
				return SOUTH;
			}

			// going east?
			else if (distance.x > FLT_EPSILON) {
				if (std::find(excluded.begin(), excluded.end(), EAST) == excluded.end()) {
					return EAST;
				}
				else if (std::find(excluded.begin(), excluded.end(), WEST) == excluded.end()) {
					return WEST;
				}
				else return NORTH;
			}

			// going west
			else {
				if (std::find(excluded.begin(), excluded.end(), WEST) == excluded.end()) {
					return WEST;
				}
				else if (std::find(excluded.begin(), excluded.end(), EAST) == excluded.end()) {
					return EAST;
				}
				else return NORTH;
			}
		}

		// going north
		else {
			// try moving north
			if (std::find(excluded.begin(), excluded.end(), NORTH) == excluded.end()) {
				return NORTH;
			}

			// going east?
			else if (distance.x > FLT_EPSILON) {
				if (std::find(excluded.begin(), excluded.end(), EAST) == excluded.end()) {
					return EAST;
				}
				else if (std::find(excluded.begin(), excluded.end(), WEST) == excluded.end()) {
					return WEST;
				}
				else return SOUTH;
			}

			// going west
			else {
				if (std::find(excluded.begin(), excluded.end(), WEST) == excluded.end()) {
					return WEST;
				}
				else if (std::find(excluded.begin(), excluded.end(), EAST) == excluded.end()) {
					return EAST;
				}
				else return SOUTH;
			}
		}
	}
}

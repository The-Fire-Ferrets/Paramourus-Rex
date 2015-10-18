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

AI::AI() {
	direction_bit = 0;
}

AI::AI(sf::Vector2f pos) {
	direction_bit = 0;
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
sf::Vector2f AI::getNPCPosition(void) {
	return npc_pos;
}


/** Looks over all the flowers in the current level, and returns the one closest
 ** the NPC
 **/
sf::Vector2f AI::findClosestFlower(void) {
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

sf::Vector2f AI::findPlayer(void) {
	return LevelView::player->getPosition();
}

/** Determines which direction to move it to head toward the destination dest
 **/
sf::Vector2f AI::chooseDirection(const sf::Vector2f dest) {
	sf::Vector2f distance = dest - npc_pos;

	// Updates collision and changes direction to sides without collision
	updateCollision();
	return selectNextDirection(distance);
}

/** Determines whether the obstacles in previous collision are still in contact
 ** dest.
 **/
void AI::updateCollision() {
	std::shared_ptr<ActorComponent> ac;
	std::shared_ptr<PhysicsComponent> pc;

	ac = owner->components[PhysicsComponent::id];
	pc = std::dynamic_pointer_cast<PhysicsComponent>(ac);
	int hit[4] = {0,0,0,0};
	//Checks to see if actors from previosu collision still exist and on which side
	for (auto it = pc->last_actors.begin(); it != pc->last_actors.end(); ++it) {
		if (*it == actor_direction[0])
			hit[0] = 1;
		else if (*it == actor_direction[1])
			hit[1] = 1;
		else if (*it == actor_direction[2])
			hit[2] = 1;
		else if (*it == actor_direction[3])
			hit[3] = 1;
		
	}

	//Flips the bit for that side to allow movement if no longer colliding
	for (int i = 0; i < 4; i++) {
		if (!hit[i]) {
			flipDirectionBit(i+1);
			actor_direction[i] = NULL;
		}
	}
}

/** Chooses a new direction based upon the distance between the npc and the
 ** destination, potentially excluding certain directions.
 **/
sf::Vector2f AI::selectNextDirection(const sf::Vector2f& distance) {
	
	if (std::abs(distance.x) > std::abs(distance.y)) {
		if (distance.x > FLT_EPSILON && !getDirectionBit(1)) {
		    return EAST;
		}
		else if (distance.x < FLT_EPSILON && !getDirectionBit(2)){
		    return WEST;
		}
		else if (distance.y > FLT_EPSILON && !getDirectionBit(4)) {
		        return SOUTH;
		}
		else if (distance.y < FLT_EPSILON && !getDirectionBit(3)) {
			return NORTH;
		}
	    }
	    else {
		if (distance.y > FLT_EPSILON && !getDirectionBit(4)) {
		    return SOUTH;
		}
		else if (distance.y < FLT_EPSILON && !getDirectionBit(3)){
		    return NORTH;
		}
		else if (distance.x > FLT_EPSILON && !getDirectionBit(1)) {
		        return EAST;
		}
		else if (distance.x < FLT_EPSILON && !getDirectionBit(2)) {
			return WEST;
		}
	    }
	return sf::Vector2f(-1.f,-1.f);
}

/** Sets the value of the given bit
 **
**/
void AI::setDirectionBit(int bit_num) {
	direction_bit = direction_bit | 1<<(bit_num - 1);
}

/** Returns the value of the given bit
 **
**/
bool AI::getDirectionBit(int bit_num) {
	return direction_bit & 1<<(bit_num - 1);
}

/** Flips the given bit
 **
**/
void AI::flipDirectionBit(int bit_num) {
	direction_bit = direction_bit ^ 1<<(bit_num - 1);
}

/** Resets all directions to 0
 **
**/
void AI::resetDirectionBit() {
	direction_bit = 0;
}

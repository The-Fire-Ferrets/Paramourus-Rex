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
	dist_from_start = 0;
}

AI::AI(sf::Vector2f pos) {
	direction_bit = 0;
	dist_from_start = 0;
	this->setNPCPosition(pos);
}

/** Sets the position of the NPC so it can be remembered for path finding
 **/
void AI::setNPCPosition(const sf::Vector2f& pos) {	
	npc_pos = pos;
}

/** Returns the position of the NPC used internally for path finding
 **/
sf::Vector2f AI::getNPCPosition(void) {
	return npc_pos;
}

sf::Vector2f AI::chooseDirection(float dist, std::vector<float>* dists, std::vector<sf::Vector2f>* dirs) {
	for (int i = 0; i < dist / 10; i ++) {
		dists->push_back(1);
		float f = 1;
		sf::Vector2f dir = chooseDirection(&f);
		dirs->push_back(dir);
		npc_pos.x += npc_pos.x * dir.x;
		npc_pos.y += npc_pos.y * dir.y; 
	}
}

void AI::generatePath(void) {
	Pathfinder::generatePath(npc_pos);
}


sf::Vector2f AI::getNextPosition() {
	return Pathfinder::getNextPosition();
}
/** Determines which direction to move it to head toward the destination dest
 **/
sf::Vector2f AI::chooseDirection(float* dist) {
	sf::Vector2f min_dir;
	int min;
	
	sf::Vector2f check_dir;
	int check_min;

	min_dir = sf::Vector2f(1,1);
	min = Pathfinder::getCost(npc_pos, min_dir);
	
	check_dir = sf::Vector2f(-1, -1);
	check_min = Pathfinder::getCost(npc_pos, check_dir);
	if (check_min < min) {
		min = check_min;
		min_dir = check_dir;
	}
	
	check_dir = sf::Vector2f(-1, 1);
	check_min = Pathfinder::getCost(npc_pos, check_dir);
	if (check_min < min) {
		min = check_min;
		min_dir = check_dir;
	}

	check_dir = sf::Vector2f(1, -1);
	check_min = Pathfinder::getCost(npc_pos, check_dir);
	if (check_min < min) {
		min = check_min;
		min_dir = check_dir;
	}

	check_dir = sf::Vector2f(1, 0);
	check_min = Pathfinder::getCost(npc_pos, check_dir);
	if (check_min < min) {
		min = check_min;
		min_dir = check_dir;
	}

	check_dir = sf::Vector2f(-1, 0);
	check_min = Pathfinder::getCost(npc_pos, check_dir);
	if (check_min < min) {
		min = check_min;
		min_dir = check_dir;
	}

	check_dir = sf::Vector2f(0, 1);
	check_min = Pathfinder::getCost(npc_pos, check_dir);
	if (check_min < min) {
		min = check_min;
		min_dir = check_dir;
	}

	check_dir = sf::Vector2f(0, -1);
	check_min = Pathfinder::getCost(npc_pos, check_dir);
	if (check_min < min) {
		min = check_min;
		min_dir = check_dir;
	}

	Pathfinder::addToCost(npc_pos, min_dir, ++dist_from_start);
	*dist = Pathfinder::getDistance(npc_pos, min_dir);
		
	//std::cout << min_dir.x << " " << min_dir.y << std::endl;
	return min_dir;
}

/** Chooses a new direction based upon the distance between the npc and the
 ** destination, potentially excluding certain directions.
 **/
sf::Vector2f AI::selectNextDirection(const sf::Vector2f& distance) {
	int threshold = 0;
	if (std::abs(distance.x) + threshold > std::abs(distance.y)) {
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
	    else if (std::abs(distance.x) + threshold < std::abs(distance.y)) {
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
	else  {
		if (distance.y >= FLT_EPSILON && distance.x >= FLT_EPSILON) {
		    return SOUTHEAST;
		}
		else if (distance.y <= FLT_EPSILON && distance.x <= FLT_EPSILON){
		    return NORTHWEST;
		}
		else if (distance.x >= FLT_EPSILON && distance.y <= FLT_EPSILON) {
		        return NORTHEAST;
		}
		else if (distance.x <= FLT_EPSILON && distance.y >= FLT_EPSILON) {
			return SOUTHWEST;
		}
	    }
	return sf::Vector2f(0,0);
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

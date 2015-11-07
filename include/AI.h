#ifndef AI_H
#define AI_H

#include "Constants.h"
#include <SFML/System/Vector2.hpp>
#include <vector>
#include "Utilities.h"

class AI {
	public:
		AI();
		AI(sf::Vector2f pos);

		void setNPCPosition(const sf::Vector2f& pos);
		sf::Vector2f getNPCPosition(void);

		void setDirectionBit(int bit_num);
		bool getDirectionBit(int bit_num);
		void flipDirectionBit(int bit_num);
		void resetDirectionBit();

		sf::Vector2f chooseDirection(float* dist);
		sf::Vector2f chooseDirection(float dist, std::vector<float>* dists, std::vector<sf::Vector2f>* dirs);
		StrongActorPtr actor_direction[4];
		StrongActorPtr owner;
		void generatePath(void);
		sf::Vector2f getNextPosition();	
			
	private:
		sf::Vector2f npc_pos;
		int direction_bit;
		int dist_from_start;
		sf::Vector2f selectNextDirection(const sf::Vector2f& distance);
};

#endif/*AI_H*/

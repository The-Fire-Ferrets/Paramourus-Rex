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

		sf::Vector2f findClosestFlower(void);
		sf::Vector2f findPlayer(void) ;
		sf::Vector2f chooseDirection(const sf::Vector2f dest);
		void updateCollision();
		StrongActorPtr actor_direction[4];
		StrongActorPtr owner;
	private:
		sf::Vector2f npc_pos;
		int direction_bit;
		sf::Vector2f selectNextDirection(const sf::Vector2f& distance);
};

#endif/*AI_H*/

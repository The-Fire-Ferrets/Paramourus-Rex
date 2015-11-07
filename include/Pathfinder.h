#ifndef PATHFINDER_H
#define PATHFINDER_H

#include "Constants.h"
class Pathfinder { 
	friend class CraftView;

	struct pathNode {
		pathNode* parent;
		std::pair<int, int> pos;
		int cost;
		pathNode* next;
		pathNode (std::pair<int, int> p, int c) {
			parent = NULL;
			next = NULL;
			pos = p;
			cost = c;
		}
		pathNode (std::pair<int, int> p, int c, pathNode* p_node) {
			parent = p_node;
			next = NULL;
			pos = p;
			cost = c;
		}
		std::vector<pathNode*> neighbors(void) {
			std::vector<pathNode*> n;
			std::pair<int, int> next_loc;
			next_loc = std::pair<int, int>(pos.first + 1, pos.second);
			if (isValidMove(next_loc))
				n.push_back(new pathNode(next_loc, cost));

			next_loc = std::pair<int, int>(pos.first - 1, pos.second);
			if (isValidMove(next_loc))
				n.push_back(new pathNode(next_loc, cost));

			next_loc = std::pair<int, int>(pos.first, pos.second + 1);
			if (isValidMove(next_loc))
				n.push_back(new pathNode(next_loc, cost));

			next_loc = std::pair<int, int>(pos.first, pos.second - 1);
			if (isValidMove(next_loc))
				n.push_back(new pathNode(next_loc, cost));

			next_loc = std::pair<int, int>(pos.first + 1, pos.second + 1);
			if (isValidMove(next_loc))
				n.push_back(new pathNode(next_loc, cost));

			next_loc = std::pair<int, int>(pos.first - 1, pos.second + 1);
			if (isValidMove(next_loc))
				n.push_back(new pathNode(next_loc, cost));

			next_loc = std::pair<int, int>(pos.first - 1, pos.second - 1);
			if (isValidMove(next_loc))
				n.push_back(new pathNode(next_loc, cost));

			next_loc = std::pair<int, int>(pos.first + 1, pos.second - 1);
			if (isValidMove(next_loc))
				n.push_back(new pathNode(next_loc, cost));
			return n;
		}
		bool containedIn(std::vector<pathNode*> l) {
			std::vector<pathNode*>::iterator itr;
			for (itr = l.begin(); itr != l.end(); itr++) {
				if ((*itr)->pos == pos)
					return true;
			}
			return false;
		}
		bool containedIn(std::vector<pathNode*>* l) {
			std::vector<pathNode*>::iterator itr;
			for (itr = l->begin(); itr != l->end(); itr++) {
				if ((*itr)->pos == pos)
					return true;
			}
			return false;
		}
		pathNode* getRoot() {
			while (parent != NULL) {
				parent->next = this;
				return parent->getRoot();
			}
			return this;
		}
		float distance(pathNode* other) {
			return sqrt(pow(other->pos.first - pos.first, 2.0) + pow(other->pos.second - pos.second, 2.0));
		}
	};
	private:
		static int** grid;
		static int rows;
		static int cols;
		static int level_height;
		static int level_width;
		static int player_size;
		static std::vector<sf::Vector2i> flowers;
		static std::vector<sf::Vector2i> start_positions;
		static void updateVertex(std::vector<pathNode*>* open, std::vector<pathNode*>* closed, pathNode* p_node, pathNode* c_node);
		static void getFullCost(pathNode* p_node, pathNode* c_node);
		static void removeFrom(std::vector<pathNode*>* l, pathNode* n);
		static std::map<std::pair<int, int>, Pathfinder::pathNode*> paths;
		static bool lineOfSight(pathNode* p_node, pathNode* c_node);

	public:
		static void Create(int lw, int lh, int ps);
		static void addToGrid(std::vector<sf::FloatRect*> bounds, int type);
		static void print(void);
		static void generateHCost(void);
		static void generateHCost_helper(std::vector<sf::Vector2i> left);
		static int getCost(sf::Vector2f pos, sf::Vector2f dir);
		static int getCost(sf::Vector2f pos);
		static int getCost(std::pair<int, int> loc);
		static int addToCost(sf::Vector2f pos, sf::Vector2f dir, int incr);
		static sf::Vector2i getPositionMapping(sf::Vector2f pos); 
		static float getDistance(sf::Vector2f pos,  sf::Vector2f dir);
		static void generatePaths(void);
		static bool isValidMove(std::pair<int, int> loc);
		static sf::Vector2f getNextPosition(sf::Vector2f start_pos);
};

#endif

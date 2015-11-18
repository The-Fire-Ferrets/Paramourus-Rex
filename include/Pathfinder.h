#ifndef PATHFINDER_H
#define PATHFINDER_H

#include "Constants.h"

typedef std::pair<int, int> GridLocation;
typedef std::vector<sf::Vector2f> PathList;
typedef int** Grid;

class Pathfinder { 
	friend class CraftView;
	public:
		struct pathNode {
			pathNode* parent;
			GridLocation pos;
			int cost;
			pathNode (GridLocation p, int c) {
				parent = NULL;
				pos = p;
				cost = c;
			}
			pathNode (GridLocation p, int c, pathNode* p_node) {
				parent = p_node;
				pos = p;
				cost = c;
			}
			std::vector<pathNode*> neighbors(Grid* target_grid) {
				std::vector<pathNode*> n;
				GridLocation next_loc;
				next_loc = GridLocation(pos.first + 1, pos.second);
				if (isValidMove(next_loc, target_grid))
					n.push_back(new pathNode(next_loc, cost + 1));

				next_loc = GridLocation(pos.first - 1, pos.second);
				if (isValidMove(next_loc, target_grid))
					n.push_back(new pathNode(next_loc, cost + 1));

				next_loc = GridLocation(pos.first, pos.second + 1);
				if (isValidMove(next_loc, target_grid))
					n.push_back(new pathNode(next_loc, cost + 1));

				next_loc = GridLocation(pos.first, pos.second - 1);
				if (isValidMove(next_loc, target_grid))
					n.push_back(new pathNode(next_loc, cost + 1));

				next_loc = GridLocation(pos.first + 1, pos.second + 1);
				if (isValidMove(next_loc, target_grid))
					n.push_back(new pathNode(next_loc, cost + 1));

				next_loc = GridLocation(pos.first - 1, pos.second + 1);
				if (isValidMove(next_loc, target_grid))
					n.push_back(new pathNode(next_loc, cost + 1));

				next_loc = GridLocation(pos.first - 1, pos.second - 1);
				if (isValidMove(next_loc, target_grid))
					n.push_back(new pathNode(next_loc, cost + 1));

				next_loc = GridLocation(pos.first + 1, pos.second - 1);
				if (isValidMove(next_loc, target_grid))
					n.push_back(new pathNode(next_loc, cost + 1));
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
			PathList getPath() {
				PathList final_path;
				pathNode* temp = this;
				while ( temp != NULL ) {
					final_path.push_back(sf::Vector2f(temp->pos.second * player_size, temp->pos.first * player_size));
					temp = temp->parent;
				}
				return final_path;
			}
			float distance(pathNode* other) {
				return sqrt(pow(other->pos.first - pos.first, 2.0) + pow(other->pos.second - pos.second, 2.0));
			}
			float distance(GridLocation other_pos) {
				return sqrt(pow(other_pos.first - pos.first, 2.0) + pow(other_pos.second - pos.second, 2.0));
			}
		};
		
	private:
		static const int size = 50;
		static int target_num;
		static int start_num;
		static std::pair<GridLocation, std::pair<GridLocation, int>>* targets;
		static std::pair<GridLocation, std::pair<GridLocation, int>>* start_positions;
		//static std::vector<std::pair<GridLocation, std::pair<GridLocation, int>>> targets;
		//static std::vector<std::pair<GridLocation, std::pair<GridLocation, int>>> start_positions;
		
		static std::map<GridLocation*, GridLocation> initial_positions;
		static std::map<GridLocation*, Grid> target_grids;
		static Grid grid;
		static int rows;
		static int cols;
		static int level_height;
		static int level_width;
		static int player_size;
		static std::map<GridLocation*, bool> target_taken;
		static std::map<GridLocation*, int> target_values;
		static std::map<GridLocation*, int> start_targets;
		static void updateVertex(std::vector<pathNode*>* open, std::vector<pathNode*>* closed, pathNode* p_node, pathNode* c_node, Grid* target_grid);
		static void getFullCost(pathNode* p_node, pathNode* c_node, Grid* target_grid);
		static void removeFrom(std::vector<pathNode*>* l, pathNode* n);
		static std::map<GridLocation*, std::pair<GridLocation*, PathList*>> paths;
		static std::map<std::pair<GridLocation*, GridLocation*>, PathList> allPaths;
		static bool lineOfSight(pathNode* p_node, pathNode* c_node, Grid* target_grid);
		static void generateHCost_helper(std::vector<GridLocation> left, Grid* target_grid);
		static std::vector<EventDelegate> delegateFuncList;
		static void generateHCost(GridLocation* target_pos, GridLocation curr_pos);
		static void setVertex(std::vector<pathNode*>* closed, pathNode* p_node, Grid* target_grid);
		static GridLocation* findTarget(GridLocation pos);
		static GridLocation* findStart(GridLocation init, GridLocation pos);
		static bool isValidTarget(GridLocation* ptr);
		static bool selectNewPath(GridLocation init_pair, GridLocation* start_pt, GridLocation curr_pair);
		static bool positionsEqual(GridLocation* pos1, GridLocation* pos2);
		static std::map<GridLocation*, bool> inVision;

	public:
		static void Create(int lw, int lh, int ps);
		static void addToGrid(std::vector<sf::FloatRect*> bounds, int path_type, int target_type);
		static void generatePath(GridLocation init, GridLocation start, GridLocation target, GridLocation new_start, bool change_path = false);
		static void generateHCosts(void);
		static void print(void);
		static void print(Grid* target_grid);
		static int getCost(sf::Vector2f pos, sf::Vector2f dir, Grid* target_grid);
		static int getCost(sf::Vector2f pos, Grid* target_grid);
		static int getCost(GridLocation loc, Grid* target_grid);
		static int addToCost(sf::Vector2f pos, sf::Vector2f dir, int incr, Grid* target_grid);
		static GridLocation getPositionMapping(sf::Vector2f pos, bool non_wall = false); 
		static float getDistance(sf::Vector2f pos,  sf::Vector2f pos2);
		static void generatePathsFromStart(void);
		static void generatePathsToTarget(void);
		static bool isValidMove(GridLocation loc, Grid* target_grid);
		static bool isValidPlacement(GridLocation loc, Grid* target_grid);
		static bool getNextPosition(float dist, sf::Vector2f init_pos, sf::Vector2f start_pos, sf::Vector2f curr_pos, sf::Vector2f* next_pos, sf::Vector2f* direction);
		static void removeFromPaths(sf::Vector2f pos);
		static void generatePath2(sf::Vector2f init_pos, sf::Vector2f start_pos, sf::Vector2f curr_pos);
		static std::mutex paths_mutex;
		static std::mutex targets_mutex;
		static std::map<GridLocation*, bool> inProcessTargets;
		static std::map<std::pair<GridLocation*, GridLocation*>, bool> inProcessPaths;
		static std::map<std::pair<GridLocation*, GridLocation*>, bool> pathUpdates;
		static void updateTargetGrid(sf::Vector2f start_pos, sf::Vector2f curr_pos);
		static bool canUpdateTargetGrid(sf::Vector2f start_pos);
		static bool canUpdateStartPath(sf::Vector2f init_pos, sf::Vector2f start_pos);
		static bool generatingPaths;
		static void changeVision(sf::Vector2f init_pos);
		static void removePositionFromPath(sf::Vector2f init_pos);
};


namespace std {
	template <> 
	struct hash<Pathfinder::pathNode> {
		std::size_t operator()(const Pathfinder::pathNode &t) const {
			return (51 + std::hash<int>()(t.pos.first)) * 51 + std::hash<int>()(t.pos.second);
		}
	};
}
#endif

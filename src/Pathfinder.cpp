#include "Pathfinder.h"

//Blank grid representation of level
Grid Pathfinder::grid;
//Grid representation of level
std::map<GridLocation*, Grid> Pathfinder::target_grids;
//Grids for each map
//Number of rows
int Pathfinder::rows;
//Number of columns
int Pathfinder::cols;
//Level Height, width and player size
int Pathfinder::level_height;
int Pathfinder::level_width;
int Pathfinder::player_size;
//target locs
std::vector<std::pair<GridLocation, int>> Pathfinder::targets;
//start Position list
std::vector<GridLocation> Pathfinder::start_positions;
//Paths
std::map<GridLocation*, std::pair<GridLocation*, PathList*>> Pathfinder::paths;
std::map<std::pair<GridLocation*, GridLocation*>, PathList> Pathfinder::allPaths;
//Event delegate
std::vector<EventDelegate> Pathfinder::delegateFuncList;
//Mutex
std::mutex Pathfinder::paths_mutex;
std::map<GridLocation*, bool> Pathfinder::inProcessTargets;
std::map<std::pair<GridLocation*, GridLocation*>, bool> Pathfinder::inProcessPaths;
std::map<std::pair<GridLocation*, GridLocation*>, bool> Pathfinder::pathUpdates;
//Target Values
std::map<GridLocation*, int> Pathfinder::target_values;
std::mutex Pathfinder::targets_mutex;
int Pathfinder::first_run = 0;
/** Constructs the grid and resets the lists
 **
**/
void Pathfinder::Create(int lw, int lh, int ps) {
	player_size = ps;
	level_height = lh;
	level_width = lw;
	rows = level_height / player_size;
	cols = level_width / player_size;
	grid = NULL;
	grid = new int*[rows];

	for (int i = 0; i < rows; i++) {
		grid[i] = new int[cols];
		for (int j = 0; j < cols; j++)
			grid[i][j] = 0;
	}
	paths_mutex.lock();
	////targets_mutex.lock();
	paths.clear();
	start_positions.clear();
	targets.clear();
	inProcessTargets.clear();
	inProcessPaths.clear();
	pathUpdates.clear();
	allPaths.clear();
	target_grids.clear();
	target_values.clear();
	////targets_mutex.lock();
	paths_mutex.unlock();
}


/** Mapts the given boundary to the grid
 **
**/
void Pathfinder::addToGrid(std::vector<sf::FloatRect*> bounds, int type, StrongActorPtr actor_ptr) {
	std::vector<sf::FloatRect*>::iterator itr;
	for (itr = bounds.begin(); itr != bounds.end(); itr++) {
		int top_left_row = (*itr)->top  / player_size;
		int top_left_col = (*itr)->left / player_size;
		int bottom_right_row = ((*itr)->top + (*itr)->height)  / player_size;
		int bottom_right_col = ((*itr)->left + (*itr)->width) / player_size;

		for (int i = top_left_row; i < bottom_right_row; i++)
			for (int j = top_left_col; j < bottom_right_col; j++) {
				GridLocation loc(i, j);
				paths_mutex.lock();				
				if (type == -2 || type == -4) {
					//targets_mutex.lock();
					targets.push_back(std::pair<GridLocation, int>(GridLocation(i,j), type));
					//GridLocation* pos = &(*targets.end());
					//std::cout << "Target " << type << targets.back().second << std::endl;
					//targets_mutex.unlock();
					//std::cout << "Target " << j * player_size << " " << i * player_size << " " << type << std::endl;
					//target_actors_push_back.insert(std::pair(loc, actor_ptr));
				}
				else if (type == -3) {
					start_positions.push_back(GridLocation(i,j));
					//std::cout << "Start " << j * player_size << " " << i * player_size << " " << type << std::endl;
				}
				else
					grid[i][j] = type;
				paths_mutex.unlock();
			}
	}
			
}


/** Generates paths from all start positions to the target
 **
**/
void Pathfinder::generatePaths() {
	for (auto itr_targets = targets.begin(); itr_targets != targets.end(); ++itr_targets) {
		GridLocation target_pair = (itr_targets->first);
		GridLocation* target_ptr = &(itr_targets->first);
		Grid* target_grid = &(target_grids[target_ptr]);
		target_values.insert(std::pair<GridLocation*, int>(target_ptr, itr_targets->second));
		inProcessTargets.insert(std::pair<GridLocation*, bool>(target_ptr, false));
		std::vector<GridLocation>::iterator itr;
		for (itr = start_positions.begin(); itr != start_positions.end(); ++itr) {
			GridLocation pos_pair = *itr;
			//std::cout << "Start: " << pos_pair.first << " " << pos_pair.second << " Target: " << target_pair.first << " " << target_pair.second << std::endl;
			//print(target_grid);			
			GridLocation* pos_ptr = &(*itr);
			inProcessPaths.insert(std::pair<std::pair<GridLocation*,GridLocation*>, bool>(std::pair<GridLocation*,GridLocation*>(pos_ptr, target_ptr), false));
			std::vector<Pathfinder::pathNode*> open;
			std::unordered_set<pathNode*> closed_set;
			std::vector<Pathfinder::pathNode*> closed;
			pathNode* root = new pathNode(pos_pair, 0 + getCost(pos_pair, target_grid)); 
			bool path_found = false;
			open.push_back(root);
			while (!open.empty()) {
				pathNode* s = open.front();
				open.erase(open.begin());
				if (Pathfinder::getCost(s->pos, target_grid) == 0) {
					s ->next = NULL;
					path_found = true;
					paths_mutex.lock();
					//targets_mutex.lock();
					allPaths.insert(std::pair<std::pair<GridLocation*, GridLocation*>, PathList>(std::pair<GridLocation*, GridLocation*>(pos_ptr, target_ptr), s->getPath()));
					PathList* path_ptr = &(allPaths[std::pair<GridLocation*, GridLocation*>(pos_ptr, target_ptr)]);					
					//if (target_values[target_ptr] != -2)				
					//paths.insert(std::pair<GridLocation*,std::pair<GridLocation*, PathList*>>(pos_ptr, std::pair<GridLocation*, PathList*>(target_ptr, path_ptr)));
					pathUpdates.insert(std::pair<std::pair<GridLocation*, GridLocation*>, bool>(std::pair<GridLocation*, GridLocation*>(pos_ptr, target_ptr), false));
					//targets_mutex.unlock();
					paths_mutex.unlock();
					//for (auto itr = path_ptr->begin(); itr != path_ptr->end(); itr++) {
					//	std::cout << "("<< itr->x << " " << itr->y << "), ";
					//}
					//std::cout << std::endl; 
					//std::cout << "Path Found!" << std::endl;
					break;
				}
				closed.push_back(s);

				std::vector<pathNode*>::iterator itr;
				std::vector<pathNode*> n =  s->neighbors(target_grid);
				for (itr = n.begin(); itr != n.end(); itr++) {
					if (!( (*itr)->containedIn(closed))) {
						if (!( (*itr)->containedIn(open))) {
							(*itr)->cost =  rows*cols*rows*cols;
							(*itr)->parent = NULL;
						}
						updateVertex(&open, &closed, s, (*itr), target_grid); 
					}
				}

			}
			if (!path_found) {
				//std::cout << "No Path Found!" << std::endl;
			}
		}
	}	
	//std::cout << "Number of Paths: " << allPaths.size() << std::endl;
}

/** Updates the open and closed vertices
 **
**/
void Pathfinder::updateVertex(std::vector<pathNode*>* open, std::vector<pathNode*>* closed, pathNode* p_node, pathNode* c_node, Grid* target_grid) {
	int old_cost = c_node->cost;
	getFullCost(p_node, c_node, target_grid);

	if (c_node->cost < old_cost) {
		if (c_node->containedIn(open))
			removeFrom(open, c_node);
		open->push_back(new pathNode(c_node->pos, c_node->cost + getCost(c_node->pos, target_grid), p_node));
	}
}

/** Updates the open and closed vertices
 **
**/
void Pathfinder::setVertex(std::vector<pathNode*>* closed, pathNode* p_node, Grid* target_grid) {
	pathNode* pp_node = p_node->parent;
	if (pp_node == NULL)
		return;
	
	int min_cost = rows*cols*rows*cols; 
	pathNode* min_node = NULL;
	if(!lineOfSight(pp_node, p_node, target_grid)) {
		std::vector<pathNode*>::iterator itr;
		std::vector<pathNode*> n =  p_node->neighbors(target_grid);
		for (itr = n.begin(); itr != n.end(); itr++) {
			if (( (*itr)->containedIn(closed))) {
				if ((*itr)->cost + p_node->distance(*itr) < min_cost) {
					min_cost = (*itr)->cost + p_node->distance(*itr);
					min_node = *itr;
				}
			}
		}
		p_node->parent = min_node;
		p_node->cost = min_cost;
	}
		
		
}
/** Updates the cost values for the given nodes
 **
**/
void Pathfinder::getFullCost(pathNode* p_node, pathNode* c_node, Grid* target_grid) {
	pathNode* pp_node = p_node->parent;
	//pp_node = NULL; //SET NULL TO JUST USE REGUAR A*
	if (pp_node != NULL && lineOfSight(pp_node, c_node, target_grid)) {
		if (pp_node->cost + pp_node->distance(c_node) < c_node->cost) {
			c_node->parent = pp_node;
			c_node->cost = pp_node->cost + pp_node->distance(c_node);
		}
	}
	else {
		if (p_node->cost + p_node->distance(c_node) < c_node->cost) {
			c_node->parent = p_node;
			c_node->cost = p_node->cost +  p_node->distance(c_node);
		}
	}
}

/** Used for Theta* algorithm to minimize search iterations and allow for travel over continuos grid
 **
**/
bool Pathfinder::lineOfSight(pathNode* p_node, pathNode* c_node, Grid* target_grid) {
	int p_row = p_node->pos.first;
	int p_col = p_node->pos.second;
	int c_row = c_node->pos.first;
	int c_col = c_node->pos.second;

	int d_row = c_row - p_row;
	int d_col = c_col - p_col;

	int f = 0;
	int s_col;
	int s_row;

	if (d_col < 0 ) {
		d_col = -1*d_col;
		s_col = -1;
	}
	else {
		s_col = 1;
	}
	
	if (d_row < 0 ) {
		d_row = -1*d_row;
		s_row = -1;
	}
	else {
		s_row = 1;
	}

	if (d_row >= d_col) { 
		while (p_row != c_row) {
			f += d_col;
			if (f >= d_row) {
				if ((*target_grid)[p_row + ((s_row - 1)/2)][ p_col + ((s_col - 1)/2)]) {
					return false;
				}
				p_col += s_col;
				f -= d_row;
			}
			if (f != 0 && (*target_grid)[p_row + ((s_row - 1)/2)][p_col + ((s_col - 1)/2)]) {
				return false;
			}
			if (d_col == 0 && (*target_grid)[p_row + ((s_row - 1)/2)][ p_col] && (*target_grid)[p_row + ((s_row - 1)/2)][ p_col - 1]) {
				return false;
			}
			p_row += s_row;
		}
	}
	else {
		while (p_col != c_col) {
			f += d_row;
			if (f >= d_col) {
				if ((*target_grid)[p_row + ((s_row - 1)/2)][ p_col + ((s_col - 1)/2)]) {
					return false;
				}
				p_row += s_row;
				f -= d_col;
			}
			if (f != 0 && (*target_grid)[p_row + ((s_row - 1)/2)][p_col + ((s_col - 1)/2)]) {
				return false;
			}
			if (d_row == 0 && (*target_grid)[p_row][ p_col + ((s_col - 1)/2)] && (*target_grid)[p_row - 1][p_col + ((s_col - 1)/2)]) {
				return false;
			}
			p_col += s_col;
		}
	}
	return true;
}

bool Pathfinder::canUpdateTargetGrid(sf::Vector2f start_pos) {
	GridLocation start_pair = getPositionMapping(start_pos);
	GridLocation* start_ptr = NULL;
	bool output = false;
	//targets_mutex.lock();
	if ((start_ptr = findTarget(start_pair)) != NULL)
		output = !inProcessTargets[start_ptr];
	//targets_mutex.unlock();
	return output;
}

bool Pathfinder::canUpdateStartPath(sf::Vector2f start_pos) {
	GridLocation start_pair = getPositionMapping(start_pos);
	GridLocation* start_ptr = NULL;	
	if ((start_ptr = findStart(start_pair)) == NULL)
		return false;
	return true;
}

GridLocation* Pathfinder::findTarget(GridLocation pos) {
	GridLocation* ptr = NULL;
	for (auto itr = targets.begin(); itr != targets.end(); itr++) {
		if ((*itr).first == pos && (*itr).first.first < level_height && (*itr).first.first >= 0 && (*itr).first.second < level_width && (*itr).first.second >= 0) {
			ptr = &((*itr).first);
			break;
		}
	}
	return ptr;
}

bool Pathfinder::isValidTarget(GridLocation* ptr) {
	if (ptr == NULL)
		return false;
	return (ptr->first < rows && ptr->first >= 0 && ptr->second < cols && ptr->second >= 0); 
}

GridLocation* Pathfinder::findNewTarget(GridLocation pos) {
	GridLocation* ptr = NULL;
	for (auto itr = targets.begin(); itr != targets.end(); itr++) {
		if ((*itr).first != pos && (*itr).first.first < level_height && (*itr).first.first >= 0 && (*itr).first.second < level_width && (*itr).first.second >= 0) {
			ptr = &((*itr).first);
			break;
		}
	}
	return ptr;
}


GridLocation* Pathfinder::findStart(GridLocation pos) {
	GridLocation* ptr = NULL;
	for (auto itr = start_positions.begin(); itr != start_positions.end(); itr++) {
		if (*itr == pos) {
			ptr = &(*itr);
			break;
		}
	}
	return ptr;
}
void Pathfinder::updateTargetGrid(sf::Vector2f start_pos, sf::Vector2f curr_pos) {
	GridLocation start_pair = getPositionMapping(start_pos);
	GridLocation curr_pair = getPositionMapping(curr_pos);
	//targets_mutex.lock();
	GridLocation* ptr = findTarget(start_pair);
	inProcessTargets[ptr] = true;
	//std::cout << "Updating Target Grid" << std::endl;
	generateHCost(ptr, curr_pair);

	for (auto itr = pathUpdates.begin(); itr != pathUpdates.end(); itr++) {
		if ((itr->first).second == ptr) {
			itr->second = true;
		}
	}
	inProcessTargets[ptr] = false;
	//targets_mutex.unlock();
}

/** Returns the next positions in the path from the given start position
 **
**/
void Pathfinder::generatePath2(sf::Vector2f start_pos, sf::Vector2f curr_pos) {
	//targets_mutex.lock();
	GridLocation start_pair = getPositionMapping(start_pos);
	GridLocation curr_pair = getPositionMapping(curr_pos);
	
	//std::cout << "Updating Paths Called" << std::endl;
	for (auto itr = pathUpdates.begin(); itr != pathUpdates.end(); itr++) {
		if (*(itr->first).first == start_pair) {
			*(itr->first).first = curr_pair;
			if (itr->second && isValidTarget(itr->first.second)) {
				generatePath(start_pair, *((itr->first).second), curr_pair);
			}
		}
	}
	//targets_mutex.unlock();
}
/** Generates path
 **
**/
void Pathfinder::generatePath(GridLocation start, GridLocation target, GridLocation new_start, bool change_path) {			
	std::vector<Pathfinder::pathNode*> open;
	std::vector<Pathfinder::pathNode*> closed;
	GridLocation* start_ptr = findStart(start);
	if (start_ptr == NULL) {
		//std::cout << "FAIL!" << std::endl;
		return;
	}

	GridLocation* target_ptr = findTarget(target);
	bool use_target = false;
	if (target_ptr != NULL)
		use_target = true;
		
	
	if (!inProcessPaths[std::pair<GridLocation*, GridLocation*>(start_ptr, target_ptr)]) {
		//std::cout << "Updating Paths" << std::endl;
		inProcessPaths[std::pair<GridLocation*, GridLocation*>(start_ptr, target_ptr)] = true;
		Grid* target_grid = &(target_grids[target_ptr]);
		//print(target_grid);
		GridLocation pos_pair = new_start;
		pathNode* root = new pathNode(pos_pair, 0 + getCost(pos_pair, target_grid)); 
		bool path_found = false;
		open.push_back(root);
		while (!open.empty()) {
			pathNode* s = open.front();
			open.erase(open.begin());
			if (Pathfinder::getCost(s->pos, target_grid) == 0) {
				//*start_ptr = start; ///REMOVE AFTER DONE TESTING
				s ->next = NULL;
				path_found = true;
				paths_mutex.lock();
				PathList* path_ptr = &(allPaths[std::pair<GridLocation*, GridLocation*>(start_ptr, target_ptr)]);
				*path_ptr = s->getPath();					
				path_ptr->pop_back();
				path_ptr->pop_back();
				if (change_path) {
					paths[start_ptr].first = target_ptr;
					paths[start_ptr].second = path_ptr;
				}
				paths_mutex.unlock();
				/*for (auto itr = path_ptr->begin(); itr != path_ptr->end(); itr++) {
					std::cout << "("<< itr->x << " " << itr->y << "), ";
				}
				std::cout << std::endl; */
				//std::cout << "Path Found!" << std::endl;
				break;
			}
			closed.push_back(s);

			std::vector<pathNode*>::iterator itr;
			std::vector<pathNode*> n =  s->neighbors(target_grid);
			for (itr = n.begin(); itr != n.end(); itr++) {
				if (!( (*itr)->containedIn(closed))) {
					if (!( (*itr)->containedIn(open))) {
						(*itr)->cost =  rows*cols*rows*cols;
						(*itr)->parent = NULL;
					}
					updateVertex(&open, &closed, s, (*itr), target_grid); 
				}
			}

		}
		if (!path_found) {
			//std::cout << "No Path Found!" << std::endl;
			//std::cout << "Start: " << start_ptr->first << " " << start_ptr->second << " Target: " << target_ptr->first << " " << target_ptr->second << std::endl;
			//std::cout << "Start: " << pos_pair.first << " " << pos_pair.second << " Target: " << std::endl;
		}
		inProcessPaths[std::pair<GridLocation*, GridLocation*>(start_ptr, target_ptr)] = false;
		pathUpdates[std::pair<GridLocation*, GridLocation*>(start_ptr, target_ptr)] = false;
	}	
}

bool Pathfinder::selectNewPath(GridLocation* start_ptr, GridLocation curr_pair) {

	for (auto itr = allPaths.begin(); itr != allPaths.end(); itr++) {
		if (itr->first.first == start_ptr && isValidTarget(itr->first.second) && target_values[itr->first.second] == -2 && !itr->second.empty()) { 
			//std::cout << "Branch 1" << std::endl;
			if (paths.empty()) {
				paths.insert(std::pair<GridLocation*,std::pair<GridLocation*, PathList*>>(start_ptr, std::pair<GridLocation*, PathList*>(itr->first.second, &(itr->second))));
			}			
			else {
				*start_ptr = curr_pair;
				//itr->second.clear();
				std::thread(&Pathfinder::generatePath, *start_ptr, *(itr->first.second), curr_pair, true).detach();
			}
			//std::cout << "Target: " << itr->first.second->first << " " << itr->first.second->second << std::endl;
			//for (auto itr1 = itr->second.begin(); itr1 != itr->second.end(); itr1++) {
			//	std::cout << "("<< itr1->x << " " << itr1->y << "), ";
			//}
			//std::cout << std::endl;
			return true;
		}
	}	

	for (auto itr = allPaths.begin(); itr != allPaths.end(); itr++) {
		//std::cout << "Branch 2" << std::endl;
		if (itr->first.first == start_ptr && isValidTarget(itr->first.second) && !itr->second.empty()) {
			if (paths.empty()) {
				paths.insert(std::pair<GridLocation*,std::pair<GridLocation*, PathList*>>(start_ptr, std::pair<GridLocation*, PathList*>(itr->first.second, &(itr->second))));
			}
			else {
				*start_ptr = curr_pair;
				//itr->second.clear();
				std::thread(&Pathfinder::generatePath, *start_ptr, *(itr->first.second), curr_pair, true).detach();
			}
			//std::cout << "Target: " << itr->first.second->first << " " << itr->first.second->second << std::endl;
			//for (auto itr1 = itr->second.begin(); itr1 != itr->second.end(); itr1++) {
			//	std::cout << "("<< itr1->x << " " << itr1->y << "), ";
			//}
			//std::cout << std::endl;
			return true;
		}
	}
	return false;
}

/** Returns the next positions in the path from the given start position
 **
**/
void Pathfinder::getNextPosition(float dist, sf::Vector2f start_pos, sf::Vector2f curr_pos, sf::Vector2f* next_pos, sf::Vector2f* direction) {
	////targets_mutex.lock();
	sf::Vector2f pos_next = curr_pos;
	sf::Vector2f dir(0,0);
	GridLocation start_pair = getPositionMapping(start_pos);
	GridLocation* start_ptr = findStart(start_pair);	
	if (start_ptr == NULL) {
		*next_pos = pos_next;
		*direction = dir;
		return;
	}
	GridLocation curr_pair = getPositionMapping(curr_pos);
	//paths_mutex.lock();
	if (paths.empty()) {
		if(!selectNewPath(start_ptr, curr_pair)) {
			*next_pos = pos_next;
			*direction = dir;
			return;
		}
	}

	PathList* path = paths[start_ptr].second;

	if (path->empty()) {
		if(!selectNewPath(start_ptr, curr_pair)) {
			*next_pos = pos_next;
			*direction = dir;
			return;
		}
	}

	GridLocation target_pair = *(paths[start_ptr].first);
	sf::Vector2f target_pos(target_pair.second * player_size, target_pair.first * player_size);
	if (findTarget(target_pair) == NULL) {
		*next_pos = pos_next;
		*direction = dir;
		return;
	}

	sf::Vector2f path_pos = path->back();
	/*if (first_run < 1) {
		std::cout << "Target: " << target_pair.first << " " << target_pair.second << std::endl;
		for (auto itr = path->begin(); itr != path->end(); itr++) {
			std::cout << "("<< itr->x << " " << itr->y << "), ";
		}
		std::cout << std::endl;
		std::cout << allPaths.size() << " ("<< path_pos.x << " " << path_pos.y << "), " << std::endl;
	}*/

	
	first_run++;
	float dist_total = getDistance(curr_pos, path_pos);
	float p = dist / dist_total;
	//std::cout << "Curr_pos " << curr_pos.x << " " << curr_pos.y << "Path_pos: "  << path_pos.x << " " << path_pos.y << "dist_total " << dist_total << "P: " << p << std::endl;

	if (p > 1)
		p = 1;
	pos_next.x = curr_pos.x + p*(path_pos.x - curr_pos.x);
	pos_next.y = curr_pos.y + p*(path_pos.y - curr_pos.y);

	if (path_pos.x - curr_pos.x > 0)
		dir.x = 1;
	else if (path_pos.x - curr_pos.x < 0)
		dir.x = -1;

	if (path_pos.y - curr_pos.y > 0) 
		dir.y = 1;
	else if (path_pos.y - curr_pos.y < 0)
		dir.y = -1;

	if (p > .99)
		path->pop_back();

	
	//paths_mutex.unlock();
	////targets_mutex.lock();
	*next_pos = pos_next;
	*direction = dir;
	return;
}


/** Removes a node from a node list
 **
**/
void Pathfinder::removeFrom(std::vector<pathNode*>* l, pathNode* n) {
	std::vector<pathNode*>::iterator itr;
	for (itr = l->begin(); itr != l->end(); itr++) {
		if ((*itr)->pos == n->pos) {
			l->erase(itr);
			return;
		}
	}
}


/** Returns if the position is valid
 **
**/
bool Pathfinder::isValidMove(GridLocation loc, Grid* target_grid) {
	return (loc.first < rows && loc.second < cols && loc.first >= 0 && loc.second >= 0 && (*target_grid)[loc.first][loc.second] != -1);
}

/** Generates costs on the grid
 **
**/
void Pathfinder::generateHCosts(void) {
	if (targets.empty())
		return;
	for (auto itr = targets.begin(); itr != targets.end(); itr++) {
		GridLocation target_pair = (*itr).first;

		Grid temp_grid = new int*[rows];
		for (int i = 0; i < rows; i++) {
			temp_grid[i] = new int[cols];
			for (int j = 0; j < cols; j++)
				temp_grid[i][j] = grid[i][j];
		}
	
		GridLocation* target_ptr = &((*itr).first);
		//std::cout << "Target Host " << (*itr).second << std::endl;
		//std::cout << target_values[target_ptr] << std::endl;
		temp_grid[target_pair.first][target_pair.second] = (*itr).second;
		std::vector<GridLocation> left;
		left.push_back(target_pair);
		generateHCost_helper(left, &temp_grid);
		paths_mutex.lock();
		target_grids.insert(std::pair<GridLocation*, Grid>(target_ptr, temp_grid));
		paths_mutex.unlock();
	}	
}

/** Generates costs on the grid
 **
**/
void Pathfinder::generateHCost(GridLocation* target_ptr, GridLocation curr_pos) {
	Grid temp_grid  = new int*[rows];
	for (int i = 0; i < rows; i++) {
		temp_grid[i] = new int[cols];
		for (int j = 0; j < cols; j++)
			temp_grid[i][j] = grid[i][j];
	}

	int type = -2;
	for (auto itr = targets.begin(); itr != targets.end(); itr++ ) {
		if (*target_ptr == itr->first) {
			type = itr->second;
			break;
		}
	}

	temp_grid[curr_pos.first][curr_pos.second] = type;
	std::vector<GridLocation> left;
	left.push_back(curr_pos);
	generateHCost_helper(left, &temp_grid);
	paths_mutex.lock();
	Grid* old_grid = &(target_grids[target_ptr]);
	*old_grid = temp_grid;	
	*target_ptr = curr_pos;
	paths_mutex.unlock();
}

/** Remove given pos from paths
 **
**/
void Pathfinder::removeFromPaths(sf::Vector2f pos) {
	GridLocation pos_pair = getPositionMapping(pos);
	//targets_mutex.lock();
	GridLocation* pos_ptr = findTarget(pos_pair);
	GridLocation* new_ptr = findNewTarget(pos_pair);

	
	if (pos_ptr == NULL) {
		return;	
	}

	//std::cout << "Removing from Paths!" << std::endl;
	for (auto itr = paths.begin(); itr != paths.end(); itr++) {
		//std::cout << "Checking Paths!" << std::endl;
		if (itr->second.first == pos_ptr) {
			pos_ptr->first = -1000;
			pos_ptr->second = -1000;
			itr->second.second->clear();
			//std::cout << "Target: " << itr->second.first->first << " " << itr->second.first->second << std::endl;
			//for (auto itr1 = itr->second.second->begin(); itr1 != itr->second.second->end(); itr1++) {
			//	std::cout << "("<< itr1->x << " " << itr1->y << "), ";
			//}
			//std::cout << std::endl;
			//std::cout << allPaths.size() << " " << paths.size() << std::endl;
		}
	}

	//targets_mutex.unlock();
		
}

/** Recursive helper method to generate costs on grid
 **
**/
void Pathfinder::generateHCost_helper(std::vector<GridLocation> left, Grid* target_grid) {
	if (left.empty())
		return;

	GridLocation prev_pos = left.front();
	left.erase(left.begin());
	
	int weight = (*target_grid)[prev_pos.first][prev_pos.second];
	if (weight < 0)
		weight = 0;

	if ( prev_pos.first + 1 < rows && (*target_grid)[prev_pos.first + 1][prev_pos.second] == 0) {
		(*target_grid)[prev_pos.first + 1][prev_pos.second] = weight + 1;
		left.push_back(GridLocation(prev_pos.first + 1, prev_pos.second));
	}

	if ( prev_pos.first - 1 >= 0 && (*target_grid)[prev_pos.first - 1][prev_pos.second] == 0) {
		(*target_grid)[prev_pos.first - 1][prev_pos.second] = weight + 1;
		left.push_back(GridLocation(prev_pos.first - 1, prev_pos.second));
	}

	if ( prev_pos.second + 1 < cols && (*target_grid)[prev_pos.first][prev_pos.second + 1] == 0) {
		(*target_grid)[prev_pos.first][prev_pos.second + 1] = weight + 1;
		left.push_back(GridLocation(prev_pos.first, prev_pos.second + 1));
	}

	if ( prev_pos.second - 1 >= 0 && (*target_grid)[prev_pos.first][prev_pos.second - 1] == 0) {
		(*target_grid)[prev_pos.first ][prev_pos.second - 1] = weight + 1;
		left.push_back(GridLocation(prev_pos.first, prev_pos.second - 1));
	}
		
	
	if ( prev_pos.first + 1 < rows && prev_pos.second + 1 < cols && (*target_grid)[prev_pos.first + 1][prev_pos.second + 1] == 0) {
		(*target_grid)[prev_pos.first + 1][prev_pos.second + 1] = weight + 1;
		left.push_back(GridLocation(prev_pos.first + 1, prev_pos.second + 1));
	}

	if ( prev_pos.first - 1 >= 0 && prev_pos.second - 1 >= 0 && (*target_grid)[prev_pos.first - 1][prev_pos.second - 1] == 0) {
		(*target_grid)[prev_pos.first - 1][prev_pos.second - 1] = weight + 1;
		left.push_back(GridLocation(prev_pos.first - 1, prev_pos.second - 1));
	}

	if ( prev_pos.first + 1 < rows && prev_pos.second - 1 >= 0 && (*target_grid)[prev_pos.first + 1][prev_pos.second - 1] == 0) {
		(*target_grid)[prev_pos.first + 1][prev_pos.second - 1] = weight + 1;
		left.push_back(GridLocation(prev_pos.first + 1, prev_pos.second - 1));
	}

	if ( prev_pos.first - 1 >= 0 && prev_pos.second + 1 < cols && (*target_grid)[prev_pos.first - 1][prev_pos.second + 1] == 0) {
		(*target_grid)[prev_pos.first - 1 ][prev_pos.second + 1] = weight + 1;
		left.push_back(GridLocation(prev_pos.first - 1, prev_pos.second + 1));
	}
	
	generateHCost_helper(left, target_grid);
		
}

/** Gets the cost of the position on the grid
 **
**/
int Pathfinder::getCost(sf::Vector2f pos, sf::Vector2f dir, Grid* target_grid) {
	GridLocation grid_pair = getPositionMapping(pos);
	grid_pair.first += dir.y;
	grid_pair.second += dir.x;

	if (grid_pair.first < rows && grid_pair.second < cols && grid_pair.first >= 0 && grid_pair.second >= 0) {
		int value =  (*target_grid)[grid_pair.first][grid_pair.second];
		if (value == -2 || value == -4)
			return 0;
		else if (value >= 0)
			return	value;
		else
			return rows*cols*rows*cols;
	}	

	return rows*cols*rows*cols;
		
		
}

/** Gets the cost of the position on the grid
 **
**/
int Pathfinder::getCost(sf::Vector2f pos, Grid* target_grid) {
	GridLocation grid_pair = getPositionMapping(pos);

	if (grid_pair.first < rows && grid_pair.second < cols && grid_pair.first >= 0 && grid_pair.second >= 0) {
		int value =  (*target_grid)[grid_pair.first][grid_pair.second];
		if (value == -2 || value == -4)
			return 0;
		else if (value >= 0)
			return	value;
		else
			return rows*cols*rows*cols;
	}	

	return rows*cols*rows*cols;
		
		
}

/** Gets the cost of the position on the grid
 **
**/
int Pathfinder::getCost(GridLocation loc, Grid* target_grid)  {

	if (loc.first < rows && loc.second < cols && loc.first >= 0 && loc.second >= 0) {
		int value =  (*target_grid)[loc.first][loc.second];
		if (value == -2 || value == -4)
			return 0;
		else if (value >= 0)
			return	value;
		else
			return rows*cols*rows*cols;
	}	

	return rows*cols*rows*cols;		
		
}

/** Updates the cost on the grid
 **
**/
int Pathfinder::addToCost(sf::Vector2f pos, sf::Vector2f dir, int incr, Grid* target_grid) {
	GridLocation grid_pair = getPositionMapping(pos);
	grid_pair.first += dir.y;
	grid_pair.second += dir.x;
	(*target_grid)[grid_pair.first][grid_pair.second] += incr;
}

/** Gets the grid mapping of the position
 **
**/
GridLocation Pathfinder::getPositionMapping(sf::Vector2f pos) {
	GridLocation grid_pair;
	grid_pair.first = (pos.y)  / player_size;
	grid_pair.second = (pos.x) / player_size;
	return grid_pair;
}

/** Gets the straight line distance between the two points
 **
**/
float Pathfinder::getDistance(sf::Vector2f pos,  sf::Vector2f pos2) {	
	return sqrt(pow(pos.x - pos2.x, 2.0) + pow(pos.y - pos2.y, 2.0));
}

/** Prints the grid
 **
**/
void Pathfinder::print(void) {
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			std::cout << grid[i][j];
			if (grid[i][j] < 10 && grid[i][j] >=0)
				std::cout << " ";
		}
		std::cout << "\n";
	}
	std::cout << std::endl;
}

void Pathfinder::print(Grid* target_grid) {
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			std::cout << (*target_grid)[i][j];
			if ((*target_grid)[i][j] < 10 && (*target_grid)[i][j] >=0)
				std::cout << " ";
		}
		std::cout << "\n";
	}
	std::cout << std::endl;
}


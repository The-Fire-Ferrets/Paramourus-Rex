#include "Pathfinder.h"

//Size of arrays
int Pathfinder::target_num;
int Pathfinder::start_num;
//target locs
std::pair<GridLocation, std::pair<GridLocation, int>>* Pathfinder::targets;
//Start positions
std::pair<GridLocation, std::pair<GridLocation, int>>* Pathfinder::start_positions;
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

//target types for start positions
std::map<GridLocation*, int> Pathfinder::start_targets;
//Current selected paths for start positions
std::map<GridLocation*, std::pair<GridLocation*, PathList*>> Pathfinder::paths;
//All available paths for start positions
std::map<std::pair<GridLocation*, GridLocation*>, PathList> Pathfinder::allPaths;
//Event delegate
std::vector<EventDelegate> Pathfinder::delegateFuncList;
//Mutex
std::mutex Pathfinder::paths_mutex;
//Targets that are being updated
std::map<GridLocation*, bool> Pathfinder::inProcessTargets;
//Paths the are being updated
std::map<std::pair<GridLocation*, GridLocation*>, bool> Pathfinder::inProcessPaths;
//Paths that require updating
std::map<std::pair<GridLocation*, GridLocation*>, bool> Pathfinder::pathUpdates;
//Target Values
std::map<GridLocation*, int> Pathfinder::target_values;
//Target is already being searched for
std::map<GridLocation*, bool> Pathfinder::target_taken;
//Start initial positions
std::map<GridLocation*, GridLocation> Pathfinder::initial_positions;
//Determines if player is in position and to change path towards player
std::map<std::pair<GridLocation*, GridLocation*>, bool> Pathfinder::inVision;
//Initial path generation is going on
bool Pathfinder::generatingPaths;

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

	target_num = 0;
	start_num = 0;
	start_positions = new std::pair<GridLocation, std::pair<GridLocation, int>>[size];
	targets = new std::pair<GridLocation, std::pair<GridLocation, int>>[size];

	paths_mutex.lock();
	paths.clear();
	start_targets.clear();
	inProcessTargets.clear();
	inProcessPaths.clear();
	pathUpdates.clear();
	allPaths.clear();
	target_grids.clear();
	target_values.clear();
	initial_positions.clear();
	inVision.clear();
	paths_mutex.unlock();
	generatingPaths = true;
}


/** Mapts the given boundary to the grid
 **
**/
void Pathfinder::addToGrid(std::vector<sf::FloatRect*> bounds, int path_type, int target_type) {
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
				if (path_type == -2 || path_type == -4 || path_type <= -5) {
					targets[target_num] = std::pair<GridLocation, std::pair<GridLocation, int>>(GridLocation(i,j), std::pair<GridLocation, int>(GridLocation(i,j), path_type));
					GridLocation target_pair = (targets[target_num].first);
					GridLocation* target_ptr = &(targets[target_num].first);
					initial_positions.insert(std::pair<GridLocation*, GridLocation>(&(targets[target_num].first), targets[target_num].second.first));
					target_values.insert(std::pair<GridLocation*, int>(target_ptr, targets[target_num].second.second));
					target_taken.insert(std::pair<GridLocation*, int>(target_ptr, false));
					inProcessTargets.insert(std::pair<GridLocation*, bool>(target_ptr, false));
					generatePathsToTarget();
					target_num++;				
				}
				else if (path_type == -3) {
					start_positions[start_num] = std::pair<GridLocation, std::pair<GridLocation, int>>(GridLocation(i,j), std::pair<GridLocation, int>(GridLocation(i,j), target_type));
					start_targets.insert(std::pair<GridLocation*, int>(&(start_positions[start_num].first), start_positions[start_num].second.second));
					initial_positions.insert(std::pair<GridLocation*, GridLocation>(&(start_positions[start_num].first), start_positions[start_num].second.first));
					paths.insert(std::pair<GridLocation*,std::pair<GridLocation*, PathList*>>(&(start_positions[start_num].first), std::pair<GridLocation*, PathList*>(NULL, NULL)));
					generatePathsFromStart();
					start_num++;		
				}
				else {
					grid[i][j] = path_type;
				}
				paths_mutex.unlock();
			}
	}			
}


/** Initializes the various structures with the path combinations
 **
**/
void Pathfinder::generatePathsToTarget() {	
	GridLocation target_pair = (targets[target_num].first);
	GridLocation* target_ptr = &(targets[target_num].first);

	for (int start_idx = 0; start_idx < start_num; start_idx++) {
		GridLocation pos_pair = start_positions[start_idx].first;
		GridLocation* pos_ptr = &(start_positions[start_idx].first);
		
		if (start_targets[pos_ptr] == target_values[target_ptr] || target_values[target_ptr] == -4 || target_values[target_ptr] == -5) {
			inProcessPaths.insert(std::pair<std::pair<GridLocation*,GridLocation*>, bool>(std::pair<GridLocation*,GridLocation*>(pos_ptr, target_ptr), false));
			pathUpdates.insert(std::pair<std::pair<GridLocation*, GridLocation*>, bool>(std::pair<GridLocation*, GridLocation*>(pos_ptr, target_ptr), false));
			PathList temp;
			temp.push_back(sf::Vector2f(pos_pair.second * player_size, pos_pair.first * player_size));
			allPaths.insert(std::pair<std::pair<GridLocation*, GridLocation*>, PathList>(std::pair<GridLocation*, GridLocation*>(pos_ptr, target_ptr), temp));
			inVision.insert(std::pair<std::pair<GridLocation*, GridLocation*>, bool>(std::pair<GridLocation*, GridLocation*>(pos_ptr, target_ptr), false));
		}
	}

	if (!generatingPaths) {
		inProcessTargets[target_ptr] = true;
		std::thread(generateHCost, target_ptr, target_pair).detach();
	}	
}

/** Initializes the various structures with the path combinations
 **
**/
void Pathfinder::generatePathsFromStart() {
	for (int targ_idx = 0; targ_idx < target_num; targ_idx++) {	
		GridLocation target_pair = (targets[targ_idx].first);
		GridLocation* target_ptr = &(targets[targ_idx].first);
		GridLocation pos_pair = start_positions[start_num].first;
		GridLocation* pos_ptr = &(start_positions[start_num].first);
		
		if (start_targets[pos_ptr] == target_values[target_ptr] || target_values[target_ptr] == -4 || target_values[target_ptr] == -5) {
			inProcessPaths.insert(std::pair<std::pair<GridLocation*,GridLocation*>, bool>(std::pair<GridLocation*,GridLocation*>(pos_ptr, target_ptr), false));
			pathUpdates.insert(std::pair<std::pair<GridLocation*, GridLocation*>, bool>(std::pair<GridLocation*, GridLocation*>(pos_ptr, target_ptr), false));
			PathList temp;
			temp.push_back(sf::Vector2f(pos_pair.second * player_size, pos_pair.first * player_size));
			allPaths.insert(std::pair<std::pair<GridLocation*, GridLocation*>, PathList>(std::pair<GridLocation*, GridLocation*>(pos_ptr, target_ptr), temp));
		}
	}	
}

/** Updates the open and closed vertices
 **
**/
void Pathfinder::updateVertex(std::vector<pathNode*>* open, std::vector<pathNode*>* closed, pathNode* p_node, pathNode* c_node, Grid* target_grid) {
	int old_cost = c_node->cost;
	getFullCost(p_node, c_node, target_grid);
	if (c_node->cost < old_cost) {
		if (c_node->containedIn(open)) {
			removeFrom(open, c_node);
		}
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

	int d_row = p_row - c_row;
	int d_col = p_col - c_col;

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
				if (!isValidMove(GridLocation(p_row + ((s_row - 1)/2), p_col + ((s_col - 1)/2)), target_grid, GridLocation(p_row, p_col))) {
					return false;
				}
				p_col += s_col;
				f -= d_row;
			}
			if (f != 0 && !isValidMove(GridLocation(p_row + ((s_row - 1)/2), p_col + ((s_col - 1)/2)), target_grid, GridLocation(p_row, p_col))) {
				return false;
			}
			if (d_col == 0 && !isValidMove(GridLocation(p_row + ((s_row - 1)/2), p_col), target_grid, GridLocation(p_row, p_col)) && !isValidMove(GridLocation(p_row + ((s_row - 1)/2), p_col - 1), target_grid, GridLocation(p_row, p_col))) {
				return false;
			}
			p_row += s_row;
		}
	}
	else {
		while (p_col != c_col) {
			f += d_row;
			if (f >= d_col) {
				if (!isValidMove(GridLocation(p_row + ((s_row - 1)/2), p_col + ((s_col - 1)/2)), target_grid, GridLocation(p_row, p_col))) {
					return false;
				}
				p_row += s_row;
				f -= d_col;
			}
			if (f != 0 && !isValidMove(GridLocation(p_row + ((s_row - 1)/2), p_col + ((s_col - 1)/2)), target_grid, GridLocation(p_row, p_col))) {
				return false;
			}
			if (d_row == 0 && !isValidMove(GridLocation(p_row, p_col + ((s_col - 1)/2)), target_grid, GridLocation(p_row, p_col)) && !isValidMove(GridLocation(p_row - 1, p_col + ((s_col - 1)/2)), target_grid, GridLocation(p_row, p_col))) {
				return false;
			}
			p_col += s_col;
		}
	}
	p_node->pos.first = p_row;
	p_node->pos.second = p_col;
	c_node->pos.first = c_row;
	c_node->pos.second = c_col;
	return true;
}

//Determined whether the target grid is available to update
bool Pathfinder::canUpdateTargetGrid(sf::Vector2f start_pos) {
	GridLocation start_pair = getPositionMapping(start_pos);
	GridLocation* start_ptr = NULL;
	bool output = false;
	if ((start_ptr = findTarget(start_pair)) != NULL)
		output = !inProcessTargets[start_ptr];
	return output;
}

//Determines if two positions point to the same object
bool Pathfinder::positionsEqual(GridLocation* pos1, GridLocation* pos2) {
	return *pos1 == *pos2 && initial_positions[pos1] == initial_positions[pos2];
}

//Deternies if the path is avialable for update
bool Pathfinder::canUpdateStartPath(sf::Vector2f init_pos, sf::Vector2f start_pos) {
	GridLocation init_pair = getPositionMapping(init_pos);	
	GridLocation start_pair = getPositionMapping(start_pos);
	GridLocation* start_ptr = findStart(init_pair, start_pair);	
	if (start_ptr == NULL) {
		return false;
	}

	for (auto itr = pathUpdates.begin(); itr != pathUpdates.end(); itr++) {
		if (positionsEqual(itr->first.first, start_ptr)) {
			if (itr->second && isValidTarget(itr->first.second) && !inProcessPaths[std::pair<GridLocation*, GridLocation*>((itr->first).first, itr->first.second)]) {
				return true;
			}
		}
	}
	return false;
}

//Finds the position pointer for the object at the grid location
GridLocation* Pathfinder::findTarget(GridLocation pos, GridLocation init) {
	GridLocation* ptr = NULL;
	if (init.first != -1) {
		for ( auto itr = initial_positions.begin(); itr != initial_positions.end(); itr++) {
			if (init == itr->second) {
				ptr = itr->first;
				break;
			}
		}
	}
	else {
		for (int targ_idx = 0; targ_idx < target_num; targ_idx++) {	
			if ((targets[targ_idx]).first == pos && (targets[targ_idx]).first.first < level_height && (targets[targ_idx]).first.first >= 0 && (targets[targ_idx]).first.second < level_width && (targets[targ_idx]).first.second >= 0) {
				ptr = &((targets[targ_idx]).first);
				break;
			}
		}
	}
	return ptr;
}

//Checks to see if the given position pointer points to a valid target
bool Pathfinder::isValidTarget(GridLocation* ptr) {
	if (ptr == NULL)
		return false;
	return (ptr->first < rows && ptr->first >= 0 && ptr->second < cols && ptr->second >= 0); 
}


//Returns the start position pointer that points to the object at the selected position and start position
GridLocation* Pathfinder::findStart(GridLocation init, GridLocation pos) {
	GridLocation* ptr = NULL;
	for ( auto itr = initial_positions.begin(); itr != initial_positions.end(); itr++) {
		if (init == itr->second) {
			ptr = itr->first;
			break;
		}
	}
	/*for (int start_idx = 0; start_idx < start_num; start_idx++) {
		if ((start_positions[start_idx]).first == pos && (start_positions[start_idx]).second.first == init) {
			ptr = &((start_positions[start_idx]).first);
			break;
		}
	}*/
	return ptr;
}

//Updates the target search grid
void Pathfinder::updateTargetGrid(sf::Vector2f start_pos, sf::Vector2f curr_pos) {
	GridLocation start_pair = getPositionMapping(start_pos);
	GridLocation curr_pair = getPositionMapping(curr_pos);
	GridLocation* ptr = findTarget(start_pair);
	inProcessTargets[ptr] = true;
	generateHCost(ptr, curr_pair);
	
	for (auto itr = pathUpdates.begin(); itr != pathUpdates.end(); itr++) {
		if ((itr->first).second == ptr) {
			itr->second = true;
		}
	}
	inProcessTargets[ptr] = false;
}

/** Generates a new path if the current path has become invalidated
 **
**/
void Pathfinder::generatePath2(sf::Vector2f init_pos, sf::Vector2f start_pos, sf::Vector2f curr_pos) {
	GridLocation init_pair = getPositionMapping(init_pos);
	GridLocation start_pair = getPositionMapping(start_pos);
	GridLocation* start_ptr = findStart(init_pair, start_pair);
	if (start_ptr == NULL)
		return;
	GridLocation curr_pair = getPositionMapping(curr_pos);
	for (auto itr = pathUpdates.begin(); itr != pathUpdates.end(); itr++) {
		if (positionsEqual(itr->first.first, start_ptr)) {
			*(itr->first).first = curr_pair;
			if (itr->second && isValidTarget(itr->first.second) && !inProcessPaths[std::pair<GridLocation*, GridLocation*>((itr->first).first, itr->first.second)]) {
				generatePath(init_pair, start_pair, *((itr->first).second), curr_pair);
			}
		}
	}
}

/** Generates path
 **
**/
void Pathfinder::generatePath(GridLocation init, GridLocation start, GridLocation target, GridLocation new_start, bool change_path) {			
	std::vector<Pathfinder::pathNode*> open;
	std::vector<Pathfinder::pathNode*> closed;
	GridLocation* start_ptr = findStart(init, start);
	//std::cout << "Path 1" << std::endl;	
	if (start_ptr == NULL) {
		return;
	}

	GridLocation* target_ptr = findTarget(target);
	
	if (target_ptr == NULL) {
		return;
	}
	//std::cout << "Path 2" << std::endl;	
	Grid* target_grid = &(target_grids[target_ptr]);
	
	if (*target_grid == NULL) {
		return;
	}

	inProcessPaths[std::pair<GridLocation*, GridLocation*>(start_ptr, target_ptr)] = true;

	GridLocation pos_pair = new_start;
	//std::cout << "Path 3" << std::endl;
	pathNode* root = new pathNode(pos_pair, 0 + getCost(pos_pair, target_grid)); 
	bool path_found = false;
	//std::cout << "Path 4" << std::endl;
	open.push_back(root);
	while (!open.empty()) {
		pathNode* s = open.front();
		open.erase(open.begin());
		if (Pathfinder::getCost(s->pos, target_grid) == 0) {
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
			break;
		}
		closed.push_back(s);

		std::vector<pathNode*> n =  s->neighbors(target_grid);
		for (auto itr_n = n.begin(); itr_n != n.end(); itr_n++) {
			if (!( (*itr_n)->containedIn(closed))) {
				if (!( (*itr_n)->containedIn(open))) {
					(*itr_n)->cost =  rows*cols*rows*cols;
					(*itr_n)->parent = NULL;
				}
				updateVertex(&open, &closed, s, (*itr_n), target_grid); 
			}
		}

	}
	inProcessPaths[std::pair<GridLocation*, GridLocation*>(start_ptr, target_ptr)] = false;
	pathUpdates[std::pair<GridLocation*, GridLocation*>(start_ptr, target_ptr)] = false;
}

//Selects a new path depending on targets on grid and objects path type and target type
bool Pathfinder::selectNewPath(GridLocation init_pair, GridLocation* start_ptr, GridLocation curr_pair) {
	//If path is already in process, return	
	///std::cout << "New path 1" << std::endl;	
	if (paths[start_ptr].second != NULL && paths[start_ptr].first != NULL) {
		if (inProcessPaths[std::pair<GridLocation*, GridLocation*>(start_ptr, paths[start_ptr].first)])
				return false;	
	}

	for (auto itr = inVision.begin(); itr != inVision.end(); itr++) {
		if ( itr->second && itr->first.first == start_ptr ) {
			if (isValidTarget(itr->first.second)) {
				if (inProcessPaths[std::pair<GridLocation*, GridLocation*>((itr->first).first, itr->first.second)])
					return false;
				//std::cout << "Branch 0" << std::endl;
				target_taken[itr->first.second] = true;
				if (paths[start_ptr].first != NULL)
					target_taken[paths[start_ptr].first] = false;
				*start_ptr = curr_pair;
				allPaths[std::pair<GridLocation*, GridLocation*>((itr->first).first, itr->first.second)].clear();
				allPaths[std::pair<GridLocation*, GridLocation*>((itr->first).first, itr->first.second)].push_back(sf::Vector2f(curr_pair.second * player_size, curr_pair.first * player_size));
				paths[start_ptr].first = itr->first.second;
				paths[start_ptr].second = &(allPaths[std::pair<GridLocation*, GridLocation*>((itr->first).first, itr->first.second)]);
				std::thread(&Pathfinder::generatePath, init_pair, *start_ptr, *(itr->first.second), curr_pair, true).detach();
				return true;
			}
			else {
				itr->second = false;
			}
		}
	
	}
	//std::cout << "New path 2" << std::endl;	
	//Select a flower if appropriate
	for (auto itr = allPaths.begin(); itr != allPaths.end(); itr++) {
		if (itr->first.first == start_ptr && isValidTarget(itr->first.second) && start_targets[start_ptr] == target_values[itr->first.second] && !itr->second.empty() && target_values[itr->first.second] == -2 && !target_taken[itr->first.second]) { 		
			if (inProcessPaths[std::pair<GridLocation*, GridLocation*>((itr->first).first, itr->first.second)])
				return false;
			target_taken[itr->first.second] = true;
			if (paths[start_ptr].first != NULL)
				target_taken[paths[start_ptr].first] = false;
			//std::cout << "Branch1" << std::endl;	
			*start_ptr = curr_pair;
			itr->second.clear();
			itr->second.push_back(sf::Vector2f(curr_pair.second * player_size, curr_pair.first * player_size));
			paths[start_ptr].first = itr->first.second;
			paths[start_ptr].second = &(itr->second);
			std::thread(&Pathfinder::generatePath, init_pair, *start_ptr, *(itr->first.second), curr_pair, true).detach();
			return true;
		}
	}	
	
	//Select a fake target if appropriate
	for (auto itr = allPaths.begin(); itr != allPaths.end(); itr++) {
		if (itr->first.first == start_ptr && isValidTarget(itr->first.second) && (start_targets[start_ptr] == target_values[itr->first.second] || target_values[itr->first.second] == -5)  && target_values[itr->first.second] <= -5 && *itr->first.second != curr_pair && !target_taken[itr->first.second]) {
			//std::cout << "Branch2 1" << std::endl;		
			if (inProcessPaths[std::pair<GridLocation*, GridLocation*>((itr->first).first, itr->first.second)])
				return false;
			target_taken[itr->first.second] = true;
			if (paths[start_ptr].first != NULL)
				target_taken[paths[start_ptr].first] = false;
			//std::cout << "Branch2 2" << std::endl;
			*start_ptr = curr_pair;
			itr->second.clear();
			//std::cout << "Branch2 3" << std::endl;
			itr->second.push_back(sf::Vector2f(curr_pair.second * player_size, curr_pair.first * player_size));
			paths[start_ptr].first = itr->first.second;
			paths[start_ptr].second = &(itr->second);
			//std::cout << "Branch2 4" << std::endl;
			std::thread(&Pathfinder::generatePath, init_pair, *start_ptr, *(itr->first.second), curr_pair, true).detach();
			//std::cout << "Branch2 5" << std::endl;
			return true;
		}
	}
	
	//Select then player if appropriate
	for (auto itr = allPaths.begin(); itr != allPaths.end(); itr++) {
		if (itr->first.first == start_ptr && isValidTarget(itr->first.second) && target_values[itr->first.second] == -4 && *itr->first.second != curr_pair) {			
			if (inProcessPaths[std::pair<GridLocation*, GridLocation*>((itr->first).first, itr->first.second)])
				return false;
			//std::cout << "Branch3 " << allPaths.size() << std::endl;
			*start_ptr = curr_pair;
			itr->second.clear();
			itr->second.push_back(sf::Vector2f(curr_pair.second * player_size, curr_pair.first * player_size));
			paths[start_ptr].first = itr->first.second;
			paths[start_ptr].second = &(itr->second);
			std::thread(&Pathfinder::generatePath, init_pair, *start_ptr, *(itr->first.second), curr_pair, true).detach();
			return true;
		}
	}
	return false;
}

//Removes the next position from the search path
void Pathfinder::removePositionFromPath(sf::Vector2f init_pos) {
	GridLocation init_pair = getPositionMapping(init_pos);
	for ( auto itr = initial_positions.begin(); itr != initial_positions.end(); itr++) {
		if (init_pair == itr->second) {
			paths[itr->first].second->pop_back();
			break;
		}
	}
}

//If player in vision, changes the path towards the player and ignores curent search
void Pathfinder::changeVision(sf::Vector2f start_init_pos, sf::Vector2f targ_init_pos) {
	GridLocation start_init_pair = getPositionMapping(start_init_pos);
	GridLocation targ_init_pair = getPositionMapping(targ_init_pos);

	GridLocation* start_ptr = findStart(start_init_pair, start_init_pair);
	GridLocation* targ_ptr = findTarget(targ_init_pair, targ_init_pair);
	
	if (start_ptr != NULL && targ_ptr != NULL) {
		if (paths[start_ptr].first != NULL) {
			if (paths[start_ptr].first == targ_ptr && target_values[targ_ptr] != -4)
				return;
			if (target_values[targ_ptr] != -4) {
				for (auto itr = inVision.begin(); itr != inVision.end(); itr++) {
					if (itr->first.first == start_ptr && itr->second)
						return;
				}
			}
			//std::cout << "Changing Vision " << target_values[targ_ptr] << " " << start_ptr->first << " " << start_ptr->second << " " << targ_ptr->first << " " << targ_ptr->second << std::endl;
			paths[start_ptr].second->clear();
		}
		
		if (target_values[targ_ptr] != -4) {
			paths[start_ptr].first = targ_ptr;
		}
		target_taken[targ_ptr] = false;
		inVision[std::pair<GridLocation*, GridLocation*>(start_ptr, targ_ptr)] = !inVision[std::pair<GridLocation*, GridLocation*>(start_ptr, targ_ptr)];
	}
}

/** Returns the next positions in the path from the given start position
 **
**/
bool Pathfinder::getNextPosition(float dist,  sf::Vector2f init_pos, sf::Vector2f start_pos, sf::Vector2f curr_pos, sf::Vector2f* next_pos, sf::Vector2f* direction) {
	//Gets mapping for variables
	//std::cout << "HERE" << std::endl;	
	sf::Vector2f pos_next = curr_pos;
	bool start_changed = false;
	sf::Vector2f dir(0,0);
	GridLocation init_pair = getPositionMapping(init_pos);	
	GridLocation start_pair = getPositionMapping(start_pos);
	GridLocation* start_ptr = findStart(init_pair, start_pair);

	if (start_ptr == NULL) {
		*next_pos = pos_next;
		*direction = dir;
		return start_changed;
	}

	GridLocation curr_pair = getPositionMapping(curr_pos);
	if (paths[start_ptr].second == NULL) {
		if(!(start_changed = selectNewPath(init_pair, start_ptr, curr_pair))) {
			*next_pos = pos_next;
			*direction = dir;
			return start_changed;
		}
	}

	PathList* path = paths[start_ptr].second;

	if (path == NULL) {
		*next_pos = pos_next;
		*direction = dir;
		return start_changed;
	}

	if (path->empty()) {
		if(!(start_changed = selectNewPath(init_pair, start_ptr, curr_pair))) {
			*next_pos = pos_next;
			*direction = dir;
			return start_changed;
		}
	}

	GridLocation target_pair = *(paths[start_ptr].first);
	sf::Vector2f target_pos(target_pair.second * player_size, target_pair.first * player_size);
	if (findTarget(target_pair) == NULL) {
		*next_pos = pos_next;
		*direction = dir;
		return start_changed;
	}

	sf::Vector2f path_pos = path->back();
	//Gets the next position torwards towards target and accounts for framerate distance
	float dist_total = getDistance(curr_pos, path_pos);
	if (dist_total < 1)
		dist_total = 1;
	float p = dist / dist_total;

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

	//Checks to see if conflict with wall
	GridLocation pos_check = getPositionMapping(pos_next);
	int fix = 0;
	if (grid[pos_check.first][pos_check.second] == -1) {
		path->pop_back();
		*next_pos = curr_pos;
		*direction = dir;
		return start_changed;
	}
	//std::cout << "HERE3" << std::endl;	
	
	if (p > .99)
		path->pop_back();
	
	//std::cout << "HERE2 " << "Position: " << pos_next.x << " " << pos_next.y << " Direction: " << dir.x << " " << dir.y << std::endl;	
	*next_pos = pos_next;
	*direction = dir;
	return start_changed;
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
bool Pathfinder::isValidMove(GridLocation loc, Grid* target_grid, GridLocation prev) {
	if (!((loc.first < rows) && (loc.second < cols) && (loc.first >= 0) && (loc.second >= 0))) 
		return false;

	if (prev == GridLocation(-1,-1))
		return ((*target_grid)[loc.first][loc.second] != -1);
	else {
		GridLocation dir;
		dir.first = loc.first - prev.first;
		dir.second = loc.second - prev.second;
		if  ((*target_grid)[loc.first][loc.second] == -1)
			return false;
		else if (dir.first != 0 && dir.second != 0 && ((*target_grid)[prev.first + dir.first][prev.second] == -1 || (*target_grid)[prev.first][prev.second + dir.second] == -1)) 
			return false;
	}
	return true;
}

/** Generates costs on the grid
 **
**/
void Pathfinder::generateHCosts(void) {
	if (target_num == 0)
		return;
	for (int targ_idx = 0; targ_idx < target_num; targ_idx++) {	
		GridLocation target_pair = (targets[targ_idx]).first;

		Grid temp_grid = new int*[rows];
		for (int i = 0; i < rows; i++) {
			temp_grid[i] = new int[cols];
			for (int j = 0; j < cols; j++)
				temp_grid[i][j] = grid[i][j];
		}
	
		GridLocation* target_ptr = &((targets[targ_idx]).first);
		temp_grid[target_pair.first][target_pair.second] = (targets[targ_idx]).second.second;
		std::vector<GridLocation> left;
		left.push_back(target_pair);
		generateHCost_helper(left, &temp_grid);
		target_grids.insert(std::pair<GridLocation*, Grid>(target_ptr, temp_grid));
	}
	//std::cout << "Pathfinder Cost Generation Success!" << std::endl;
	generatingPaths = false;	
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
	for (int targ_idx = 0; targ_idx < target_num; targ_idx++) {	
		if (*target_ptr == (targets[targ_idx].first)) {
			type = targets[targ_idx].second.second;
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
	GridLocation* pos_ptr = findTarget(pos_pair);

	
	if (pos_ptr == NULL) {
		return;	
	}

	for (int targ_idx = 0; targ_idx < target_num; targ_idx++) {	
		GridLocation init_pair = (targets[targ_idx]).second.first;
		if (init_pair == pos_pair) {
			(targets[targ_idx]).first.first = -1000;
			(targets[targ_idx]).first.second = -1000;
		}
	}

	for (auto itr = allPaths.begin(); itr != allPaths.end(); itr++) {
		if (itr->first.second == pos_ptr) {
			itr->second.clear();
		}
	}

	for (auto itr = inVision.begin(); itr != inVision.end(); itr++) {
		if (itr->first.second == pos_ptr) {
			itr->second = false;
		}
	}		
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
		if (value == -2 || value == -4 || value <= -5)
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
		if (value == -2 || value == -4 || value <= -5)
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
		if (value == -2 || value == -4 || value <= -5)
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
GridLocation Pathfinder::getPositionMapping(sf::Vector2f pos, bool non_wall) {
	GridLocation grid_pair;
	grid_pair.first = (pos.y)  / player_size;
	grid_pair.second = (pos.x) / player_size;

	int dir = 0;
	if (non_wall && grid[grid_pair.first][grid_pair.second] == -1) {
		if (grid[grid_pair.first + 1][grid_pair.second] != -1)
			dir = 1;
		else if (grid[grid_pair.first - 1][grid_pair.second] != -1)
			dir = 2;	
		else if (grid[grid_pair.first][grid_pair.second + 1] != -1)
			dir = 3;
		else if (grid[grid_pair.first][grid_pair.second - 1] != -1)
			dir = 4;
	}

	if (dir == 1)
		++grid_pair.first;
	else if (dir == 2)
		--grid_pair.first;
	else if (dir == 3)
		++grid_pair.second;
	else if (dir == 4)
		--grid_pair.second;

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

//Print the given grid
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


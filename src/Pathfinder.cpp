#include "Pathfinder.h"

//Blank grid representation of level
int** Pathfinder::grid;
//Grid representation of level
std::map<std::pair<int, int>, int**> Pathfinder::flower_grids;
//Grids for each map
//Number of rows
int Pathfinder::rows;
//Number of columns
int Pathfinder::cols;
//Level Height, width and player size
int Pathfinder::level_height;
int Pathfinder::level_width;
int Pathfinder::player_size;
//Flower locs
std::vector<sf::Vector2i> Pathfinder::flowers;
//start Position list
std::vector<sf::Vector2i> Pathfinder::start_positions;
//Paths
std::map<std::pair<int, int>, Pathfinder::pathNode*> Pathfinder::paths;
//Event delegate
std::vector<EventDelegate> Pathfinder::delegateFuncList;


/** Constructs the grid and resets the lists
 **
**/
void Pathfinder::Create(int lw, int lh, int ps) {
	player_size = 10;
	level_height = lh;
	level_width = lw;
	rows = level_height / player_size;
	cols = level_width / player_size;

	grid = new int*[rows];

	for (int i = 0; i < rows; i++) {
		grid[i] = new int[cols];
		for (int j = 0; j < cols; j++)
			grid[i][j] = 0;
	}
	paths.clear();
	start_positions.clear();
	flowers.clear();
}


/** Mapts the given boundary to the grid
 **
**/
void Pathfinder::addToGrid(std::vector<sf::FloatRect*> bounds, int type) {
	std::vector<sf::FloatRect*>::iterator itr;
	for (itr = bounds.begin(); itr != bounds.end(); itr++) {
		int top_left_row = (*itr)->top  / player_size;
		int top_left_col = (*itr)->left / player_size;
		int bottom_right_row = ((*itr)->top + (*itr)->height)  / player_size;
		int bottom_right_col = ((*itr)->left + (*itr)->width) / player_size;

		for (int i = top_left_row; i < bottom_right_row; i++)
			for (int j = top_left_col; j < bottom_right_col; j++) {
				if (type == -2) {
					flowers.push_back(sf::Vector2i(i,j));
				}
				else if (type == -3) {
					start_positions.push_back(sf::Vector2i(i,j));
				}
				else
					grid[i][j] = type;
			}
	}
			
}


/** Generates paths from all start positions to the flower
 **
**/
void Pathfinder::generatePaths() {
	std::vector<sf::Vector2i>::iterator itr;
	std::vector<sf::Vector2i>::iterator itr_flowers;
	for (itr_flowers = flowers.begin(); itr_flowers != flowers.end(); ++itr_flowers) {
		sf::Vector2i flower_pos = *itr_flowers;
		std::pair<int, int> flower_pair(flower_pos.x, flower_pos.y);
		int*** flower_grid = &(flower_grids[flower_pair]);
		for (itr = start_positions.begin(); itr != start_positions.end(); ++itr) {
			sf::Vector2i pos = *itr;
			std::vector<Pathfinder::pathNode*> open;
			std::vector<Pathfinder::pathNode*> closed;
			std::pair<int, int> pos_pair(pos.x, pos.y);
			pathNode* root = new pathNode(pos_pair, 0 + getCost(pos_pair, flower_grid)); 
			bool path_found = false;
			open.push_back(root);
			while (!open.empty()) {
				pathNode* s = open.front();
				open.erase(open.begin());
				if (Pathfinder::getCost(s->pos, flower_grid) == 0) {
					s ->next = NULL;
					path_found = true;
					std::cout << "Path Found!" << std::endl;
					paths.insert(std::pair<std::pair<int, int>, pathNode*>(pos_pair, s->getRoot()));
					break;
				}
				closed.push_back(s);

				std::vector<pathNode*>::iterator itr;
				std::vector<pathNode*> n =  s->neighbors(flower_grid);
				for (itr = n.begin(); itr != n.end(); itr++) {
					if (!( (*itr)->containedIn(closed))) {
						if (!( (*itr)->containedIn(open))) {
							(*itr)->cost =  rows*cols*rows*cols;
							(*itr)->parent = NULL;
						}
						updateVertex(&open, &closed, s, (*itr), flower_grid); 
					}
				}

			}
			if (!path_found)
				std::cout << "No Path Found!" << std::endl;
		}
	}	
}

/** Returns the next positions in the path from the given start position
 **
**/
sf::Vector2f Pathfinder::getNextPosition(sf::Vector2f start_pos) {
	sf::Vector2i pos = getPositionMapping(start_pos);
	std::pair<int, int> pos_pair(pos.x, pos.y);
	sf::Vector2f pos_next;

	if (!paths.empty()) {
		pathNode* path = paths[pos_pair];
		pos_next = sf::Vector2f(path->pos.second * player_size, path->pos.first * player_size);
		if (path->next != NULL)
			paths[pos_pair] = paths[pos_pair]->next;
	}
	else {
		pos_next = start_pos;
	}
	return pos_next;
	/*
	sf::Vector2i pos = getPositionMapping(start_pos);
	std::pair<int, int> pos_pair(pos.x, pos.y);

	std::pair<std::pair<int, int>, pathNode*>* flower_path = &paths[pos_pair];
	pathNode* path = flower_path->second;
	sf::Vector2f pos_next(path->pos.second * player_size, path->pos.first * player_size);
	if (path->next != NULL)
		path = path->next;
	return pos_next;
	*/
}

/** Used for Theta* algorithm to minimize search iterations and allow for travel over continuos grid
 **
**/
bool Pathfinder::lineOfSight(pathNode* p_node, pathNode* c_node, int*** flower_grid) {
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
		d_col *= -1;
		s_col = -1;
	}
	else {
		s_col = 1;
	}
	
	if (d_row < 0 ) {
		d_row *= -1;
		s_row = -1;
	}
	else {
		s_row = 1;
	}

	if (d_row >= d_col) { 
		while (p_row != c_row) {
			f += d_col;
			if (f >= d_row) {
				if ((*flower_grid)[p_row + ((s_row - 1)/2)][ p_col + ((s_col - 1)/2)]) {
					return false;
				}
				p_col += s_col;
				f -= d_row;
			}
			if (f != 0 && (*flower_grid)[p_row + ((s_row - 1)/2)][p_col + ((s_col - 1)/2)]) {
				return false;
			}
			if (d_col == 0 && (*flower_grid)[p_row + ((s_row - 1)/2)][ p_col] && (*flower_grid)[p_row + ((s_row - 1)/2)][ p_col - 1]) {
				return false;
			}
			p_row += s_row;
		}
	}
	else {
		while (p_col != p_col) {
			f += d_row;
			if (f >= d_col) {
				if ((*flower_grid)[p_row + ((s_row - 1)/2)][ p_col + ((s_col - 1)/2)]) {
					return false;
				}
				p_row += s_row;
				f -= d_col;
			}
			if (f != 0 && (*flower_grid)[p_row + ((s_row - 1)/2)][p_col + ((s_col - 1)/2)]) {
				return false;
			}
			if (d_row == 0 && (*flower_grid)[p_row][ p_col + ((s_col - 1)/2)] && (*flower_grid)[p_row - 1][p_col + ((s_col - 1)/2)]) {
				return false;
			}
			p_col += s_col;
		}
	}
	return true;
}

/** Updates the open and closed vertices
 **
**/
void Pathfinder::updateVertex(std::vector<pathNode*>* open, std::vector<pathNode*>* closed, pathNode* p_node, pathNode* c_node, int*** flower_grid) {
	int old_cost = c_node->cost;
	getFullCost(p_node, c_node, flower_grid);

	if (c_node->cost < old_cost) {
		if (c_node->containedIn(open))
			removeFrom(open, c_node);
		open->push_back(new pathNode(c_node->pos, c_node->cost + getCost(c_node->pos, flower_grid), p_node));
	}
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

/** Updates the cost values for the given nodes
 **
**/
void Pathfinder::getFullCost(pathNode* p_node, pathNode* c_node, int*** flower_grid) {
	pathNode* pp_node = p_node->parent;
	if (pp_node != NULL && lineOfSight(pp_node, c_node, flower_grid)) {
		if (pp_node->cost + pp_node->distance(c_node) < c_node->cost) {
			c_node->parent = pp_node;
			c_node->cost = pp_node->cost + pp_node->distance(c_node);
		}
	}
	else {
		if (p_node->cost + c_node->distance(p_node) < c_node->cost) {
			c_node->parent = p_node;
			c_node->cost = p_node->cost +  c_node->distance(p_node);
		}
	}
}

/** Returns if the position is valid
 **
**/
bool Pathfinder::isValidMove(std::pair<int, int> loc, int*** flower_grid) {
	return (loc.first < rows && loc.second < cols && loc.first >= 0 && loc.second >= 0 && (*flower_grid)[loc.first][loc.second] != -1);
}

/** Generates costs on the grid
 **
**/
void Pathfinder::generateHCosts(void) {
	if (flowers.empty())
		return;
	std::vector<sf::Vector2i>::iterator itr;
	for (itr = flowers.begin(); itr != flowers.end(); itr++) {
		sf::Vector2i flower_pos = (*itr);

		int** temp_grid = grid;
		temp_grid[flower_pos.x][flower_pos.y] = -2;

		std::vector<sf::Vector2i> left;
		left.push_back(flower_pos);
		generateHCost_helper(left, &temp_grid);

		flower_grids.insert(std::pair<std::pair<int, int>, int**>(std::pair<int, int>(flower_pos.x, flower_pos.y), temp_grid));
	}	
}

/** Remove given pos from paths
 **
**/
void Pathfinder::removeFromPaths(sf::Vector2f pos) {
	sf::Vector2i grid_pos = getPositionMapping(pos);
	std::pair<int, int> pos_pair(grid_pos.x, grid_pos.y);

	
}

/** Recursive helper method to generate costs on grid
 **
**/
void Pathfinder::generateHCost_helper(std::vector<sf::Vector2i> left, int*** flower_grid) {
	if (left.empty())
		return;
	
	sf::Vector2i prev_pos = left.front();
	left.erase(left.begin());
	
	int weight = (*flower_grid)[prev_pos.x][prev_pos.y];
	if (weight < 0)
		weight = 0;

	if ( prev_pos.x + 1 < rows && (*flower_grid)[prev_pos.x + 1][prev_pos.y] == 0) {
		(*flower_grid)[prev_pos.x + 1][prev_pos.y] = weight + 1;
		left.push_back(sf::Vector2i(prev_pos.x + 1, prev_pos.y));
	}

	if ( prev_pos.x - 1 >= 0 && (*flower_grid)[prev_pos.x - 1][prev_pos.y] == 0) {
		(*flower_grid)[prev_pos.x - 1][prev_pos.y] = weight + 1;
		left.push_back(sf::Vector2i(prev_pos.x - 1, prev_pos.y));
	}

	if ( prev_pos.y + 1 < cols && (*flower_grid)[prev_pos.x][prev_pos.y + 1] == 0) {
		(*flower_grid)[prev_pos.x][prev_pos.y + 1] = weight + 1;
		left.push_back(sf::Vector2i(prev_pos.x, prev_pos.y + 1));
	}

	if ( prev_pos.y - 1 >= 0 && (*flower_grid)[prev_pos.x][prev_pos.y - 1] == 0) {
		(*flower_grid)[prev_pos.x ][prev_pos.y - 1] = weight + 1;
		left.push_back(sf::Vector2i(prev_pos.x, prev_pos.y - 1));
	}
		
	
	if ( prev_pos.x + 1 < rows && prev_pos.y + 1 < cols && (*flower_grid)[prev_pos.x + 1][prev_pos.y + 1] == 0) {
		(*flower_grid)[prev_pos.x + 1][prev_pos.y + 1] = weight + 1;
		left.push_back(sf::Vector2i(prev_pos.x + 1, prev_pos.y + 1));
	}

	if ( prev_pos.x - 1 >= 0 && prev_pos.y - 1 >= 0 && (*flower_grid)[prev_pos.x - 1][prev_pos.y - 1] == 0) {
		(*flower_grid)[prev_pos.x - 1][prev_pos.y - 1] = weight + 1;
		left.push_back(sf::Vector2i(prev_pos.x - 1, prev_pos.y - 1));
	}

	if ( prev_pos.x + 1 < rows && prev_pos.y - 1 >= 0 && (*flower_grid)[prev_pos.x + 1][prev_pos.y - 1] == 0) {
		(*flower_grid)[prev_pos.x + 1][prev_pos.y - 1] = weight + 1;
		left.push_back(sf::Vector2i(prev_pos.x + 1, prev_pos.y - 1));
	}

	if ( prev_pos.x - 1 >= 0 && prev_pos.y + 1 < cols && (*flower_grid)[prev_pos.x - 1][prev_pos.y + 1] == 0) {
		(*flower_grid)[prev_pos.x - 1 ][prev_pos.y + 1] = weight + 1;
		left.push_back(sf::Vector2i(prev_pos.x - 1, prev_pos.y + 1));
	}

	generateHCost_helper(left, flower_grid);
		
}
/** Gets the cost of the position on the grid
 **
**/
int Pathfinder::getCost(sf::Vector2f pos, sf::Vector2f dir, int*** flower_grid) {
	sf::Vector2i grid_pos = getPositionMapping(pos);
	grid_pos.x += dir.y;
	grid_pos.y += dir.x;

	if (grid_pos.x < rows && grid_pos.y < cols && grid_pos.x >= 0 && grid_pos.y >= 0) {
		int value =  (*flower_grid)[grid_pos.x][grid_pos.y];
		if (value == -2)
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
int Pathfinder::getCost(sf::Vector2f pos, int*** flower_grid) {
	sf::Vector2i grid_pos = getPositionMapping(pos);

	if (grid_pos.x < rows && grid_pos.y < cols && grid_pos.x >= 0 && grid_pos.y >= 0) {
		int value =  (*flower_grid)[grid_pos.x][grid_pos.y];
		if (value == -2)
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
int Pathfinder::getCost(std::pair<int, int> loc, int*** flower_grid)  {

	if (loc.first < rows && loc.second < cols && loc.first >= 0 && loc.second >= 0) {
		int value =  (*flower_grid)[loc.first][loc.second];
		if (value == -2)
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
int Pathfinder::addToCost(sf::Vector2f pos, sf::Vector2f dir, int incr, int*** flower_grid) {
	sf::Vector2i grid_pos = getPositionMapping(pos);
	grid_pos.x += dir.y;
	grid_pos.y += dir.x;
	(*flower_grid)[grid_pos.x][grid_pos.y] += incr;
}

/** Gets the grid mapping of the position
 **
**/
sf::Vector2i Pathfinder::getPositionMapping(sf::Vector2f pos) {
	sf::Vector2i grid_pos;
	grid_pos.x = (pos.y)  / player_size;
	grid_pos.y = (pos.x) / player_size;
	return grid_pos;
}

/** Gets the straight line distance between the two points
 **
**/
float Pathfinder::getDistance(sf::Vector2f pos,  sf::Vector2f dir) {
	sf::Vector2i grid_pos;
	grid_pos.x = abs(dir.x) * (player_size - (int) pos.x % player_size);
	grid_pos.y = abs(dir.y) * (player_size - (int) pos.y % player_size);
	
	return sqrt(pow(grid_pos.y, 2.0) + pow(grid_pos.x, 2.0));	

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

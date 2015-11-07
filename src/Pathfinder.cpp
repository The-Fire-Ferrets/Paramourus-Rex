#include "Pathfinder.h"

//Grid representation of level
int** Pathfinder::grid;
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
//Lists
std::vector<Pathfinder::pathNode*> Pathfinder::open;
std::vector<Pathfinder::pathNode*> Pathfinder::closed;
Pathfinder::pathNode* Pathfinder::path = NULL;


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

	path = NULL;
	open.clear();	
	closed.clear();

}

void Pathfinder::addToGrid(std::vector<sf::FloatRect*> bounds, int type) {
	std::vector<sf::FloatRect*>::iterator itr;
	for (itr = bounds.begin(); itr != bounds.end(); itr++) {
		int top_left_row = (*itr)->top  / player_size;
		int top_left_col = (*itr)->left / player_size;
		int bottom_right_row = ((*itr)->top + (*itr)->height)  / player_size;
		int bottom_right_col = ((*itr)->left + (*itr)->width) / player_size;

		for (int i = top_left_row; i < bottom_right_row; i++)
			for (int j = top_left_col; j < bottom_right_col; j++) {
				grid[i][j] = type;
				if (type == -2) {
					flowers.push_back(sf::Vector2i(i,j));
				}
			}
	}
			
}

void Pathfinder::generatePath(sf::Vector2f start_pos) {
	if (path == NULL) {
		std::cout << start_pos.x << " " << start_pos.y << std::endl;
		sf::Vector2i pos = getPositionMapping(start_pos);
		pathNode* root = new pathNode(std::pair<int, int>(pos.x, pos.y), 0 + getCost(start_pos)); 
	
		open.push_back(root);
		while (!open.empty()) {
			pathNode* s = open.front();
			open.erase(open.begin());
			if (Pathfinder::getCost(s->pos) == 0) {
				s ->next = NULL;
				path = s->getRoot();
				std::cout << "Path Found!" << std::endl;
				return;
			}
			closed.push_back(s);

			std::vector<pathNode*>::iterator itr;
			std::vector<pathNode*> n =  s->neighbors();
			for (itr = n.begin(); itr != n.end(); itr++) {
				if (!( (*itr)->containedIn(closed))) {
					if (!( (*itr)->containedIn(open))) {
						(*itr)->cost =  rows*cols*rows*cols;
						(*itr)->parent = NULL;
					}
					updateVertex(s, (*itr)); 
				}
			}

		}
		std::cout << "No Path Found!" << std::endl;
		return;
	}
	
	
}

sf::Vector2f Pathfinder::getNextPosition(void) {
	sf::Vector2f pos(path->pos.second * player_size, path->pos.first * player_size);
	if (path->next != NULL)
		path = path->next;
	return pos;
}

bool Pathfinder::lineOfSight(pathNode* p_node, pathNode* c_node) {
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
				if (grid[p_row + ((s_row - 1)/2)][ p_col + ((s_col - 1)/2)]) {
					return false;
				}
				p_col += s_col;
				f -= d_row;
			}
			if (f != 0 && grid[p_row + ((s_row - 1)/2)][p_col + ((s_col - 1)/2)]) {
				return false;
			}
			if (d_col == 0 && grid[p_row + ((s_row - 1)/2)][ p_col] && grid[p_row + ((s_row - 1)/2)][ p_col - 1]) {
				return false;
			}
			p_row += s_row;
		}
	}
	else {
		while (p_col != p_col) {
			f += d_row;
			if (f >= d_col) {
				if (grid[p_row + ((s_row - 1)/2)][ p_col + ((s_col - 1)/2)]) {
					return false;
				}
				p_row += s_row;
				f -= d_col;
			}
			if (f != 0 && grid[p_row + ((s_row - 1)/2)][p_col + ((s_col - 1)/2)]) {
				return false;
			}
			if (d_row == 0 && grid[p_row][ p_col + ((s_col - 1)/2)] && grid[p_row - 1][p_col + ((s_col - 1)/2)]) {
				return false;
			}
			p_col += s_col;
		}
	}
	return true;
}

void Pathfinder::updateVertex(pathNode* p_node, pathNode* c_node) {
	int old_cost = c_node->cost;
	getFullCost(p_node, c_node);

	if (c_node->cost < old_cost)
		if (c_node->containedIn(open))
			removeFromOpen(c_node);
		open.push_back(new pathNode(c_node->pos, c_node->cost + getCost(c_node->pos), p_node));
}

void Pathfinder::removeFromOpen(pathNode* n) {
	std::vector<pathNode*>::iterator itr;
		for (itr = open.begin(); itr != open.end(); itr++) {
			if ((*itr)->pos == n->pos) {
				open.erase(itr);
				return;
			}
		}
}
void Pathfinder::getFullCost(pathNode* p_node, pathNode* c_node) {
	//pathNode* pp_node = p_node->parent;
	pathNode* pp_node = NULL;
	if (pp_node != NULL && lineOfSight(pp_node, c_node)) {
		//if (pp_node->cost + pp_node->distance(c_node) < c_node->cost) {
		if (getCost(pp_node->pos) + pp_node->distance(c_node) < c_node->cost) {
			c_node->parent = pp_node;
			//c_node->cost = pp_node->cost + pp_node->distance(c_node);
			c_node->cost = getCost(pp_node->pos) + pp_node->distance(c_node);
		}
	}
	else {
		//if (p_node->cost + c_node->distance(p_node) < c_node->cost) {
		if (getCost(p_node->pos) + c_node->distance(p_node) < c_node->cost) {
			c_node->parent = p_node;
			//c_node->cost = p_node->cost +  c_node->distance(p_node);
			c_node->cost = getCost(p_node->pos) + abs(c_node->cost - p_node->cost);
			//c_node->cost = getCost(p_node->pos) + c_node->distance(p_node);
		}
	}
}

void Pathfinder::reset(void) {
	
}

bool Pathfinder::isValidMove(std::pair<int, int> loc) {
	return (loc.first < rows && loc.second < cols && loc.first >= 0 && loc.second >= 0 && grid[loc.first][loc.second] != -1);
}

void Pathfinder::generateHCost(void) {
	if (flowers.empty())
		return;
	sf::Vector2i flower_pos = flowers.front();
	std::vector<sf::Vector2i> left;
	left.push_back(flower_pos);
	generateHCost_helper(left);	
}

int Pathfinder::getCost(sf::Vector2f pos, sf::Vector2f dir) {
	sf::Vector2i grid_pos = getPositionMapping(pos);
	grid_pos.x += dir.y;
	grid_pos.y += dir.x;

	if (grid_pos.x < rows && grid_pos.y < cols && grid_pos.x >= 0 && grid_pos.y >= 0) {
		int value =  grid[grid_pos.x][grid_pos.y];
		if (value == -2)
			return 0;
		else if (value >= 0)
			return	value;
		else
			return rows*cols*rows*cols;
	}	

	return rows*cols*rows*cols;
		
		
}


int Pathfinder::getCost(sf::Vector2f pos) {
	sf::Vector2i grid_pos = getPositionMapping(pos);

	if (grid_pos.x < rows && grid_pos.y < cols && grid_pos.x >= 0 && grid_pos.y >= 0) {
		int value =  grid[grid_pos.x][grid_pos.y];
		if (value == -2)
			return 0;
		else if (value >= 0)
			return	value;
		else
			return rows*cols*rows*cols;
	}	

	return rows*cols*rows*cols;
		
		
}

int Pathfinder::getCost(std::pair<int, int> loc)  {

	if (loc.first < rows && loc.second < cols && loc.first >= 0 && loc.second >= 0) {
		int value =  grid[loc.first][loc.second];
		if (value == -2)
			return 0;
		else if (value >= 0)
			return	value;
		else
			return rows*cols*rows*cols;
	}	

	return rows*cols*rows*cols;		
		
}

int Pathfinder::addToCost(sf::Vector2f pos, sf::Vector2f dir, int incr) {
	sf::Vector2i grid_pos = getPositionMapping(pos);
	grid_pos.x += dir.y;
	grid_pos.y += dir.x;
	grid[grid_pos.x][grid_pos.y] += incr;
}
sf::Vector2i Pathfinder::getPositionMapping(sf::Vector2f pos) {
	sf::Vector2i grid_pos;
	grid_pos.x = (pos.y)  / player_size;
	grid_pos.y = (pos.x) / player_size;
	//std::cout << "X " << grid_pos.x << " " << pos.x << " Y " << grid_pos.y << " " << pos.y << std::endl;
	return grid_pos;
}

float Pathfinder::getDistance(sf::Vector2f pos,  sf::Vector2f dir) {
	sf::Vector2i grid_pos;
	grid_pos.x = abs(dir.x) * (player_size - (int) pos.x % player_size);
	grid_pos.y = abs(dir.y) * (player_size - (int) pos.y % player_size);
	
	//std::cout << sqrt(pow(grid_pos.y, 2.0) + pow(grid_pos.x, 2.0)) << std::endl;
	return sqrt(pow(grid_pos.y, 2.0) + pow(grid_pos.x, 2.0));	

}
void Pathfinder::generateHCost_helper(std::vector<sf::Vector2i> left) {
	if (left.empty())
		return;
	
	sf::Vector2i prev_pos = left.front();
	left.erase(left.begin());
	
	int weight = grid[prev_pos.x][prev_pos.y];
	if (weight < 0)
		weight = 0;

	if ( prev_pos.x + 1 < rows && grid[prev_pos.x + 1][prev_pos.y] == 0) {
		grid[prev_pos.x + 1][prev_pos.y] = weight + 1;
		left.push_back(sf::Vector2i(prev_pos.x + 1, prev_pos.y));
	}

	if ( prev_pos.x - 1 >= 0 && grid[prev_pos.x - 1][prev_pos.y] == 0) {
		grid[prev_pos.x - 1][prev_pos.y] = weight + 1;
		left.push_back(sf::Vector2i(prev_pos.x - 1, prev_pos.y));
	}

	if ( prev_pos.y + 1 < cols && grid[prev_pos.x][prev_pos.y + 1] == 0) {
		grid[prev_pos.x][prev_pos.y + 1] = weight + 1;
		left.push_back(sf::Vector2i(prev_pos.x, prev_pos.y + 1));
	}

	if ( prev_pos.y - 1 >= 0 && grid[prev_pos.x][prev_pos.y - 1] == 0) {
		grid[prev_pos.x ][prev_pos.y - 1] = weight + 1;
		left.push_back(sf::Vector2i(prev_pos.x, prev_pos.y - 1));
	}
		
	
	if ( prev_pos.x + 1 < rows && prev_pos.y + 1 < cols && grid[prev_pos.x + 1][prev_pos.y + 1] == 0) {
		grid[prev_pos.x + 1][prev_pos.y + 1] = weight + 1;
		left.push_back(sf::Vector2i(prev_pos.x + 1, prev_pos.y + 1));
	}

	if ( prev_pos.x - 1 >= 0 && prev_pos.y - 1 >= 0 && grid[prev_pos.x - 1][prev_pos.y - 1] == 0) {
		grid[prev_pos.x - 1][prev_pos.y - 1] = weight + 1;
		left.push_back(sf::Vector2i(prev_pos.x - 1, prev_pos.y - 1));
	}

	if ( prev_pos.x + 1 < rows && prev_pos.y - 1 >= 0 && grid[prev_pos.x + 1][prev_pos.y - 1] == 0) {
		grid[prev_pos.x + 1][prev_pos.y - 1] = weight + 1;
		left.push_back(sf::Vector2i(prev_pos.x + 1, prev_pos.y - 1));
	}

	if ( prev_pos.x - 1 >= 0 && prev_pos.y + 1 < cols && grid[prev_pos.x - 1][prev_pos.y + 1] == 0) {
		grid[prev_pos.x - 1 ][prev_pos.y + 1] = weight + 1;
		left.push_back(sf::Vector2i(prev_pos.x - 1, prev_pos.y + 1));
	}

	generateHCost_helper(left);
		
}

void Pathfinder::print(void) {
	std::cout << rows << " " << cols << std::endl;
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

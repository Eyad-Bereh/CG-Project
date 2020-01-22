#ifndef __MAP_H__
#define __MAP_H__

#include <string>
#include <map>
#include "Point.h"
#define TILESIZE 2

class Map {
private:
	int **map;
	int size;

public:
	static const int EMPTY = 0;
	static const int OBSTACLE = 1;
	static const int COIN = 2;
	static const int SNAKE = 3;

	Map();
	Map(int);
	std::string ToString();
	int &operator()(int, int);

};

#endif
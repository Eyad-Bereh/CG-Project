#ifndef __SNAKE_H__
#define __SNAKE_H__

#include <vector>
#include <queue>
#include "Point.h"

class Snake {
private:
	Point head;
	std::vector<Point> body;

public:
	Snake();
	Snake(int, int);
	Point GetHead();
	void SetHead(int, int);
	int GetCount();
	void InsertBody(int, int);
};

#endif
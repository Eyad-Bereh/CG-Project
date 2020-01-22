#ifndef __POINT_H__
#define __POINT_H__

#include <string>
#include <cstring>
#include <array>

class Point {
public:
	int X;
	int Y;
	int Z;

	Point();
	Point(int, int, int);
	std::string ToString();
	static Point FromString(std::string);
	static Point FromString(char*);
};

#endif
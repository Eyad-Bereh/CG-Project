#include "Point.h"
#include "StringSplitter.h"

Point::Point() {
	this->X = 0;
	this->Y = 0;
	this->Z = 0;
}

Point::Point(int x, int y, int z) {
	this->X = x;
	this->Y = y;
	this->Z = z;
}

std::string Point::ToString() {
	std::string result = "";
	result += std::to_string(this->X);
	result += ":";
	result += std::to_string(this->Y);
	result += ":";
	result += std::to_string(this->Z);
	return result;
}

Point Point::FromString(std::string str) {
	std::array<std::string, 3> temp = Split(str, ':');
	Point P;
	P.X = std::stod(temp[0]);
	P.Y = std::stod(temp[1]);
	P.Z = std::stod(temp[2]);
	return P;
}

Point Point::FromString(char* str) {
	std::string input = str;
	std::array<std::string, 3> temp = Split(input, ':');
	Point P;
	P.X = std::stod(temp[0]);
	P.Y = std::stod(temp[1]);
	P.Z = std::stod(temp[2]);
	return P;
}
#include "Map.h"
#define TILESIZE 2

Map::Map() {}

Map::Map(int size) {
	this->size = size * 2;
	this->map = new int*[this->size];
	int x = 0, z = 0;
	Point Points[4];
	for (int i = 0; i < this->size; i++) {
		z = 0;
		this->map[i] = new int[this->size];
		for (int j = 0; j < this->size; j++) {
			this->map[i][j] = Map::EMPTY;
			z += 2;
		}
		x += 2;
	}
}

std::string Map::ToString() {
	std::string result = "";
	for (int i = 0; i < this->size; i++) {
		for (int j = 0; j < this->size; j++) {
			result += std::to_string(this->map[i][j]);
			result += "   ";
		}
		result += "\n";
	}
	return result;
}

int &Map::operator()(int x, int z) {
	return this->map[x][z];
}
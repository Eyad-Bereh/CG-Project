#include "Snake.h"

Snake::Snake() {
	this->head.X = 0;
	this->head.Y = 1;
	this->head.Z = 0;

	this->body = std::vector<Point>();
}

Snake::Snake(int x, int z) {
	this->head.X = x;
	this->head.Y = 1;
	this->head.Z = z;
	this->body = std::vector<Point>();
}

Point Snake::GetHead() {
	return this->head;
}

void Snake::SetHead(int x, int z) {
	this->head.X = x;
	this->head.Z = z;


}

int Snake::GetCount() {
	return this->body.size();
}

void Snake::InsertBody(int x, int z) {
	this->body.push_back({x, 1, z});
}
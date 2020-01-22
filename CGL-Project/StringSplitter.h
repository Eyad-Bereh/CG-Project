#ifndef __STRING_SPLITTER_H__
#define __STRING_SPLITTER_H__

#include <string>
#include <sstream>
#include <array>

std::array<std::string, 3> Split(const std::string &str, char delimiter) {
	std::array<std::string, 3> result;
	std::stringstream ss(str);
	std::string token;
	int c = 0;
	while (std::getline(ss, token, delimiter)) {
		result[c] = token;
		c++;
	}
	return result;
}

#endif
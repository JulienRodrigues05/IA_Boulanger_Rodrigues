#pragma once

#include "Globals.h"
#include <vector>

class Vertex {
public:
	Vertex(STileInfo t) : myTile {t}{
	};

	~Vertex() = default;

	STileInfo myTile;
	std::vector<Vertex*> neighboors{};

	bool isNeighboor(const Vertex&, const SObjectInfo*, const int);

	bool operator==(const Vertex& v) const noexcept {
		return this->myTile == v.myTile;
	}



	static bool isObstructedWay(const STileInfo& curr, const STileInfo& next, const SObjectInfo* objectInfoArray, const int objectInfoArraySize);
};
#include "Vertex.h"
#include "Globals.h"

//	Fonction : obstructedWay
//
//	Indique s'il y a un objet qui obstrue le chemin d'une tuile à une autre
//
bool Vertex::isObstructedWay(const STileInfo& curr, const STileInfo& next, const SObjectInfo* objectInfoArray, const int objectInfoArraySize) {
	bool obstrue = false;
	EHexCellDirection dir = curr.getDirection(next);
	EHexCellDirection revDir = next.getDirection(curr);
	for (auto p = objectInfoArray + 0; p != objectInfoArray + objectInfoArraySize; ++p) {
		bool objOnCurr = (*p).q == curr.q && (*p).r == curr.r && (*p).cellPosition == dir;
		bool objOnNext = (*p).q == next.q && (*p).r == next.r && (*p).cellPosition == revDir;
		if (objOnCurr || objOnNext) {
			obstrue = true;
			break;
		}
	}

	return obstrue;
}


//	Fonction : isNeighboor
//
//	Indique si le second vertex est un voisin
bool Vertex::isNeighboor(const Vertex& v, const SObjectInfo* objectInfoArray, const int objectInfoArraySize) {
	STileInfo tile = this->myTile;
	STileInfo tile_ = v.myTile;

	bool neighboor = tile.isNeighboor(tile_);
	bool obstructed = isObstructedWay(tile, tile_, objectInfoArray, objectInfoArraySize);

	return neighboor && !obstructed;
}
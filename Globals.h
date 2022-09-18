#pragma once

enum EHexCellType
{
	Goal,
	Forbidden,
	Default
};

enum EHexCellDirection
{
	W, NW, NE, E, SE, SW, CENTER
};

enum EObjectType
{
	Wall,
	Window,
	Door,
	PressurePlate
};

enum EObjectState
{
	Opened,
	Closed
};

enum EOrderType
{
	Move,
	Interact
};

enum EInteractionType
{
	OpenDoor,
	CloseDoor,
	SearchHiddenDoor,
};

struct SOrder
{
	EOrderType orderType;
	int npcUID;
	EHexCellDirection direction;
	int objectUID;
	EInteractionType interactionType;
};

struct STileInfo
{
	int q;
	int r;
	EHexCellType type;

	//	Fonction : getDirection
	//
	//	Retourne la direction qu'il faut suivre pour se déplacer d'une case à une autre
	//
	EHexCellDirection getDirection(const STileInfo& next) const noexcept {
		EHexCellDirection direction{};

		if (next.q == q + 1 && next.r == r - 1)
			direction = SW;
		else if (next.q == q + 1 && next.r == r)
			direction = SE;
		else if (next.q == q && next.r == r + 1)
			direction = E;
		else if (next.q == q - 1 && next.r == r + 1)
			direction = NE;
		else if (next.q == q - 1 && next.r == r)
			direction = NW;
		else if (next.q == q && next.r == r - 1)
			direction = W;

		return direction;
	}

	//	Fonction : isNeighboor
	//
	//	Indique si la tuile est voisine
	//
	bool isNeighboor(const STileInfo& tile_) const noexcept {
		bool ne = tile_.q == q + 1 && tile_.r == r - 1;
		bool e = tile_.q == q + 1 && tile_.r == r;
		bool se = tile_.q == q && tile_.r == r + 1;
		bool sw = tile_.q == q - 1 && tile_.r == r + 1;
		bool w = tile_.q == q - 1 && tile_.r ==r;
		bool nw = tile_.q == q && tile_.r == r - 1;

		bool neighboor = ne || e || se || sw || w || nw;
		bool forbidden = tile_.type == Forbidden;

		return neighboor && !forbidden;
	}

	//	Fonction : operator==
	//
	//	Indique si deux STileInfo sont égaux
	//
	bool operator==(const STileInfo& tile2) const noexcept {
		return r == tile2.r && q == tile2.q && type == tile2.type;
	}

	//	Fonction : operator!=
	//
	//	Indique si deux STileInfo sont différents
	//
	bool operator!=(const STileInfo& tile2) const noexcept {
		return !(operator==(tile2));
	}
};

struct SObjectInfo
{
	int uid;
	int q;
	int r;
	EHexCellDirection cellPosition;

	int* types;
	int typesSize;

	int* states;
	int statesSize;

	int* connectedTo;
	int connectedToSize;
};

struct SNPCInfo
{
	int uid;
	int q;
	int r;
	int visionRange;

	//	Fonction : getTileFromNPC
	//
	//	Retourne la case associer à la position du NPC
	//
	STileInfo getTileFromNPC() const noexcept {
		STileInfo tile{};
		tile.q = q;
		tile.r = r;
		tile.type = Default;
		return tile;
	}
};

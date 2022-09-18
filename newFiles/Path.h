#pragma once

#include "Vertex.h"

class Path {
public:
	enum State { TERMINE, NON_DEMARRE, EN_COURS };

	std::list<Vertex*> way;

	State state{ NON_DEMARRE };

	//	Fonction : operator==
	//
	//	Indique si deux SWay sont �gaux
	//
	bool operator==(const Path& way) {
		if (this->way.size() == way.way.size()) {
			bool tousEgaux = true;
			auto it1 = begin(this->way);
			auto it2 = begin(way.way);
			for (; it1 != end(this->way) && it2 != end(way.way); ++it1, ++it2)
				tousEgaux &= *it1 == *it2;
			return tousEgaux;
		}

		return this->way == way.way && this->state == way.state;
	}

	//	Fonction : lastVertex
	//
	//	Retourne le dernier vertex de la liste
	//
	Vertex* lastVertex() {
		return this->way.back();
	}
};
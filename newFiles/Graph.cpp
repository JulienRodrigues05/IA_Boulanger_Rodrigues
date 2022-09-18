#include "Graph.h"
#include "Vertex.h"
#include "Logger.h"
#include <vector>
#include <algorithm>
#include <string>
#include <MyBotLogic.h>

//using namespace std;

//	Constructeur
//
//	Construit le graphe en associant tous les sommets à leurs voisins
//
Graph::Graph(const SInitData& _initData) {
	this->vertex = *(new std::vector<Vertex*>());

	for (auto tile = _initData.tileInfoArray + 0; tile != _initData.tileInfoArray + _initData.tileInfoArraySize; ++tile) {
		// Créer un sommet associer à la tuile
		Vertex* v1 = new Vertex(*tile);

		// Ajouter le sommet dans le vecteur de sommet
		this->vertex.push_back(v1);

		// Associer ce sommet à tous ces voisins déjà présents dans le vecteur de sommet
		for (auto v2 = begin(this->vertex); v2 != end(this->vertex); ++v2) {
			if ((*v2)->isNeighboor(*v1, _initData.objectInfoArray, _initData.objectInfoArraySize)) {
				(*v2)->neighboors.push_back(v1);
				v1->neighboors.push_back(*v2);
			}
		}
	}
}

//	Fonction : getNeighbours
//
//	Retourne un tableau contenant toutes les cases voisines d'une case
//
void getNeighbours(const Vertex& v, const SInitData& _initData, std::vector<Vertex*>& neight) {
	STileInfo tile = v.myTile;

	SObjectInfo* objectInfoArray = _initData.objectInfoArray;
	int objectInfoArraySize = _initData.objectInfoArraySize;

	auto lambda = [&neight, &tile, &objectInfoArray, &objectInfoArraySize](STileInfo tile_) {
		bool neighboor = tile.isNeighboor(tile_);
		bool obstructed = Vertex::isObstructedWay(tile, tile_, objectInfoArray, objectInfoArraySize);

		if (neighboor && !obstructed) {
			Vertex* temp = new Vertex(tile_);
			neight.push_back(temp);
		}
	};

	std::for_each(_initData.tileInfoArray + 0, _initData.tileInfoArray + _initData.tileInfoArraySize, lambda);
}

//	Fonction : keepNonVisited
//
//	Retourne tous les sommets qui n'ont pas encore été visités
//
void Graph::keepNonVisited(std::vector<Vertex*>& vec) {
	std::vector<Vertex*>* myVertex = &this->vertex;
	auto pos = std::partition(begin(vec), end(vec),
		[&myVertex](Vertex* v) {
			return std::find_if(begin(*myVertex), end(*myVertex), 
				[&v](Vertex* vbis) {
					return v->myTile == vbis->myTile;
				}) != end(*myVertex);
		});
	vec.erase(begin(vec), pos);
}

//	Constructeur
//
//	Construit le graphe en associant tous les sommets accessible depuis une tuile donnée, à leurs voisins
//
Graph::Graph(const SInitData& _initData, const STileInfo& start) {
	STileInfo t = start;
	Vertex* first = new Vertex(t);

	// Obtenir les voisins de la première tuile
	std::vector<Vertex*> neighb{};
	getNeighbours(*first, _initData, neighb);

	// Associer les voisins au sommet
	first->neighboors = neighb;
	this->vertex.push_back(first);

	std::vector<Vertex*>* myVertex = &this->vertex;

	// Tant qu'il reste des voisins non visités, continuer à en rechercher d'autres
	while (neighb.size() > 0) {

		std::vector<Vertex*> newN = neighb;

		// Associer à chaque nouveaux voisins leurs voisins
		auto lambda = [&myVertex, &newN, &_initData](Vertex* v) {
			// Obtenir les voisins de la tuile
			std::vector<Vertex*> nb{};
			getNeighbours(v->myTile, _initData, nb);

			// Associer les voisins au sommet
			v->neighboors = nb;

			// Ajouter le sommet dans le vecteur de sommet
			myVertex->push_back(v);

			auto lambda = [&newN, &myVertex](Vertex* v) {
				if (find_if(begin(newN), end(newN), [&v](Vertex* vbis) {return v->myTile == vbis->myTile; }) == end(newN) &&
					find_if(begin(*myVertex), end(*myVertex), [&v](Vertex* vbis) {return v->myTile == vbis->myTile; }) == end(*myVertex))
					newN.push_back(v);
			};

			std::for_each(begin(nb), end(nb), lambda);
		};

		std::for_each(begin(neighb), end(neighb), lambda);

		neighb = newN;

		this->keepNonVisited(neighb);
	}
}

//	Fonction : getGoals
//
//	Construit un vecteur de Vertex contenant toutes les arrivées
//
void Graph::getGoals(std::vector<Vertex*>& goals) const {
	std::for_each(begin(vertex), end(vertex), [&goals](Vertex* v) {if (v->myTile.type == Goal) goals.push_back(v); });
}

Vertex* Graph::getVertexFromTile(const STileInfo& tile) const {
	auto pos = std::find_if(begin(this->vertex), end(this->vertex), [&tile](Vertex* v) {return v->myTile == tile; });
	return *pos;
}
#pragma once

#include "Path.h"
#include "Globals.h"
#include "Vertex.h"
#include "Graph.h"

#include <vector>
#include <list>
#include <algorithm>
#include <string>

using namespace std;


class AStar {
public:
	//	Fonction : distance
	//
	//	Retourne la distance entre deux cases du damier en nombre de cases
	//
	static int distance(const STileInfo& first, const STileInfo& second) {
		int absq = abs(first.q - second.q);
		int absr = abs(first.r - second.r);
		int abss = abs(second.q + second.r - first.q - first.r);
		return (absq + absr + abss) / 2;
	}

	//	Fonction : sortGoals
	//
	//	Range les arriv�es en fonction de leur distance � une npc
	//
	static void sortGoals(const STileInfo& curr, vector<Vertex>& goals, const vector<Vertex>& goalsPicked, Logger& mLogger) {
		// Conserver que les arriv�es qui n'ont pas d�j� �taient choisies
		auto pos = partition(begin(goals), end(goals),
						[&goalsPicked](Vertex v) {
							return find(begin(goalsPicked), end(goalsPicked), v) == end(goalsPicked);
						});
		
		goals.erase(pos, end(goals));

		// Ranger les arriv�e dans l'ordre croissant des distances
		auto lambda = [&curr](Vertex tile, Vertex other) {
			return distance(curr, tile.myTile) < distance(curr, other.myTile);
		};
		sort(begin(goals), end(goals), lambda);

		/*for_each(begin(goals), end(goals),
			[&npc, &mLogger](Vertex tile) {BOT_LOGIC_LOG(mLogger, "ordere goals q = " + to_string(tile.myTile->q) + ", r = " + to_string(tile.myTile->r), true); });*/
	}


	//	Fonction : setNeighboursToListsStart
	//
	//	Place des les chemins autoris�s pour acc�der au voisins de la permi�re case dans la liste openWays
	//
	static void setNeighboursToListsStart(const SNPCInfo& npc, const Vertex& goal, vector<Path>& openWays, const Graph& myGraph, Logger& mLogger) {
		STileInfo tile = npc.getTileFromNPC();

		// Trouver les voisins de la case de d�part
		vector<Vertex> voisins = myGraph.getVertexFromTile(tile).neighboors;

		// Ranger les chemins dans openWays
		auto lambda = [&goal, &openWays, &mLogger](Vertex tile_) {
			if (!(tile_.myTile.type == Goal && !(tile_ == goal))) {
				//BOT_LOGIC_LOG(mLogger, "q = " + to_string(tile_.myTile.q) + ", r = " + to_string(tile_.myTile.r), true);

				Path nPath;
				nPath.way.push_back(tile_);
				openWays.push_back(nPath);
			}
		};
		//BOT_LOGIC_LOG(mLogger, "avant le for_each", true);
		std::for_each(begin(voisins), end(voisins), lambda);
		//BOT_LOGIC_LOG(mLogger, "apr�s le for_each", true);
	}

	//	Fonction : isGoalReached
	//
	//	Indique si l'arriv�e a �t� atteinte, et place les chemins les plus courts en premiers
	//
	static bool isGoalReached(const Vertex& goal, vector<Path>& openWays, Logger& mLogger) {

		return partition(openWays.begin(),
			openWays.end(),
			[&goal, &mLogger](Path way) {
				Vertex v = way.lastVertex();
				//BOT_LOGIC_LOG(mLogger, "is goal reached q = " + to_string(v.myTile.q) + ", r = " + to_string(v.myTile.r), true);

				return v.myTile == goal.myTile;
			}) != openWays.begin();
	}

	// Fonction : getClosestTiles
	//
	//	Retourne le tableau contenant les sommets les plus proches de l'arriv�e pr�sentent dans la liste ouverte
	//
	static vector<Path> getClosestTiles(const Vertex& goal, vector<Path>& openWays, const int maxTurnNb, Logger& mLogger) {
		int i = -1;
		vector<Path> plusProches;
		vector<vector<Path>::iterator> aSupprimer;
		for (auto p = begin(openWays); p != end(openWays);) {
			// Si le chemin est trop long, on l'abandonne
			if (distance((*p).lastVertex().myTile, goal.myTile) + (*p).way.size() > maxTurnNb) {
				//BOT_LOGIC_LOG(mLogger, "on en a supprimer un", true);
				p = openWays.erase(p);
			}
			else {
				if (i == -1 || distance((*p).lastVertex().myTile, goal.myTile) == i) {
					plusProches.push_back(*p);
				}
				else if (distance((*p).lastVertex().myTile, goal.myTile) < i) {
					plusProches.clear();
					plusProches.push_back(*p);
				}
				++p;
			}
		}

		// S'il n'y a plus de chemin possible, lever une erreur pour changer de destination (arriv�e)
		if (openWays.size() == 0) throw 0;

		return plusProches;
	}

	//	Fonction : setNeighboursToLists
	//
	//	Place des les voisins d'une case dans les bonnes listes, openWays et closedTiles
	//
	static void setNeighboursToLists(Path& way, const Vertex& goal, vector<Path>& openWays, const Graph& myGraph, Logger& mLogger) {
		// Trouver les voisins de la case
		vector<Vertex> voisins = myGraph.getVertexFromTile(way.lastVertex().myTile).neighboors;

		// Retirer le chemin dont on vient de calculer tous les voisins de la liste des chemins possibles
		auto pos = find(begin(openWays), end(openWays), way);
		if (pos != end(openWays))
			openWays.erase(pos);

		// Ranger les chemins dans openWays
		auto lambda = [&goal, &openWays, &way, &mLogger](Vertex tile) {
			if (find(way.way.begin(), way.way.end(), tile) == way.way.end()) {
				if (!(tile.myTile.type == Goal && !(tile == goal))) {
					//BOT_LOGIC_LOG(mLogger, "q = " + to_string(tile.myTile.q) + ", r = " + to_string(tile.myTile.r), true);

					Path nPath;
					nPath.way = way.way; // Continuer le chemin
					nPath.way.push_back(tile);
					openWays.push_back(nPath);
				}
			}
		};
		std::for_each(begin(voisins), end(voisins), lambda);

	}

	//	Fonction : shortestWay
	//
	//	Retourne le chemin le plus court entre la case de d�part d'un npc et une arriv�e
	//
	static Path shortestWay(const SNPCInfo& npc, const Vertex& goal, const Graph& myGraph, const int maxTurnNb, Logger& mLogger) {
		// La liste des chemins ouvertes
		vector<Path> openWays;
		//BOT_LOGIC_LOG(mLogger, "setNeighboursToListsStart", true);

		// Ranger les voisins dans les listes correspondantes
		setNeighboursToListsStart(npc, goal, openWays, myGraph, mLogger);

		//BOT_LOGIC_LOG(mLogger, "isGoalReached", true);
		while (!isGoalReached(goal, openWays, mLogger)) {

			//BOT_LOGIC_LOG(mLogger, "getClosestTiles", true);
			vector<Path> casesPlusProches = getClosestTiles(goal, openWays, maxTurnNb, mLogger);
			//BOT_LOGIC_LOG(mLogger, "fin getClosestTiles nb chemin : " + to_string(casesPlusProches.size()), true);

			// Pour toutes cases les plus proches, ajouter leur voisins � la liste openWays
			auto lambda = [&goal, &openWays, &myGraph, &mLogger](Path way) {
				//BOT_LOGIC_LOG(mLogger, "setNeighboursToLists", true);
				setNeighboursToLists(way, goal, openWays, myGraph, mLogger);
			};

			std::for_each(casesPlusProches.begin(), casesPlusProches.end(), lambda);
		}

		// Retourne un des chemins possibles, a �t� mis en premier par isGoalReached()
		Path bestWay = openWays.front();
		for_each(begin(bestWay.way), end(bestWay.way),
			[&npc, &mLogger](Vertex tile) {BOT_LOGIC_LOG(mLogger, "npc#" + to_string(npc.uid) + "ordere q = " + to_string(tile.myTile.q) + ", r = " + to_string(tile.myTile.r), true); });

		return bestWay;
	}

	static void findBestPath(Path& path, const SNPCInfo& npc, vector<Vertex>& goals, vector<Vertex>& goalsPicked, const Graph& myGraph, const int maxTurnNb, Logger& mLogger) {
		// Classer les arriv�es selon leur distance � la tuile courante
		AStar::sortGoals(npc.getTileFromNPC(), goals, goalsPicked, mLogger);
		BOT_LOGIC_LOG(mLogger, "arriv�es rang�es, nb = " + to_string(goals.size()), true);

		for (auto goal = begin(goals); goal != end(goals); ++goal) {
			try {
				BOT_LOGIC_LOG(mLogger, "Goal picked : q = " + to_string((*goal).myTile.q) + ", r = " + to_string((*goal).myTile.r), true);
				// Trouver le plus court chemin menant � l'arriv�e pr�c�demment choisie
				Path way = AStar::shortestWay(npc, *goal, myGraph, maxTurnNb, mLogger);

				// Ajouter l'arriv�e � la liste des arriv�es d�j� choisies
				goalsPicked.push_back(*goal);

				// Associer le chemin au npc
				path = way;
				path.state = Path::EN_COURS;
				break;
			}
			catch (int e) {
				continue;
			}
		}
	}
};
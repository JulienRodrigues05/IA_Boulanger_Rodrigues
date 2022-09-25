#include "MyBotLogic.h"

#include "Globals.h"
#include "ConfigData.h"
#include "InitData.h"
#include "TurnData.h"
#include "newFiles/Graph.h"
#include "newFiles/Vertex.h"
#include "newFiles/Path.h"
#include "newFiles/AStar.h"
#include "newFiles/DataNPC.h"

#include <string>
#include <algorithm>
#include <iterator>
#include <map>
#include <vector>

using namespace std;


MyBotLogic::MyBotLogic()
{
	//Write Code Here

}

MyBotLogic::~MyBotLogic()
{
	//Write Code Here
}

void MyBotLogic::Configure(const SConfigData& _configData)
{
#ifdef BOT_LOGIC_DEBUG
	mLogger.Init(_configData.logpath, "MyBotLogic.log");
#endif

	BOT_LOGIC_LOG(mLogger, "Configure", true);

	//Write Code Here
}

void MyBotLogic::Init(const SInitData& _initData)
{
	BOT_LOGIC_LOG(mLogger, "Init", true);
	
	//Write Code Here

	maxTurnNb = _initData.maxTurnNb;

	myGraph = Graph(_initData);

	for (int i = 0; i < _initData.nbNPCs; ++i)
	{
		auto& npc = _initData.npcInfoArray[i];

		STileInfo curr = npc.getTileFromNPC();
		tileWithNPCOn.push_back(curr);

		//DataNPC d = DataNPC(Path{}, Graph(_initData, curr));
		DataNPC d = DataNPC(Path{});

		d.firstNeighboors = myGraph.getVertexFromTile(curr).neighboors;

		this->mapNpcDatas[npc.uid] = d;
	}

}

void MyBotLogic::GetTurnOrders(const STurnData& _turnData, std::list<SOrder>& _orders)
{
	BOT_LOGIC_LOG(mLogger, "GetTurnOrders", true);

	struct utils {

		//	Fonction : getOrderMove
		//
		//	Retourne un ordre de déplacement vers un autre Tile
		//
		static SOrder getOrderMove(const int uid, const STileInfo& current, const STileInfo& next, list<SOrder>& _orders, Logger& mLogger) {
			BOT_LOGIC_LOG(mLogger, "setOrderMove", true);
			SOrder order{};
			order.npcUID = uid;
			order.orderType = Move;
			order.direction = current.getDirection(next);

			return order;
		}

		//	Fonction : addOrder
		//
		//	Ajoute un ordre d'action dans liste pour chaque npc
		//
		static void addOrder(const SNPCInfo& npc, vector<STileInfo>& tileWithNPCOn, Path& way, list<SOrder>& _orders, Logger& mLogger) {
			BOT_LOGIC_LOG(mLogger, "addOrder", true);

			for_each(begin(tileWithNPCOn), end(tileWithNPCOn),
				[&mLogger](STileInfo tile) {BOT_LOGIC_LOG(mLogger, "tile with npc on q = " + to_string(tile.q) + ", r = " + to_string(tile.r), true); });

			
			STileInfo current = npc.getTileFromNPC();
			STileInfo next = way.way.front().myTile;

			BOT_LOGIC_LOG(mLogger, "next q = " + to_string(next.q) + ", r =" + to_string(next.r), true);
				
			// Aucun npc n'est déjà présent sur la case
			if (find(begin(tileWithNPCOn), end(tileWithNPCOn), next) == end(tileWithNPCOn)) {
				SOrder order = getOrderMove(npc.uid, current, next, _orders, mLogger);

				_orders.push_back(order);
				tileWithNPCOn.push_back(next);

				way.way.pop_front();
		
				// Changer l'état du chemin / npc s'il y a lieu
				if (way.way.size() == 0) {
					BOT_LOGIC_LOG(mLogger, "way of npc number : " + to_string(npc.uid) + " is ended", true);
					BOT_LOGIC_LOG(mLogger, "last tile is : q = " + to_string(next.q) + ", r = " + to_string(next.r), true);
					if (next.type == Goal)
						way.state = Path::TERMINE;
					else
						way.state = Path::INDECIS;
				}
						

				tileWithNPCOn.erase(find(begin(tileWithNPCOn), end(tileWithNPCOn), current));
			}
		}
	};

	//Write Code Here

	

	for (int i = 0; i < _turnData.npcInfoArraySize; ++i) {
		SNPCInfo& npc = _turnData.npcInfoArray[i];

		Path& path = mapNpcDatas[npc.uid].myPath;
		Graph& graph = myGraph;
		vector<Vertex>& firstN = mapNpcDatas[npc.uid].firstNeighboors;
		//vector<Vertex>& nextN = mapNpcDatas[npc.uid].nextNeighboors;
		vector<Vertex>& nextN = toVisit;
		vector<Vertex>& alreadyV = alreadyVisited;
		STileInfo curr = npc.getTileFromNPC();
		vector<Vertex> goals;

		if (path.state != Path::TERMINE) {

			alreadyV.push_back(graph.getVertexFromTile(curr));
			// Retirer le voisin de liste des voisins inexplorés
			auto pos = find(begin(nextN), end(nextN), graph.getVertexFromTile(curr));
			if (pos != end(nextN)) {
				nextN.erase(pos);
			}
			

			BOT_LOGIC_LOG(mLogger, "npc " + to_string(npc.uid) + " is looking for his path ", true);

			if (path.state != Path::NON_DEMARRE) {
				BOT_LOGIC_LOG(mLogger, "npc " + to_string(npc.uid) + " is updating", true);
				// Mettre à jour le graph
				graph.update(_turnData);

				// Ajouter les nouveaux voisins à la liste
				BOT_LOGIC_LOG(mLogger, "nombre already = " + to_string(alreadyV.size()), true);
				for_each(begin(alreadyV), end(alreadyV),
					[&](Vertex tile) {BOT_LOGIC_LOG(mLogger, "already present q = " + to_string(tile.myTile.q) + ", r = " + to_string(tile.myTile.r), true); });
				for (int i = 0; i < graph.getVertexFromTile(curr).neighboors.size(); ++i) {
					Vertex v = graph.getVertexFromTile(curr).neighboors[i];
					if (find_if(begin(nextN), end(nextN), [&v](Vertex vbis) { return v.myTile == vbis.myTile; }) == end(nextN) &&
							find(begin(alreadyV), end(alreadyV), v) == end(alreadyV)) {
						BOT_LOGIC_LOG(mLogger, "On ajoutea : q = " + to_string(v.myTile.q) + ", r = " + to_string(v.myTile.r), true);
						nextN.push_back(v);
					}
				}
			}

			// Si le npc n'a pas de chemin à suivre
			if (path.state != Path::EN_COURS || !path.lastVertex().myTile.type == Goal) {
				BOT_LOGIC_LOG(mLogger, "npc " + to_string(npc.uid) + " is lost so he looks for a way", true);

				graph.getGoals(goals);

				// Il y a une arrivée dans le graphe
				if (!goals.empty()) {
					// A* normal
					AStar::findBestPath(path, npc, goals, goalsPicked, graph, maxTurnNb - _turnData.turnNb + 1, mLogger);

					if (path.way.empty() && path.state != Path::EN_COURS)
						path.state = Path::INDECIS;
				}
				else if (path.state != Path::EN_COURS)
					path.state = Path::INDECIS;
			}


			if (path.state == Path::EN_COURS) {
				BOT_LOGIC_LOG(mLogger, "npc " + to_string(npc.uid) + " follows his path", true);
				utils::addOrder(npc, tileWithNPCOn, path, _orders, mLogger);
			}
			// Si le npc est indécis, il faut lui trouver une destination
			else if (path.state == Path::INDECIS) {
				// S'il n'a pas visité tous ses premiers voisins, il le fait
				if (!firstN.empty()) {
					BOT_LOGIC_LOG(mLogger, "npc " + to_string(npc.uid) + " explores his closest neighboors", true);
					// Trouver le chemin pour aller au voisin
					if (find(begin(tileWithNPCOn), end(tileWithNPCOn), firstN.back().myTile) == end(tileWithNPCOn)) {
						Path way = AStar::shortestWay(npc, firstN.back(), graph, maxTurnNb - _turnData.turnNb + 1, mLogger);

						// Associer le chemin au npc
						path = way;
						path.state = Path::EN_COURS;

						// Retirer le voisin de liste des voisins inexplorés
						alreadyV.push_back(firstN.back());
						firstN.pop_back();
					}
					else {
						if (find(begin(goalsPicked), end(goalsPicked), firstN.back()) != end(goalsPicked))
							firstN.pop_back();
					}
					
				}
				// Sinon il se déplace vers un autre voisin le plus proche
				else {
					vector<Path> ways;
					for (auto neihb = begin(nextN); neihb != end(nextN); ++neihb) {
						if (find(begin(tileWithNPCOn), end(tileWithNPCOn), neihb->myTile) == end(tileWithNPCOn)) {
							try {
								BOT_LOGIC_LOG(mLogger, "neihb picked : q = " + to_string((*neihb).myTile.q) + ", r = " + to_string((*neihb).myTile.r), true);
								// Trouver le plus court chemin menant au voisin précédemment choisi
								Path way = AStar::shortestWay(npc, *neihb, graph, maxTurnNb - _turnData.turnNb + 1, mLogger);

								// Associer le chemin au npc
								way.state = Path::EN_COURS;
								ways.push_back(way);
								/*path = way;
								break;*/
							}
							catch (int e) {
								continue;
							}
						}
						else BOT_LOGIC_LOG(mLogger, "il y a deja quelqu'un : q = " + to_string(neihb->myTile.q) + ", r = " + to_string(neihb->myTile.r), true);
					}
					
					// Prendre le plus court chemin
					int size = -1;
					BOT_LOGIC_LOG(mLogger, "nombre de chemin " + to_string(ways.size()), true);
					for (auto p = begin(ways); p != end(ways); ++p) {
						BOT_LOGIC_LOG(mLogger, "taille du chemin " + to_string(p->way.size()), true);
						if (size == -1 || p->way.size() <= size) {
							BOT_LOGIC_LOG(mLogger, "c'est un bon chemin", true);
							size = p->way.size();
							path = *p;
						}
					}

				}

				if (path.way.empty())
					path.state = Path::INDECIS;
				else
					// Puis s'y déplacer
					utils::addOrder(npc, tileWithNPCOn, path, _orders, mLogger);
			}

		}
	}
	BOT_LOGIC_LOG(mLogger, "fin GetTurnOrder", true);
}
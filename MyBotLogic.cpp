#include "MyBotLogic.h"

#include "Globals.h"
#include "ConfigData.h"
#include "InitData.h"
#include "TurnData.h"
#include "newFiles/Graph.h"
#include "newFiles/Vertex.h"
#include "newFiles/Path.h"
#include "newFiles/AStar.h"

#include <string>
#include <algorithm>
#include <iterator>
#include <map>
#include <vector>

using namespace std;


MyBotLogic::MyBotLogic()
{
	//Write Code Here

	std::map<int, Path> mapNpcPaths {};

	std::vector<STileInfo> tileWithNPCOn {};
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

	// Obtenir le graphe en partant de la case de départ du npc
	//Graph* myGraph = new Graph(_initData, (*npc).getTileFromNPC(), mLogger);
	Graph* myGraph = new Graph(_initData);

	vector<Vertex*> goalsPicked{};
	
	for (SNPCInfo* npc = _initData.npcInfoArray + 0; npc != _initData.npcInfoArray + _initData.nbNPCs; ++npc) {
	 
	 	BOT_LOGIC_LOG(mLogger, "npc numero : " + to_string(npc->uid), true);
	 
		
		BOT_LOGIC_LOG(mLogger, "graph créét", true);

		// Obtenir toutes les arrivées accessibles
		vector<Vertex*> goals;
		myGraph->getGoals(goals);
		BOT_LOGIC_LOG(mLogger, "arrivées récupérées " + to_string(goals.size()), true);
	 
		// Classer les arrivées selon leur distance à la tuile départ
		AStar::sortGoals(*npc, goals, goalsPicked, mLogger);
		BOT_LOGIC_LOG(mLogger, "arrivées rangées, nb = " + to_string(goals.size()), true);

		for (auto goal = begin(goals); goal != end(goals); ++goal) {
			try {
				BOT_LOGIC_LOG(mLogger, "Goal picked : q = " + to_string((*goal)->myTile.q) + ", r = " + to_string((*goal)->myTile.r), true);
				// Trouver le plus court chemin menant à l'arrivée précédemment choisie
				Path way = AStar::shortestWay(*npc, **goal, *myGraph, _initData.maxTurnNb, mLogger);

				// Ajouter l'arrivée à la liste des arrivées déjà choisies
				goalsPicked.push_back(*goal);

				// Associer le chemin au npc
				mapNpcPaths[npc->uid] = way;
				break;
			}
			catch (int e) {
				continue;
			}
		}
	}



}

void MyBotLogic::GetTurnOrders(const STurnData& _turnData, std::list<SOrder>& _orders)
{
	BOT_LOGIC_LOG(mLogger, "GetTurnOrders", true);

	struct utils {

		//	Fonction : initTileOccupied
		//
		//	Initialise la liste des Tiles avec la position courante de tous les npc
		//
		static vector<STileInfo> initTileOccupied(const STurnData& _turnData, Logger& mLogger) {
			vector<STileInfo> tiles{};

			auto lambda = [&tiles,&mLogger](SNPCInfo npc) {BOT_LOGIC_LOG(mLogger, "q = " + to_string(npc.q) + ", r =" + to_string(npc.r), true);
				tiles.push_back(npc.getTileFromNPC()); };
			std::for_each(_turnData.npcInfoArray + 0, _turnData.npcInfoArray + _turnData.npcInfoArraySize, lambda);

			return tiles;
		}

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
		static void addOrder(const SNPCInfo& npc, vector<STileInfo>& tileWithNPCOn, map<int, Path>& mapNpcWays, list<SOrder>& _orders, Logger& mLogger) {
			BOT_LOGIC_LOG(mLogger, "addOrder", true);
			Path way = mapNpcWays[npc.uid];

			if (way.state != Path::TERMINE) {
				STileInfo current = npc.getTileFromNPC();
				STileInfo next = way.way.front()->myTile;

				BOT_LOGIC_LOG(mLogger, "next q = " + to_string(next.q) + ", r =" + to_string(next.r), true);

				int t = static_cast<int>(find(begin(tileWithNPCOn), end(tileWithNPCOn), next) - end(tileWithNPCOn));
				BOT_LOGIC_LOG(mLogger, "distance à la fin : " + to_string(t), true);
				
				// Aucun npc n'est déjà présent sur la case
				if (find(begin(tileWithNPCOn), end(tileWithNPCOn), next) == end(tileWithNPCOn)) {
					BOT_LOGIC_LOG(mLogger, "ncp libre", true);
					SOrder order = getOrderMove(npc.uid, current, next, _orders, mLogger);

					_orders.push_back(order);
					tileWithNPCOn.push_back(next);

					way.way.pop_front();
		
					if (way.way.size() == 0)
						way.state = Path::TERMINE;
					mapNpcWays[npc.uid] = way;
					tileWithNPCOn.erase(find(begin(tileWithNPCOn), end(tileWithNPCOn), current));
				}
			}
		}
	};

	//Write Code Here

	BOT_LOGIC_LOG(mLogger, "initTileOccupied", true);
	this->tileWithNPCOn = utils::initTileOccupied(_turnData, mLogger);
	BOT_LOGIC_LOG(mLogger, "for_each", true);

	/*auto lambda = [&tileWithNPCOn, &_orders](SNPCInfo npc) {utils::addOrder(npc, tileWithNPCOn, _orders); };
	std::for_each(_turnData.npcInfoArray + 0, _turnData.npcInfoArray + _turnData.npcInfoArraySize, lambda);*/
	for (auto p = _turnData.npcInfoArray + 0; p != _turnData.npcInfoArray + _turnData.npcInfoArraySize; ++p) {
		utils::addOrder(*p, tileWithNPCOn, this->mapNpcPaths, _orders, mLogger);
	}
	
}
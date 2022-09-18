#pragma once

#include "BotLogicIF.h"
#include "Logger.h"
#include "newFiles/Path.h"

#include <map>
#include <list>

#ifdef _DEBUG
#define BOT_LOGIC_DEBUG
#endif

#ifdef BOT_LOGIC_DEBUG
#define BOT_LOGIC_LOG(logger, text, autoEndLine) logger.Log(text, autoEndLine)
#else
#define BOT_LOGIC_LOG(logger, text, autoEndLine) 0
#endif

struct SConfigData;
struct STurnData;

//Custom BotLogic where the AIBot decision making algorithms should be implemented.
//This class must be instantiated in main.cpp.
class MyBotLogic : public virtual BotLogicIF
{
public:
	MyBotLogic();
	virtual ~MyBotLogic();

	virtual void Configure(const SConfigData& _configData);
	virtual void Init(const SInitData& _initData);
	virtual void GetTurnOrders(const STurnData& _turnData, std::list<SOrder>& _orders);

protected:
	Logger mLogger;

	// Map de clef uid npc, et valeur un chemin de type SWay
	std::map<int, Path> mapNpcPaths;

	// Vecteur contenant la position des NPC à tout instant
	std::vector<STileInfo> tileWithNPCOn;
};
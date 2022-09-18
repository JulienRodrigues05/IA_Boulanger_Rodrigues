#pragma once

#include "Globals.h"
#include "InitData.h"
#include "Vertex.h"
#include "Logger.h"

class Graph {
public:
	Graph(const SInitData&);
	Graph(const SInitData&, const STileInfo&);
	~Graph() = default;

	std::vector<Vertex*> vertex;

	void getGoals(std::vector<Vertex*>&) const;

	Vertex* getVertexFromTile(const STileInfo&) const;

private:
	void keepNonVisited(std::vector<Vertex*>&);
};
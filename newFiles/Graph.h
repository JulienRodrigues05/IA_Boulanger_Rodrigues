#pragma once

#include "Globals.h"
#include "InitData.h"
#include "Vertex.h"
#include "Logger.h"
#include "TurnData.h"

class Graph {
public:
	Graph(const SInitData&);
	Graph(const SInitData&, const STileInfo&);
	Graph() = default;
	Graph(const Graph&);
	~Graph() = default;

	Graph& operator=(const Graph&);

	std::vector<Vertex> vertex;

	void getGoals(std::vector<Vertex>&) const;

	Vertex getVertexFromTile(const STileInfo&) const;

	void update(const STurnData&);

private:
	void keepNonVisited(std::vector<Vertex>&);
};
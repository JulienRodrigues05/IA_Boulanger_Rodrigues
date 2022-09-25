#pragma once

#include "newFiles/Path.h"
#include "newFiles/Graph.h"

#include <vector>

struct DataNPC {
public:
	DataNPC(Path& p, Graph& g) : myPath( p ), myGraph( g ) 
	{
	}
	DataNPC() = default;
	DataNPC(const DataNPC& d) 
		: myPath(d.myPath), myGraph(d.myGraph),
		  firstNeighboors(d.firstNeighboors),
		nextNeighboors(d.nextNeighboors),
		alreadyVisited(d.alreadyVisited) 
	{
	}

	Graph myGraph{};
	Path myPath{};

	std::vector<Vertex> firstNeighboors{};
	std::vector<Vertex> nextNeighboors{};
	std::vector<Vertex> alreadyVisited{};

	DataNPC& operator=(const DataNPC& n) {
		this->myGraph = n.myGraph;
		this->myPath = n.myPath;
		this->firstNeighboors = n.firstNeighboors;
		this->nextNeighboors = n.nextNeighboors;
		this->alreadyVisited = n.alreadyVisited;
		return *this;
	}
};
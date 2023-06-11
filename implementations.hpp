#ifndef IMPLEMENTATIONS_HPP_CHECK
#define IMPLEMENTATIONS_HPP_CHECK

#include <iostream>
#include <vector>
#include <array>
#include <algorithm>
#include "render.hpp"

constexpr bool DEBUG = false;

// returns a variable amount of nodes that are closest to the genesis node
std::vector<Node> closestNodes(
	const std::vector<Node>& nodes,
	const Node& genesisNode,
	uint16_t nodeAmount,
	const std::vector<Node>& illegal = {}
) {
	// holds [nodeAmount] of the closest nodes
	std::vector<Node> toReturn {};
	// closest nodes
	// fill it with the first [nodeAmount] nodes just so the logic works
	std::vector<std::pair<Node,float>> closestNodes {};
	bool still = false;
	for (uint16_t m = 0; m < nodeAmount; ++m) {
		if (nodes[m].id == genesisNode.id) {
			still = true;
			continue;
		}
		closestNodes.push_back({
			nodes[m],
			distance({genesisNode.X,genesisNode.Y},{nodes[m].X,nodes[m].Y})
		});
	}
	if (still) {
		closestNodes.push_back({
			nodes[nodeAmount],
			distance(
				{genesisNode.X,genesisNode.Y},
				{nodes[nodeAmount].X,nodes[nodeAmount].Y}
			)
		});
	}
	if (N_NODEAMOUNT <= 1) return toReturn;
	// loop through all nodes
	for (auto& i : nodes) {
		// dont compare to anything in illegal
		bool kill = false;
		for (auto& x : illegal) if (x.id == i.id) kill = true;
		if (kill) continue;
		// also dont compare to yourself idiot
		if (i.id == genesisNode.id) continue;
		
		// distance from genesis node to current node
		double dist = distance({genesisNode.X,genesisNode.Y},{i.X,i.Y});
		// check if the node is shorter than everything in toReturn
		for (auto& n : closestNodes) {
			if (dist < n.second) {
				// replace the largest node in closestNodes with the new node
				uint16_t largestInd = 0;
				float largestVal = 0;
				for (uint16_t m = 0; m < closestNodes.size(); ++m) {
					if (closestNodes[m].second > largestVal) {
						largestVal = closestNodes[m].second;
						largestInd = m;
					}
				}
				closestNodes[largestInd] = {i,dist};
				break;
			}
		}
	}
	for (auto& b : closestNodes) toReturn.push_back(b.first);
	return toReturn;
}

// returns the index of a node with a specific ID
constexpr inline uint16_t findId(const uint16_t& id) {
	for (uint16_t i = 0; i < N_NODES.size(); ++i) 
		if (N_NODES[i].id == id) 
			return i;
	// shit, its not in the node list
	// what now
	// pray to god it never gets to this
	return UINT16_MAX;
}

// returns a node from a node list given a specific ID
inline Node findNode(uint64_t id) {
	for (uint16_t i = 0; i < N_NODES.size(); ++i) 
		if (N_NODES[i].id == id) return N_NODES[i];
	
	// shit, its not in the node list
	// what now
	// pray to god it never gets to this
	return {};
}

// finds the total distance of a node path
double nodeDistanceTotal(std::vector<uint16_t> path) {
	double totalDistance = 0;
	for (uint16_t i = 0; i < path.size(); ++i) 
	{
		// make sure its not the last node
		if (i == path.size()-1) break;
		totalDistance += distance(
			{N_NODES[findId(path[i  ])].X, N_NODES[findId(path[i  ])].Y},
			{N_NODES[findId(path[i+1])].X, N_NODES[findId(path[i+1])].Y}
		);
	}
	return totalDistance;
}

std::vector<Node> subtractVec(
	std::vector<Node> vec1,
	const std::vector<Node>& vec2
) {
	// find items in vec2 and if they are in vec1, remove them
	// then return vec1
	for (Node i : vec2) {
		for (uint16_t c = 0; c < vec1.size(); ++c) {
			if (vec1[c].id == i.id) {
				vec1.erase(vec1.begin()+c);
				break;
			}
		}
	}
	return vec1;
}

// connects each node with the closest node
std::vector<uint16_t> IMP_quickPath(
	std::vector<Node> nodes,
	uint16_t variation = 0
) {
	std::vector<uint16_t> toReturn {};
	if (N_NODEAMOUNT <= 1) return toReturn;
	// holds the next node to connect to
	Node nextNode = nodes[0];
	
	while (1) {
		bool kill = true;
		for (uint16_t c = 0; c < nodes.size(); ++c) {
			// check if all nodes have been checked
			if (!nodes[c].check) {
				kill = false;
				break;
			}
		}
		if (kill) return toReturn;

		uint16_t closestNode;
		uint16_t closestNodeDistance = UINT16_MAX;
		
		for (uint16_t i = 0; i < nodes.size(); ++i) 
		{
			// check if the node is comparing something it shouldnt be
			if ((nodes[i].id == nextNode.id) || nodes[i].check) continue;
				
			// check if the node is closer than the next node
			double dist = distance({nextNode.X, nextNode.Y},{nodes[i].X, nodes[i].Y});
			if (variation) {
				if (dist+(rand()%variation) < closestNodeDistance+(rand()%variation)) {
					closestNode = nodes[i].id;
					closestNodeDistance = (uint16_t)dist;
				}
			}
			else {
				if (dist < closestNodeDistance) {
					closestNode = nodes[i].id;
					closestNodeDistance = (uint16_t)dist;
				}
			}
		}
		// found the right node! use it. 
		toReturn.push_back(closestNode);
		nextNode = nodes[closestNode];
		nodes[closestNode].check = true;
	}
}


// makes small shortest paths and puts them all together
std::vector<uint16_t> IMP_quickPath_branched(
	std::vector<Node> nodes,
	uint16_t branchAmount = 3,
	uint16_t branchLength = 3,
	uint16_t V1 = 0,
	uint16_t V2 = 0,
	uint16_t V3 = 0
) {
	if (DEBUG) std::cout << "\n! ";
	if (N_NODEAMOUNT <= 1) return {};
	
	Node current = nodes[0];
	std::vector<uint16_t> toReturn {current.id};
	std::vector<Node> globalIllegal {};

	while (1) {
		// are there enough nodes to search through?
		std::vector<Node> pool = subtractVec(nodes,globalIllegal);
		if (pool.size() < branchAmount*branchLength) {
			// if not, just connect the rest of the nodes
			std::vector<uint16_t> temp = IMP_quickPath(nodes);
			for (uint16_t i : temp) toReturn.push_back(i);
			return toReturn;
		}
		// find the closest nodes
		std::vector<Node> nodes_closest = closestNodes(nodes,current,branchAmount,globalIllegal);
		std::vector<std::vector<uint16_t>> paths_local {};
		for (auto i : nodes_closest) {
			std::vector<Node> localIllegal = globalIllegal;
			localIllegal.push_back(current);
			// generate the pool of nodes to path through
			std::vector<Node> nodes_closest_local = closestNodes(nodes,current,branchLength,localIllegal);
				/*std::cout << "\nCurrent: " << current.id << '\n';
				std::cout << "Local illegal: ";
				for (auto i : localIllegal) std::cout << i.id << ' ';
				std::cout << "\nLocal closest: ";
				for (auto i : nodes_closest_local) std::cout << i.id << ' ';
				std::cout << '\n';*/
			// generate the path
			paths_local.push_back(IMP_quickPath(nodes_closest_local,V3));
		}
		// find the shortest path
		double shortestPath = DBL_MAX;
		uint16_t shortestPathIndex = 0;
		for (uint16_t i = 0; i < paths_local.size(); ++i) {
			double dist = nodeDistanceTotal(paths_local[i]);
			if (dist < shortestPath) {
				shortestPath = dist;
				shortestPathIndex = i;
			}
		}
		// add the shortest path to the global path, and update the global illegal nodes
		for (uint16_t i = 0; i < paths_local[shortestPathIndex].size(); ++i) {
			toReturn.push_back(paths_local[shortestPathIndex][i]);
			globalIllegal.push_back(findNode(paths_local[shortestPathIndex][i]));
		}
		// update the current node
		current = nodes[paths_local[shortestPathIndex][paths_local[shortestPathIndex].size()-1]];
	}
}

#endif
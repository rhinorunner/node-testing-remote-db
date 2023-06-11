#ifndef ADDITIONS_HPP_CHECK
#define ADDITIONS_HPP_CHECK

#include <iostream>
#include <fstream>
#include <vector>
#include <SDL.h>
#include <thread>
#include "render.hpp"

static SDL_Window*   N_WINDOW;
static SDL_Renderer* N_RENDERER;

// window setup
void N_SDLSETUP() { 
	N_WINDOW = NULL;
	// renderer
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		std::cout << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
		exit(1);
	}
	SDL_CreateWindowAndRenderer(N_SCREENWIDTH,N_SCREENHEIGHT,0,&N_WINDOW,&N_RENDERER);
	if (N_WINDOW == NULL) {
		std::cout << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
		exit(1);
	}

	// set background color
	SDL_SetRenderDrawColor(N_RENDERER,N_BACKCOLOR.R,N_BACKCOLOR.G,N_BACKCOLOR.B,255);
	SDL_RenderClear(N_RENDERER);
}

void resetNodes() {
	N_NODES = {};
	for (uint16_t i = 0; i < N_NODEAMOUNT; ++i) {
		Node temp {
			(uint16_t)(rand() % N_SCREENWIDTH),
			(uint16_t)(rand() % N_SCREENHEIGHT),
			i,
			10,
			false,
			{155,155,155}
		};
		N_NODES.push_back(temp);
	}
}

void calcNodes() {
	bool first = false;
	
	nodeConnections = {
		IMP_quickPath(N_NODES, 10),
		IMP_quickPath(N_NODES, 20)
	};

	if (nodeShow.empty()) {
		first = true;
		for (auto i : nodeConnections) nodeShow.push_back(true);
	}

	// find the shortest node path
	float shortestPath = FLT_MAX;
	uint16_t shortestPathName = 0;
	
	for (uint16_t ll = 0; ll < nodeConnections.size(); ++ll) {
		if ((nodeDistanceTotal(nodeConnections[ll]) < shortestPath) && nodeShow[ll]) {
			shortestPath = (float)nodeDistanceTotal(nodeConnections[ll]);
			shortestPathName = ll;
		}
	}
	
	std::cout << "\n[" << N_NODES.size() << " nodes]\n";
	for (uint16_t im = 0; im < nodeConnections.size(); ++im) {
		std::cout 
			<< (nodeShow[im] ? "[shown]  " : "[hidden] ")
			<< "Path " 
			<< im << ": " 
			<< nodeDistanceTotal(!OVERRIDE[im].empty() ? OVERRIDE[im] : nodeConnections[im])
			<< (im == shortestPathName ? " <---" : "")
			<< '\n';
	}
	std::cout << '\n';
}

// turns a string path into a real path
std::vector<uint16_t> pathConv_vec(const std::string& str) {
	std::vector<uint16_t> toReturn;
	std::string temp;
	for (uint64_t i = 0; i < str.size(); ++i) {
		if (str[i] == ' ') {
			toReturn.push_back(std::stoi(temp));
			temp = "";
			continue;
		}
		temp += str[i];
	}
	return toReturn;
}

// turns a path into a string path
std::string pathConv_str(const std::vector<uint16_t>& vec) {
	std::string toReturn;
	for (uint64_t i = 0; i < vec.size(); ++i) {
		toReturn += std::to_string(vec[i]);
		if (i != vec.size()-1) toReturn += ", ";
	}
	return toReturn;
}

// loads all paths from file
std::vector<std::vector<uint16_t>> loadPaths() {
	std::vector<std::vector<uint16_t>> toReturn {};
	
	std::ifstream file ("localDbStorage.txt");
	std::string line;

	while (getline(file,line))
		toReturn.push_back(pathConv_vec(line));
	
	return toReturn;
}

#endif
#include "Island.h"

Island::Island() : visited(false), reachableIslands() {}

List<int>& Island::getReachableIslands() { return reachableIslands; }

void Island::markVisited() { visited = true; }

void Island::markUnvisited() { visited = false; }

bool Island::isVisited() const { return visited; }
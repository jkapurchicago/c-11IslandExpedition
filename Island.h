#ifndef ISLAND_EXPEDITION_ISLAND_H
#define ISLAND_EXPEDITION_ISLAND_H

#include "List.h"

class Island {
    List<int> reachableIslands;
    bool visited;

public:
    Island();

    List<int>& getReachableIslands();

    void markVisited();

    void markUnvisited();

    [[nodiscard]] bool isVisited() const;
};


#endif //ISLAND_EXPEDITION_ISLAND_H

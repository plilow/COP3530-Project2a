#pragma once

#include <vector>

#include "graph.h"

// One actor on a path, plus the movie linking them to the previous actor.
struct PathStep {
    int actor;
    int movie;  // -1 for the first step, which has no previous actor
    int weight; // billing-order weight of that link; 0 for the first step
};

struct PathResult {
    bool found = false;
    int degrees = -1;  //
    //Sum of the billing-order weights along the path (lower = stronger connection); -1 if not found
    //Filled in by both algorithms so their paths can be compared
    long long totalWeight = -1;
    std::vector<PathStep> path;
};

//Finds a path with the fewest degrees of separation using breadth-first search
//Start and target are actor indices. An index outside the graph gives not-found result
// An actor is 0 degrees from themselves
PathResult bfs(const Graph& graph, int start, int target);

// Finds the strongest connection: the path with the lowest total billing-order weight,
// using Dijkstra's algorithm with our own binary min-heap (src/minheap.h).
// Runs in O((V + E) log V). If several paths tie on weight, it returns one with the
// fewest degrees among them.
//
// Same input rules as bfs: an index outside the graph gives a not-found result, and an
// actor is 0 degrees (weight 0) from themselves.
PathResult dijkstra(const Graph& graph, int start, int target);

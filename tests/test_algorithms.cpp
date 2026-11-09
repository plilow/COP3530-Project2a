#include <iostream>
#include <string>

#include "algorithms.h"
#include "graph.h"

namespace {

int failures = 0;

// Prints the result of one check and counts it if it failed
void check(bool passed, const std::string& description) {
    std::cout << (passed ? "  PASS: " : "  FAIL: ") << description << "\n";
    if (!passed) {
        failures++;
    }
}

// Actor indices, in the order buildGraph adds them
const int START = 0;
const int MIDDLE = 1;
const int END = 2;
const int LONER = 3;

// Movie indices, in the order buildGraph adds them
const int FIRST_LINK = 0;
const int SECOND_LINK = 1;
const int DIRECT = 2;

// two routes from START to END, called short but weak if START -> END, weight 5
// long but strong if START -> MIDDLE -> END, weight 1 + 1 = 2
// LONER is in no movies, so nobody can reach them

Graph buildGraph() {
    Graph graph;
    graph.addActor(100, "Start");
    graph.addActor(101, "Middle");
    graph.addActor(102, "End");
    graph.addActor(103, "Loner");

    graph.addMovie(200, "First Link", 2000, {100, 101});
    graph.addMovie(201, "Second Link", 2001, {101, 102});

    // Ids 998 and 999 are not actors, but they push Start and End down to billing 2 and 3
    graph.addMovie(202, "Direct", 2002, {998, 999, 100, 102});
    return graph;
}

// BFS should take the route with the fewest actors, even though it is weaker
void testBfs() {

    std::cout << "BFS\n";
    Graph graph = buildGraph();
    PathResult result = bfs(graph, START, END);

    check(result.found, "finds a path");

    check(result.degrees == 1, "uses the fewest degrees");
    check(result.totalWeight == 5, "reports the weight of that path");
    check(result.path.size() == 2 && result.path[0].actor == START && result.path[1].actor == END,
          "goes straight from start to end");

    check(result.path.size() == 2 && result.path[1].movie == DIRECT, "links them with the shared movie");
    check(!bfs(graph, START, LONER).found, "does not reach an unconnected actor");
}

// Dijkstra should take the route with the lowest weight, even though it is longer
void testDijkstra() {
    std::cout << "Dijkstra\n";
    Graph graph = buildGraph();
    PathResult result = dijkstra(graph, START, END);

    check(result.found, "finds a path");
    check(result.degrees == 2, "accepts more degrees for a stronger path");
    check(result.totalWeight == 2, "uses the lowest total weight");

    check(result.path.size() == 3 && result.path[0].actor == START &&
              result.path[1].actor == MIDDLE && result.path[2].actor == END,
          "goes through the middle actor");
          
    check(result.path.size() == 3 && result.path[1].movie == FIRST_LINK &&
              result.path[2].movie == SECOND_LINK,
          "links each step with the right movie");
    check(!dijkstra(graph, START, LONER).found, "does not reach an unconnected actor");
}

}  // namespace

int main() {
    testBfs();
    testDijkstra();

    if (failures > 0) {
        std::cout << failures << " check(s) failed\n";
        return 1;
    }
    std::cout << "All checks passed\n";
    return 0;
}

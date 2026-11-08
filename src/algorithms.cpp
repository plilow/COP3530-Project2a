#include "algorithms.h"

#include <algorithm>
#include <limits>
#include <queue>

#include "minheap.h"

namespace {

// Walks the parent links back from target, then reverses them into start-to-target order.
// Shared by bfs and dijkstra, which only differ in how they fill in the parent arrays.
PathResult buildResult(int target, const std::vector<int>& parentActor,
                       const std::vector<int>& parentMovie, const std::vector<int>& parentWeight) {
    PathResult result;
    result.totalWeight = 0;
    for (int actor = target; actor != -1; actor = parentActor[actor]) {
        result.path.push_back({actor, parentMovie[actor], parentWeight[actor]});
        result.totalWeight += parentWeight[actor];
    }
    std::reverse(result.path.begin(), result.path.end());

    result.found = true;
    result.degrees = static_cast<int>(result.path.size()) - 1;
    return result;
}

}  // namespace

PathResult bfs(const Graph& graph, int start, int target) {
    PathResult result;
    int n = graph.actorCount();
    if (start < 0 || start >= n || target < 0 || target >= n) {
        return result;
    }

    std::vector<bool> visited(n, false);
    //How each actor was first reached, used to rebuild the path at the end.
    std::vector<int> parentActor(n, -1);
    std::vector<int> parentMovie(n, -1);
    std::vector<int> parentWeight(n, 0);

    std::queue<int> frontier;
    visited[start] = true;
    frontier.push(start);

    //Actors leave the queue in order of distance from start, so the first time
    //an actor is reached is along a shortest path.
    while (!frontier.empty() && !visited[target]) {
        int current = frontier.front();
        frontier.pop();

        for (const Edge& edge : graph.neighbors(current)) {
            if (!visited[edge.neighbor]) {
                visited[edge.neighbor] = true;
                parentActor[edge.neighbor] = current;
                parentMovie[edge.neighbor] = edge.movie;
                parentWeight[edge.neighbor] = edge.weight;
                frontier.push(edge.neighbor);
            }
        }
    }

    if (!visited[target]) {
        return result;
    }

    return buildResult(target, parentActor, parentMovie, parentWeight);
}

PathResult dijkstra(const Graph& graph, int start, int target) {
    PathResult result;
    int n = graph.actorCount();
    if (start < 0 || start >= n || target < 0 || target >= n) {
        return result;
    }

    const long long UNREACHED = std::numeric_limits<long long>::max();
    // dist[a] = lowest total weight found so far from start to a.
    // hops[a] = degrees on that path, used only to break ties between equal weights.
    std::vector<long long> dist(n, UNREACHED);
    std::vector<int> hops(n, 0);
    // settled[a] is true once a has left the heap; its dist can no longer improve.
    std::vector<bool> settled(n, false);
    std::vector<int> parentActor(n, -1);
    std::vector<int> parentMovie(n, -1);
    std::vector<int> parentWeight(n, 0);

    IndexedMinHeap frontier(n);
    dist[start] = 0;
    frontier.pushOrDecrease(start, 0);

    // Actors leave the heap in order of total weight from start. Every weight is at
    // least 1, so when an actor leaves the heap no cheaper path to it can exist.
    while (!frontier.empty()) {
        int current = frontier.popMin();
        settled[current] = true;
        if (current == target) {
            break;
        }

        for (const Edge& edge : graph.neighbors(current)) {
            int next = edge.neighbor;
            if (settled[next]) {
                continue;
            }
            long long candidate = dist[current] + edge.weight;
            bool cheaper = candidate < dist[next];
            bool sameWeightFewerHops = candidate == dist[next] && hops[current] + 1 < hops[next];
            if (cheaper || sameWeightFewerHops) {
                dist[next] = candidate;
                hops[next] = hops[current] + 1;
                parentActor[next] = current;
                parentMovie[next] = edge.movie;
                parentWeight[next] = edge.weight;
                frontier.pushOrDecrease(next, candidate);
            }
        }
    }

    if (!settled[target]) {
        return result;
    }
    return buildResult(target, parentActor, parentMovie, parentWeight);
}

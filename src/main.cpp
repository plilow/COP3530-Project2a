#include <chrono>
#include <iomanip>
#include <iostream>
#include <string>

#include "algorithms.h"
#include "graph.h"

namespace {

// Removes leading and trailing whitespace from user input
std::string trim(const std::string& text) {
    size_t first = text.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) {
        return "";
    }
    size_t last = text.find_last_not_of(" \t\r\n");
    return text.substr(first, last - first + 1);
}

// Tries data/ first (running from the repo root), then ../data/ (running from a build folder)
bool loadGraph(Graph& graph) {
    const std::string folders[] = {"data/", "../data/"};
    for (const std::string& folder : folders) {
        if (graph.loadFromFiles(folder + "actors.tsv", folder + "movies.tsv")) {
            return true;
        }
    }
    return false;
}

// One movie the actor appeared in, to tell apart actors who share a name
std::string knownFor(const Graph& graph, int actor) {
    const std::vector<Edge>& edges = graph.neighbors(actor);
    if (edges.empty()) {
        return "no co-stars in dataset";
    }
    const Movie& movie = graph.movie(edges.front().movie);
    return "e.g. " + movie.title + " (" + std::to_string(movie.year) + ")";
}

// Asks for an actor's name until it matches someone in the graph.
// Returns the actor's index, or -1 if the user wants to quit.
int promptForActor(const Graph& graph, const std::string& label) {
    while (true) {
        std::cout << label << " (or q to quit): ";
        std::string line;
        if (!std::getline(std::cin, line)) {
            return -1;
        }
        std::string name = trim(line);
        if (name == "q" || name == "Q") {
            return -1;
        }
        if (name.empty()) {
            continue;
        }

        std::vector<int> matches = graph.findActorsByName(name);
        if (matches.empty()) {
            std::cout << "  No actor named \"" << name << "\" in the dataset. Try again.\n";
            continue;
        }
        if (matches.size() == 1) {
            return matches.front();
        }

        // Names are not unique (e.g. two Tom Hollands), so let the user pick
        std::cout << "  " << matches.size() << " actors are named \"" << name << "\":\n";
        for (size_t i = 0; i < matches.size(); i++) {
            const Actor& actor = graph.actor(matches[i]);
            std::cout << "    " << i + 1 << ") " << actor.name << " [TMDB " << actor.id << "] - "
                      << knownFor(graph, matches[i]) << "\n";
        }
        while (true) {
            std::cout << "  Choose 1-" << matches.size() << ": ";
            if (!std::getline(std::cin, line)) {
                return -1;
            }
            try {
                int choice = std::stoi(trim(line));
                if (choice >= 1 && choice <= static_cast<int>(matches.size())) {
                    return matches[choice - 1];
                }
            } catch (const std::exception&) {
                // fall through to the retry message
            }
            std::cout << "  Please enter a number from the list.\n";
        }
    }
}

void printResult(const Graph& graph, const std::string& title, const PathResult& result,
                 double milliseconds) {
    std::cout << "\n== " << title << " ==\n";
    if (!result.found) {
        std::cout << "  These actors are not connected in the dataset.\n";
    } else {
        std::cout << "  Degrees of separation: " << result.degrees
                  << "    Total weight: " << result.totalWeight << "\n\n";
        for (const PathStep& step : result.path) {
            if (step.movie != -1) {
                const Movie& movie = graph.movie(step.movie);
                std::cout << "      | " << movie.title << " (" << movie.year << "), weight "
                          << step.weight << "\n";
            }
            std::cout << "    " << graph.actor(step.actor).name << "\n";
        }
    }
    std::cout << "\n  Time: " << std::fixed << std::setprecision(3) << milliseconds << " ms\n";
}

// Runs a search and measures how long it took, in milliseconds
template <typename Search>
PathResult timed(Search search, const Graph& graph, int start, int target, double& milliseconds) {
    auto begin = std::chrono::steady_clock::now();
    PathResult result = search(graph, start, target);
    auto end = std::chrono::steady_clock::now();
    milliseconds = std::chrono::duration<double, std::milli>(end - begin).count();
    return result;
}

}  // namespace

int main() {
    std::cout << "DegreesOfActors\n";
    std::cout << "Loading dataset...\n";

    Graph graph;
    auto loadStart = std::chrono::steady_clock::now();
    if (!loadGraph(graph)) {
        std::cerr << "Could not open data/actors.tsv and data/movies.tsv. "
                     "Run the program from the repository root.\n";
        return 1;
    }
    double loadSeconds =
        std::chrono::duration<double>(std::chrono::steady_clock::now() - loadStart).count();
    std::cout << "Loaded " << graph.actorCount() << " actors and " << graph.movieCount()
              << " movies in " << std::fixed << std::setprecision(2) << loadSeconds << " s\n";

    std::cout << "\nBFS finds the fewest degrees of separation.\n"
                 "Dijkstra finds the strongest connection: the lowest total billing-order weight\n"
                 "(1 = the top two billed actors in a movie, 27 = the bottom two of 15).\n";

    while (true) {
        std::cout << "\n";
        int start = promptForActor(graph, "Actor 1");
        if (start == -1) {
            break;
        }
        int target = promptForActor(graph, "Actor 2");
        if (target == -1) {
            break;
        }

        double bfsMs = 0;
        double dijkstraMs = 0;
        PathResult bfsResult = timed(bfs, graph, start, target, bfsMs);
        PathResult dijkstraResult = timed(dijkstra, graph, start, target, dijkstraMs);

        std::cout << "\n" << graph.actor(start).name << " -> " << graph.actor(target).name << "\n";
        printResult(graph, "BFS (fewest degrees)", bfsResult, bfsMs);
        printResult(graph, "Dijkstra (strongest connection)", dijkstraResult, dijkstraMs);
    }

    std::cout << "Goodbye!\n";
    return 0;
}

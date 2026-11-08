#include "graph.h"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <sstream>


std::string toLower(std::string text) {
    for (char& c : text) {
        c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    }
    return text;
}

//Reads a line and removes the trailing \r that Windows line endings leave
bool readLine(std::istream& in, std::string& line) {
    if (!std::getline(in, line)) {
        return false;
    }
    if (!line.empty() && line.back() == '\r') {
        line.pop_back();
    }
    return true;
}


bool Graph::loadFromFiles(const std::string& actorsPath, const std::string& moviesPath) {
    std::ifstream actorsFile(actorsPath);
    std::ifstream moviesFile(moviesPath);
    if (!actorsFile || !moviesFile) {
        return false;
    }

    std::string line;

    //Format: actor_id  name
    readLine(actorsFile, line);  
    while (readLine(actorsFile, line)) {
        std::istringstream fields(line);
        std::string id, name;
        std::getline(fields, id, '\t');
        std::getline(fields, name, '\t');
        addActor(std::stoi(id), name);
    }

    //Format: movie_id  title  year, actor ids
    readLine(moviesFile, line);  
    while (readLine(moviesFile, line)) {
        std::istringstream fields(line);
        std::string id, title, year, cast;
        std::getline(fields, id, '\t');
        std::getline(fields, title, '\t');
        std::getline(fields, year, '\t');
        std::getline(fields, cast, '\t');

        std::vector<int> castIds;
        std::istringstream castFields(cast);
        std::string castId;
        while (std::getline(castFields, castId, ',')) {
            castIds.push_back(std::stoi(castId));
        }
        addMovie(std::stoi(id), title, std::stoi(year), castIds);
    }
    return true;
}

void Graph::addActor(int id, const std::string& name) {
    int index = actorCount();
    actors.push_back({id, name});
    adjacency.emplace_back();
    indexById[id] = index;
    indexByName[toLower(name)].push_back(index);
}

void Graph::addMovie(int id, const std::string& title, int year, const std::vector<int>& castIds) {
    int movieIndex = movieCount();
    movies.push_back({id, title, year});

    //Each cast member's actor index and billing position (0 = top billed).
    //The position comes from the original list, so it is recorded before anything is filtered or sorted
    struct CastMember {
        int actor;
        int billing;
    };
    std::vector<CastMember> cast;
    for (size_t position = 0; position < castIds.size(); position++) {
        int index = findActorById(castIds[position]);
        if (index != -1) {
            cast.push_back({index, static_cast<int>(position)});
        }
    }

    //Removes duplicates to ensure no loops. Sorting by actor, then billing, puts each actor's
    //best billing first, and std::unique keeps that one
    std::sort(cast.begin(), cast.end(), [](const CastMember& a, const CastMember& b) {
        return a.actor != b.actor ? a.actor < b.actor : a.billing < b.billing;
    });
    cast.erase(std::unique(cast.begin(), cast.end(),
                           [](const CastMember& a, const CastMember& b) { return a.actor == b.actor; }),
               cast.end());

    for (size_t i = 0; i < cast.size(); i++) {
        for (size_t j = i + 1; j < cast.size(); j++) {
            int weight = cast[i].billing + cast[j].billing;
            adjacency[cast[i].actor].push_back({cast[j].actor, movieIndex, weight});
            adjacency[cast[j].actor].push_back({cast[i].actor, movieIndex, weight});
        }
    }
}

int Graph::findActorById(int id) const {
    auto it = indexById.find(id);
    return it == indexById.end() ? -1 : it->second;
}

std::vector<int> Graph::findActorsByName(const std::string& name) const {
    auto it = indexByName.find(toLower(name));
    return it == indexByName.end() ? std::vector<int>{} : it->second;
}

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
    indicesByName[toLower(name)].push_back(index);
}

void Graph::addMovie(int id, const std::string& title, int year, const std::vector<int>& castIds) {
    int movieIndex = movieCount();
    movies.push_back({id, title, year});

    std::vector<int> cast;
    for (int castId : castIds) {
        int index = findActorById(castId);
        if (index != -1) {
            cast.push_back(index);
        }
    }

    //Removes duplicates to ensure no loops
    std::sort(cast.begin(), cast.end());
    cast.erase(std::unique(cast.begin(), cast.end()), cast.end());

    for (size_t i = 0; i < cast.size(); i++) {
        for (size_t j = i + 1; j < cast.size(); j++) {
            adjacency[cast[i]].push_back({cast[j], movieIndex});
            adjacency[cast[j]].push_back({cast[i], movieIndex});
        }
    }
}

int Graph::findActorById(int id) const {
    auto it = indexById.find(id);
    return it == indexById.end() ? -1 : it->second;
}

std::vector<int> Graph::findActorsByName(const std::string& name) const {
    auto it = indicesByName.find(toLower(name));
    return it == indicesByName.end() ? std::vector<int>{} : it->second;
}

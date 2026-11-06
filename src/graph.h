#pragma once

#include <string>
#include <unordered_map>
#include <vector>

struct Actor {
    int id;  //TMDB person id
    std::string name;
};

struct Movie {
    int id;  //TMDB movie id
    std::string title;
    int year;
};


//graph's actor and movie index lists, not TMDB ids.
struct Edge {
    int neighbor;
    int movie;
};

//Undirected graph stored as an adjacency list
//Each vertex is an actor. Two actors share an edge for every movie they appeared in together
//A pair of actors with three shared movies has three parallel edges
class Graph {
public:
    //Loads the TSV files in data/ Returns false if either can't be opened
    bool loadFromFiles(const std::string& actorsPath, const std::string& moviesPath);

    void addActor(int id, const std::string& name);

    //Connects every pair of actors in the cast
    //Cast ids that don't match a known actor are ignored
    void addMovie(int id, const std::string& title, int year, const std::vector<int>& castIds);

    int actorCount() const { return static_cast<int>(actors.size()); }
    int movieCount() const { return static_cast<int>(movies.size()); }

    const Actor& actor(int index) const { return actors[index]; }
    const Movie& movie(int index) const { return movies[index]; }
    const std::vector<Edge>& neighbors(int index) const { return adjacency[index]; }

    //Returns the actor's index or -1 if the id is unknown
    int findActorById(int id) const;

    //Returns the index of every actor with this name, ignoring case
    //Names are not unique (the dataset has two actors named Tom Holland), so callers need to handle more than one match.
    std::vector<int> findActorsByName(const std::string& name) const;

private:
    std::vector<Actor> actors;
    std::vector<Movie> movies;
    std::vector<std::vector<Edge>> adjacency;
    std::unordered_map<int, int> indexById;
    std::unordered_map<std::string, std::vector<int>> indexByName;
};

#pragma once
#include <vector>
#include <array>

struct Graphe
{
    using Point = std::pair<float, float>; // Représente un point (x, y)
    using Adjacency = std::pair<Point, Point>; // Représente une paire d'indices de points adjacents

    std::vector<float> pointList; // Exemple: [ x0, y0, x1, y1, ...]
    std::vector<int> adjacencySize; // Exemple: [ 0(indice start for P0), 2(nbr of adjacence for P0), 2, 4, 6, 1, ...] 
    std::vector<int> pointAdjacencyList; // Exemple: [ -->2, 3<--(adj for P0), -->1, 3, 4, 8<--(adj for P1), -->0<--(adj for P2), ...] 

    void centralisation();

    void init_from_bad_format(const std::vector<Point>& points, const std::vector<Adjacency>& adjacencies);

    bool floatEqual(float& a, float& b, float epsilon = 0.0001)
    {
        return (std::abs(a - b) < epsilon);
    }

    bool pointEqual(Point& const p1, Point& const p2)
    {
        return (floatEqual(p1.first, p2.first) && floatEqual(p1.second, p2.second));
    }
};



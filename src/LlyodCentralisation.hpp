#pragma once
#include <vector>

struct Graphe
{
    std::vector<float> pointList; // Exemple: [ x0, y0, x1, y1, ...]
    std::vector<int> adjacencySize; // Exemple: [ 0(indice start for P0), 2(nbr of adjacence for P0), 2, 4, 6, 1, ...] 
    std::vector<int> pointAdjacencyList; // Exemple: [ -->2, 3<--(adj for P0), -->1, 3, 4, 8<--(adj for P1), -->0<--(adj for P2), ...] 



    void centralisation();
};
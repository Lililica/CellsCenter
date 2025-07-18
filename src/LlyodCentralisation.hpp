#pragma once
#include <array>
#include <iostream>
#include <vector>
#include "Delaunay/include/delaunay.h"
#include "Delaunay/include/triangle.h"
#include "Delaunay/include/vector2.h"
#include "utils.hpp"

struct Graphe {
    using Point     = std::pair<float, float>; // Représente un point (x, y)
    using Adjacency = std::pair<Point, Point>; // Représente une paire d'indices de points adjacents
    using Circle    = std::pair<Point, float>; // Représente un cercle avec son centre et son rayon
    using Triangle  = std::array<Point, 3>;    // Représente un triangle avec ses trois sommets

    float radius = 10.f; // Rayon de la zone d'influence des points

    float step                   = 1.f; // Variable de temps pour l'animation ou la centralisation
    int   nbrCentralisation      = 0;   // Nombre de centralisations effectuées
    int   nbrPoints              = 500;
    int   currentIdxEnergiePoint = 0;   // Index of the point for which we want to calculate the CVT energy
    float currentCVTEnergie      = 0.f; // Current CVT energy for the point at currentIdxEnergiePoint

    bool useWelzl       = false; // Flag to indicate whether to use Welzl's algorithm for circle calculation
    bool useCentroid    = true;  // Flag to indicate whether to use the centroid for circle calculation
    bool useSquare      = false; // Flag to indicate whether to use the square of the distance for energy calculation
    bool kNearest       = false; // Flag to indicate whether to use the k-nearest neighbors for energy calculation
    bool useOrientedBox = false; // Flag to control whether to use the oriented bounding box method
    bool useMean        = false; // Flag to control whether to use the mean for energy calculation

    Point welzlCenterOf0;    // Center of the circle calculated by Welzl's algorithm
    Point centroidCenterOf0; // Center of the centroid calculated from the points

    std::vector<Point> pointList; // Exemple: [ (x0, y0), (x1, y1), ...]

    std::vector<std::vector<Point>>                pointsAdjacents; // List of adjacent points for each point in pointList
    std::vector<std::vector<Point>>                nearCellulePoints;
    std::vector<std::vector<std::array<Point, 2>>> nearCellulePointsTriees; // List of segments formed by the near cell points

    std::vector<Point> nearCellulePointsList; // List of points that are near the cell
    std::vector<Point> celluleBorder;         // Indices of the centers that are considered border points

    std::vector<int> idxPointBorder;            // Indices of the points that are considered border points
    bool             hasDetectedBorder = false; // Flag to indicate if border points have been detected

    std::vector<Triangle>           trianglesPoints; // List of triangles formed by the near cell points
    std::vector<std::array<int, 3>> idxTriangles;    // List of indices of points in triangles
    std::vector<Circle>             triangleCircles; // List of circles formed by the near cell points
    int                             nbrFlips = 0;    // Number of flips performed during the Delaunay triangulation

    std::vector<Circle>               allCircles;
    std::vector<std::array<Point, 4>> allOrientedBoxes; // List of oriented bounding boxes for the points

    std::vector<double> energies; // List of energies after each iterations

    int                             k = 8;          // Number of nearest neighbors to consider for k-nearest points
    std::vector<std::vector<Point>> kNearestPoints; // List of k-nearest points for each point in pointList

    void centralisation();

    bool floatEqual(const float& a, const float& b, float epsilon = 0.0001)
    {
        return (std::abs(a - b) < epsilon);
    }

    bool pointEqual(const Point& p1, const Point& p2)
    {
        return (floatEqual(p1.first, p2.first) && floatEqual(p1.second, p2.second));
    }

    int getIndexFromPoint(const Point& p)
    {
        for (int i = 0; i < pointList.size(); ++i)
        {
            // std::cout << "Comparing point: " << pointList[i].first << ", " << pointList[i].second
            //           << " with point: " << p.first << ", " << p.second << std::endl;
            if (pointEqual(pointList[i], p))
            {
                return i;
            }
        }
        std::cerr << "Point not found in the graph: (" << p.first << ", " << p.second << ")" << '\n';
        return -1; // Point not found
    }

    void set_triangles(const std::vector<dt::Triangle<double>>& triangles)
    {
        trianglesPoints.clear();
        pointsAdjacents.clear();
        pointsAdjacents.resize(pointList.size()); // Resize the adjacency list to match the number of points
        for (const auto& triangle : triangles)
        {
            trianglesPoints.emplace_back(Triangle{Point(triangle.a->x, triangle.a->y), Point(triangle.b->x, triangle.b->y), Point(triangle.c->x, triangle.c->y)});
            int idxA = getIndexFromPoint(trianglesPoints.back()[0]);
            int idxB = getIndexFromPoint(trianglesPoints.back()[1]);
            int idxC = getIndexFromPoint(trianglesPoints.back()[2]);
            if (idxA == -1 || idxB == -1 || idxC == -1)
            {
                std::cerr << "Error: One of the triangle points is not found in the graph." << '\n';
                continue; // Skip this triangle if any point is not found
            }
            idxTriangles.emplace_back(std::array<int, 3>{idxA, idxB, idxC}); // Add the indices of the triangle points

            if (std::find(pointsAdjacents[idxA].begin(), pointsAdjacents[idxA].end(), trianglesPoints.back()[1]) == pointsAdjacents[idxA].end())
                pointsAdjacents[idxA].push_back(trianglesPoints.back()[1]);
            if (std::find(pointsAdjacents[idxA].begin(), pointsAdjacents[idxA].end(), trianglesPoints.back()[2]) == pointsAdjacents[idxA].end())
                pointsAdjacents[idxA].push_back(trianglesPoints.back()[2]);
            if (std::find(pointsAdjacents[idxB].begin(), pointsAdjacents[idxB].end(), trianglesPoints.back()[0]) == pointsAdjacents[idxB].end())
                pointsAdjacents[idxB].push_back(trianglesPoints.back()[0]);
            if (std::find(pointsAdjacents[idxB].begin(), pointsAdjacents[idxB].end(), trianglesPoints.back()[2]) == pointsAdjacents[idxB].end())
                pointsAdjacents[idxB].push_back(trianglesPoints.back()[2]);
            if (std::find(pointsAdjacents[idxC].begin(), pointsAdjacents[idxC].end(), trianglesPoints.back()[0]) == pointsAdjacents[idxC].end())
                pointsAdjacents[idxC].push_back(trianglesPoints.back()[0]);
            if (std::find(pointsAdjacents[idxC].begin(), pointsAdjacents[idxC].end(), trianglesPoints.back()[1]) == pointsAdjacents[idxC].end())
                pointsAdjacents[idxC].push_back(trianglesPoints.back()[1]);

            std::vector<Point> currentTriangle = {trianglesPoints.back()[0], trianglesPoints.back()[1], trianglesPoints.back()[2]};
            std::vector<Point> boundaryPoints  = {}; // Get the boundary points for the current triangle
            Circle             currentCircle   = welzl(currentTriangle, boundaryPoints);
            triangleCircles.emplace_back(currentCircle); // Add the circle to the list of triangle circles
        }
    }

    void doDelaunayAndCalculateCenters();
    void flipDelaunayTriangles();

    void calculateCenterFromDelaunayTriangles(const std::vector<Triangle>& triangles);

    void findBorderPoints();
    bool hasOtherTriangleForSegment(const std::vector<Triangle>& trianglesPoints, const Point& p1, const Point& p2, const Point& excluded);
    void findBorderPoints_BROKEN_Function();

    float calcul_CVT_energie(const int& idxPoint);
    void  triesNearCellulePoints(); // Sort the near cell points based on their distance to the current energy point

    void updateCenterExample();
};

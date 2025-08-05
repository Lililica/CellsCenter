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

    std::vector<std::vector<int>>                  pointsAdjacentsIdx; // List of adjacent points for each point in pointList
    std::vector<std::vector<Point>>                nearCellulePoints;
    std::vector<std::vector<std::array<Point, 2>>> nearCellulePointsTriees; // List of segments formed by the near cell points

    std::vector<Point> nearCellulePointsList; // List of points that are near the cell
    std::vector<Point> celluleBorder;         // Indices of the centers that are considered border points

    std::vector<int> idxPointBorder;            // Indices of the points that are considered border points
    bool             hasDetectedBorder = false; // Flag to indicate if border points have been detected

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
        idxTriangles.clear();
        pointsAdjacentsIdx.clear();
        pointsAdjacentsIdx.resize(pointList.size()); // Resize the adjacency list to match the number of points
        triangleCircles.clear();                     // Clear the list of triangle circles before setting new triangles
        triangleCircles.reserve(triangles.size());   // Reserve space for the triangle circles to avoid
        for (const auto& triangle : triangles)
        {
            Point p1   = {triangle.a->x, triangle.a->y}; // Get the first point of the triangle
            Point p2   = {triangle.b->x, triangle.b->y}; // Get the second point of the triangle
            Point p3   = {triangle.c->x, triangle.c->y}; // Get the third point of the triangle
            int   idxA = getIndexFromPoint(p1);
            int   idxB = getIndexFromPoint(p2);
            int   idxC = getIndexFromPoint(p3);
            idxTriangles.emplace_back(std::array<int, 3>{idxA, idxB, idxC}); // Add the indices of the triangle points

            if (idxA == -1 || idxB == -1 || idxC == -1)
            {
                triangleCircles.emplace_back(Circle{});
                std::cerr << "Error: One of the triangle points is not found in the graph." << '\n';
                continue; // Skip this triangle if any point is not found
            }

            if (std::find(pointsAdjacentsIdx[idxA].begin(), pointsAdjacentsIdx[idxA].end(), idxB) == pointsAdjacentsIdx[idxA].end())
                pointsAdjacentsIdx[idxA].push_back(idxB);
            if (std::find(pointsAdjacentsIdx[idxA].begin(), pointsAdjacentsIdx[idxA].end(), idxC) == pointsAdjacentsIdx[idxA].end())
                pointsAdjacentsIdx[idxA].push_back(idxC);
            if (std::find(pointsAdjacentsIdx[idxB].begin(), pointsAdjacentsIdx[idxB].end(), idxA) == pointsAdjacentsIdx[idxB].end())
                pointsAdjacentsIdx[idxB].push_back(idxA);
            if (std::find(pointsAdjacentsIdx[idxB].begin(), pointsAdjacentsIdx[idxB].end(), idxC) == pointsAdjacentsIdx[idxB].end())
                pointsAdjacentsIdx[idxB].push_back(idxC);
            if (std::find(pointsAdjacentsIdx[idxC].begin(), pointsAdjacentsIdx[idxC].end(), idxA) == pointsAdjacentsIdx[idxC].end())
                pointsAdjacentsIdx[idxC].push_back(idxA);
            if (std::find(pointsAdjacentsIdx[idxC].begin(), pointsAdjacentsIdx[idxC].end(), idxB) == pointsAdjacentsIdx[idxC].end())
                pointsAdjacentsIdx[idxC].push_back(idxB);

            // // Get the circumcircle of the current triangle
            // if (std::find(idxPointBorder.begin(), idxPointBorder.end(), idxA) != idxPointBorder.end()
            //     || std::find(idxPointBorder.begin(), idxPointBorder.end(), idxB) != idxPointBorder.end()
            //     || std::find(idxPointBorder.begin(), idxPointBorder.end(), idxC) != idxPointBorder.end())
            // {
            //     triangleCircles.emplace_back(Circle{});
            //     continue; // Skip this triangle if any point is a border point
            // }

            Circle currentCircle;

            std::array<std::array<float, 3>, 3> matForX{{{1.f, (p1.second), static_cast<float>(((std::pow(p1.second, 2) + std::pow(p1.first, 2)) / 2.))},   // L1
                                                         {1.f, (p2.second), static_cast<float>(((std::pow(p2.second, 2) + std::pow(p2.first, 2)) / 2.))},   // L2
                                                         {1.f, (p3.second), static_cast<float>(((std::pow(p3.second, 2) + std::pow(p3.first, 2)) / 2.))}}}; // L3

            auto x = static_cast<float>(-determinant3x3(matForX));

            std::array<std::array<float, 3>, 3> matForY{{{1.f, static_cast<float>(p1.first), static_cast<float>((std::pow(p1.second, 2) + std::pow(p1.first, 2)) / 2.)}, {1.f, static_cast<float>(p2.first), static_cast<float>((std::pow(p2.second, 2) + std::pow(p2.first, 2)) / 2.)}, {1.f, static_cast<float>(p3.first), static_cast<float>((std::pow(p3.second, 2) + std::pow(p3.first, 2)) / 2.)}}};

            auto y = static_cast<float>(determinant3x3(matForY));

            std::array<std::array<float, 3>, 3> matForW{{{1.f, static_cast<float>(p1.first), static_cast<float>(p1.second)}, {1.f, static_cast<float>(p2.first), static_cast<float>(p2.second)}, {1.f, static_cast<float>(p3.first), static_cast<float>(p3.second)}}};

            auto w = static_cast<float>(determinant3x3(matForW));

            Point center(x / w, y / w); // Calculate the center of the circumcircle

            currentCircle = Circle(center, std::sqrt(std::pow(p1.first - center.first, 2) + std::pow(p1.second - center.second, 2))); // Calculate the radius of the circumcircle
            triangleCircles.emplace_back(currentCircle);                                                                              // Add the circle to the list of triangle circles
        }
    }

    void set_triangle_v2()
    {
        pointsAdjacentsIdx.clear();
        pointsAdjacentsIdx.resize(pointList.size());  // Resize the adjacency list to match the number of points
        triangleCircles.clear();                      // Clear the list of triangle circles before setting new triangles
        triangleCircles.reserve(idxTriangles.size()); // Reserve space for the triangle circles to avoid
        for (const auto& triangle : idxTriangles)
        {
            int idxA = (triangle[0]);
            int idxB = (triangle[1]);
            int idxC = (triangle[2]);
            if (idxA == -1 || idxB == -1 || idxC == -1)
            {
                std::cerr << "Error: One of the triangle points is not found in the graph." << '\n';
                continue; // Skip this triangle if any point is not found
            }

            if (std::find(pointsAdjacentsIdx[idxA].begin(), pointsAdjacentsIdx[idxA].end(), idxB) == pointsAdjacentsIdx[idxA].end())
                pointsAdjacentsIdx[idxA].push_back(idxB);
            if (std::find(pointsAdjacentsIdx[idxA].begin(), pointsAdjacentsIdx[idxA].end(), idxC) == pointsAdjacentsIdx[idxA].end())
                pointsAdjacentsIdx[idxA].push_back(idxC);
            if (std::find(pointsAdjacentsIdx[idxB].begin(), pointsAdjacentsIdx[idxB].end(), idxA) == pointsAdjacentsIdx[idxB].end())
                pointsAdjacentsIdx[idxB].push_back(idxA);
            if (std::find(pointsAdjacentsIdx[idxB].begin(), pointsAdjacentsIdx[idxB].end(), idxC) == pointsAdjacentsIdx[idxB].end())
                pointsAdjacentsIdx[idxB].push_back(idxC);
            if (std::find(pointsAdjacentsIdx[idxC].begin(), pointsAdjacentsIdx[idxC].end(), idxA) == pointsAdjacentsIdx[idxC].end())
                pointsAdjacentsIdx[idxC].push_back(idxA);
            if (std::find(pointsAdjacentsIdx[idxC].begin(), pointsAdjacentsIdx[idxC].end(), idxB) == pointsAdjacentsIdx[idxC].end())
                pointsAdjacentsIdx[idxC].push_back(idxB);

            // if (std::find(idxPointBorder.begin(), idxPointBorder.end(), idxA) != idxPointBorder.end()
            //     || std::find(idxPointBorder.begin(), idxPointBorder.end(), idxB) != idxPointBorder.end()
            //     || std::find(idxPointBorder.begin(), idxPointBorder.end(), idxC) != idxPointBorder.end())
            // {
            //     triangleCircles.emplace_back(Circle{});
            //     continue; // Skip this triangle if any point is a border point
            // }
            // Get the circumcircle of the current triangle
            Point p1 = pointList[idxA];
            Point p2 = pointList[idxB];
            Point p3 = pointList[idxC];

            Circle currentCircle;

            std::array<std::array<float, 3>, 3> matForX{{{1.f, (p1.second), static_cast<float>(((std::pow(p1.second, 2) + std::pow(p1.first, 2)) / 2.))},   // L1
                                                         {1.f, (p2.second), static_cast<float>(((std::pow(p2.second, 2) + std::pow(p2.first, 2)) / 2.))},   // L2
                                                         {1.f, (p3.second), static_cast<float>(((std::pow(p3.second, 2) + std::pow(p3.first, 2)) / 2.))}}}; // L3

            auto x = static_cast<float>(-determinant3x3(matForX));

            std::array<std::array<float, 3>, 3> matForY{{{1.f, static_cast<float>(p1.first), static_cast<float>((std::pow(p1.second, 2) + std::pow(p1.first, 2)) / 2.)}, {1.f, static_cast<float>(p2.first), static_cast<float>((std::pow(p2.second, 2) + std::pow(p2.first, 2)) / 2.)}, {1.f, static_cast<float>(p3.first), static_cast<float>((std::pow(p3.second, 2) + std::pow(p3.first, 2)) / 2.)}}};

            auto y = static_cast<float>(determinant3x3(matForY));

            std::array<std::array<float, 3>, 3> matForW{{{1.f, static_cast<float>(p1.first), static_cast<float>(p1.second)}, {1.f, static_cast<float>(p2.first), static_cast<float>(p2.second)}, {1.f, static_cast<float>(p3.first), static_cast<float>(p3.second)}}};

            auto w = static_cast<float>(determinant3x3(matForW));

            Point center(x / w, y / w); // Calculate the center of the circumcircle

            currentCircle = Circle(center, std::sqrt(std::pow(p1.first - center.first, 2) + std::pow(p1.second - center.second, 2))); // Calculate the radius of the circumcircle
            triangleCircles.emplace_back(currentCircle);                                                                              // Add the circle to the list of triangle circles
        }
    }

    void doDelaunayAndCalculateCenters();
    void doDelaunayFlipVersion(int& nbrFlips);
    int  flipDelaunayTriangles();

    void calculateCenterFromDelaunayTriangles(const std::vector<std::array<int, 3>>& triangles);

    void findBorderPoints();
    bool hasOtherTriangleForSegment(const std::vector<Triangle>& trianglesPoints, const Point& p1, const Point& p2, const Point& excluded);
    void findBorderPoints_BROKEN_Function();

    float calcul_CVT_energie(const int& idxPoint);
    void  triesNearCellulePoints(); // Sort the near cell points based on their distance to the current energy point

    void updateCenterExample();
};

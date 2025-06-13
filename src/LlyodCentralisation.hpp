#pragma once
#include <array>
#include <iostream>
#include <vector>
#include "Delaunay/include/delaunay.h"
#include "Delaunay/include/triangle.h"
#include "Delaunay/include/vector2.h"

struct Graphe {
    using Point     = std::pair<float, float>; // Représente un point (x, y)
    using Adjacency = std::pair<Point, Point>; // Représente une paire d'indices de points adjacents

    float step              = 0.001f; // Variable de temps pour l'animation ou la centralisation
    int   nbrCentralisation = 0;      // Nombre de centralisations effectuées
    int   nbrPoints         = 500;

    std::vector<Point> pointList; // Exemple: [ (x0, y0), (x1, y1), ...]

    std::vector<std::vector<Point>> nearCellulePoints;
    std::vector<Point>              nearCellulePointsList; // List of points that are near the cell
    std::vector<Point>              celluleBorder;         // Indices of the centers that are considered border points

    std::vector<int> idxPointBorder;            // Indices of the points that are considered border points
    bool             hasDetectedBorder = false; // Flag to indicate if border points have been detected

    std::vector<std::array<Point, 3>> trianglesPoints; // List of triangles formed by the near cell points

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
        for (const auto& triangle : triangles)
        {
            trianglesPoints.emplace_back(std::array<Point, 3>{Point(triangle.a->x, triangle.a->y), Point(triangle.b->x, triangle.b->y), Point(triangle.c->x, triangle.c->y)});
        }
    }

    void doDelaunayAndCalculateCenters();

    void calculateCenterFromDelaunayTriangles(const std::vector<dt::Triangle<double>>& triangles);

    void findBorderPoints();
    bool hasOtherTriangleForSegment(const std::vector<std::array<Point, 3>>& trianglesPoints, const Point& p1, const Point& p2, const Point& excluded);
    void findBorderPoints_BROKEN_Function();
};

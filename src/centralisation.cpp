#include <complex.h>
#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <iostream>
#include <numbers>
#include "LlyodCentralisation.hpp"
#include "boundingBox.hpp"
#include "utils.hpp"

using Point = std::pair<float, float>; // Représente un point (x, y)

// Fonction utilitaire pour calculer le centre (barycentre simple) des points
Point computeCenter(const std::vector<Point>& points)
{
    float cx = 0.0f;
    float cy = 0.0f;
    for (const auto& p : points)
    {
        cx += p.first;
        cy += p.second;
    }
    cx /= static_cast<float>(points.size());
    cy /= static_cast<float>(points.size());
    return {cx, cy};
}

// Fonction pour trier les points dans l'ordre antihoraire autour du centre
void sortPointsCCW(std::vector<Point>& points)
{
    Point center = computeCenter(points);
    std::sort(points.begin(), points.end(), [center](const Point& a, const Point& b) {
        float angleA = std::atan2(a.second - center.second, a.first - center.first);
        float angleB = std::atan2(b.second - center.second, b.first - center.first);
        return angleA < angleB;
    });
}

// Calcule le centroïde d'un polygone 2D
Point computeCentroid(const std::vector<Point>& vertices)
{
    double area = 0.0;
    double cx   = 0.0;
    double cy   = 0.0;
    size_t n    = vertices.size();

    for (int i = 0; i < n; ++i)
    {
        const Point& p0 = vertices[i];
        const Point& p1 = vertices[(i + 1) % n];

        double cross = p0.first * p1.second - p1.first * p0.second;
        area += cross;
        cx += (p0.first + p1.first) * cross;
        cy += (p0.second + p1.second) * cross;
    }

    area *= 0.5;
    cx /= (6.0 * area);
    cy /= (6.0 * area);

    return {cx, cy};
}

Point squareCenter(const std::vector<Point>& points)
{
    if (points.size() < 3)
    {
        std::cerr << "Not enough points to form a square. Returning the first point as center.\n";
        return points.empty() ? Point{0, 0} : points[0];
    }

    // Calculate the center of the square formed by the points
    float minX = points[0].first;
    float maxX = points[0].first;
    float minY = points[0].second;
    float maxY = points[0].second;

    for (const auto& p : points)
    {
        minX = std::min(p.first, minX);
        maxX = std::max(p.first, maxX);
        minY = std::min(p.second, minY);
        maxY = std::max(p.second, maxY);
    }

    // Return the center of the square
    return {(minX + maxX) / 2.0f, (minY + maxY) / 2.0f};
}

void Graphe::centralisation()
{
    allCircles.clear();       // Clear the list of circles before centralisation
    allOrientedBoxes.clear(); // Clear the list of oriented boxes before centralisation

    if (kNearest)
    {
        kNearestPoints.clear();                  // Clear the list of k-nearest points before centralisation
        kNearestPoints.resize(pointList.size()); // Resize to match the number of points in pointList
        for (int i = 0; i < pointList.size(); ++i)
        {
            int   compteur = 0;            // Counter for the number of k-nearest points found
            Point point    = pointList[i]; // Get the current point from the graph
            while (compteur < k)
            {
                float minDistance = std::numeric_limits<float>::max();
                Point closestPoint;
                for (const auto& neighbor : pointList)
                {
                    if (std::find(kNearestPoints[i].begin(), kNearestPoints[i].end(), neighbor) != kNearestPoints[i].end())
                    {
                        float d = distance(neighbor, point);
                        if (d < minDistance)
                        {
                            minDistance  = d;
                            closestPoint = neighbor;
                        }
                    }
                }
                if (minDistance < 10000) // Check if the closest point is valid
                {
                    kNearestPoints[i].push_back(closestPoint); // Add the closest point to the k-nearest points
                    compteur++;                                // Increment the counter for k-nearest points
                }
                else
                {
                    continue; // Break if no valid closest point is found
                }
            }
        }
    }

    for (int i = 0; i < pointList.size(); ++i)
    {
        if (std::find(idxPointBorder.begin(), idxPointBorder.end(), i) != idxPointBorder.end())
        {
            // std::cerr << "Skipping centralisation for border point (" << pointList[i].first << ", " << pointList[i].second << ").\n";
            continue; // Skip centralisation for border points
        }

        std::vector<Point> neighbors; // Get the neighbors of the current point
        Point              centroid;

        if (useWelzl)
        {
            auto neighborsIdx = pointsAdjacentsIdx[i];
            neighbors.clear();
            for (const auto& idx : neighborsIdx)
            {
                neighbors.push_back(pointList[idx]);
            }
            sortPointsCCW(neighbors);
            std::vector<Point>      boundaryPoints;                            // Get the boundary points for the first point
            std::pair<Point, float> circle = welzl(neighbors, boundaryPoints); // Calculate the circle using Welzl's algorithm
            centroid                       = circle.first;                     // Calculate the centroid using Welzl's algorithm

            if (pointEqual(centroid, Point{0, 0}))
            {
                std::cout << "Welzl returned (0, 0) for point (" << pointList[i].first << ", " << pointList[i].second
                          << "). Removing this point of the list.\n";

                // Check if the point is dupliacte in my list
                int nbrApparition = 0;
                for (const auto& p : pointList)
                {
                    if (pointEqual(p, pointList[i]))
                    // std::cout << "Skipping centralisation for this duplicate point.\n";
                    // std::cout << "--------------------------------------------------\n";
                    {
                        nbrApparition++;
                    }
                    if (nbrApparition > 1)
                    {
                        std::cerr << "Point (" << pointList[i].first << ", " << pointList[i].second
                                  << ") appears multiple times in the point list. Skipping centralisation for this point.\n";
                        break; // Skip centralisation for this point if it appears multiple times
                    }
                }

                // Show neighbors
                std::cout << "Neighbors for point (" << pointList[i].first << ", " << pointList[i].second << "):\n";
                for (const auto& neighbor : neighbors)
                {
                    std::cout << "Neighbor: (" << neighbor.first << ", " << neighbor.second << ")\n";
                }

                // Remove the point from the list
                idxPointBorder.push_back(i); // Add the index of the border point to the list
                continue;                    // Skip centralisation for this point
            }

            allCircles.push_back(circle); // Store the circle for later use
        }
        else if (useSquare)
        {
            auto neighborsIdx = pointsAdjacentsIdx[i];
            neighbors.clear();
            for (const auto& idx : neighborsIdx)
            {
                neighbors.push_back(pointList[idx]);
            }

            centroid = squareCenter(neighbors);
        }
        else if (useCentroid)
        {
            neighbors = nearCellulePoints[i]; // Get the neighbors from the nearCellulePoints
            sortPointsCCW(neighbors);         // Sort neighbors in counter-clockwise order around the current point

            centroid = computeCentroid(neighbors);
        }
        else if (kNearest)
        {
            neighbors = kNearestPoints[i]; // Get the k-nearest neighbors from the kNearestPoints
            sortPointsCCW(neighbors);      // Sort neighbors in counter-clockwise order around the current point

            centroid = computeCentroid(neighbors);
        }
        else if (useOrientedBox)
        {
            auto neighborsIdx = pointsAdjacentsIdx[i];
            neighbors.clear(); // Get the neighbors from the nearCellulePoints
            for (const auto& idx : neighborsIdx)
            {
                neighbors.push_back(pointList[idx]);
            }

            // onvert neighbors to Point2D for oriented bounding box computation
            std::vector<Point2D> neighbors2D;
            neighbors2D.reserve(neighbors.size());
            for (const auto& neighbor : neighbors)
            {
                neighbors2D.emplace_back(neighbor.first, neighbor.second);
            }

            std::vector<Point2D> orientedBox = computeOrientedBoundingBox(neighbors2D);

            std::vector<Point> orientedBoxPointsV = {
                Point{orientedBox[0].x(), orientedBox[0].y()},
                Point{orientedBox[1].x(), orientedBox[1].y()},
                Point{orientedBox[2].x(), orientedBox[2].y()},
                Point{orientedBox[3].x(), orientedBox[3].y()}
            };

            sortPointsCCW(orientedBoxPointsV);
            centroid = computeCentroid(orientedBoxPointsV); // Calculate the centroid of the oriented bounding box

            // Make the midpoint of the oriented bounding box the centroid
            // centroid = {
            //     (orientedBox[0].x() + orientedBox[1].x() + orientedBox[2].x() + orientedBox[3].x()) / 4.0f,
            //     (orientedBox[0].y() + orientedBox[1].y() + orientedBox[2].y() + orientedBox[3].y()) / 4.0f
            // };

            // Store the oriented bounding box for later use
            std::array<Point, 4> orientedBoxPoints = {
                Point{orientedBox[0].x(), orientedBox[0].y()},
                Point{orientedBox[1].x(), orientedBox[1].y()},
                Point{orientedBox[2].x(), orientedBox[2].y()},
                Point{orientedBox[3].x(), orientedBox[3].y()}
            };
            allOrientedBoxes.emplace_back(orientedBoxPoints); // Store the oriented bounding box for later use
        }
        else if (useMean)
        {
            neighbors = nearCellulePoints[i]; // Get the neighbors from the nearCellulePoints

            centroid = computeCenter(neighbors);
        }
        else
        {
            std::cerr << "No valid method selected for centralisation. Skipping point (" << pointList[i].first << ", " << pointList[i].second << ").\n";
            continue; // Skip if no valid method is selected
        }

        pointList[i] = {
            (1 - step) * pointList[i].first + step * centroid.first,
            (1 - step) * pointList[i].second + step * centroid.second
        }; // Update the current point to the centroid
    }
    // }

    // std::cout << "Centralisation completed.\n";
}

void Graphe::doDelaunayAndCalculateCenters()
{
    std::vector<dt::Vector2<double>> points;
    points.reserve(pointList.size());
    for (const auto& point : pointList)
    {
        points.emplace_back(point.first, point.second); // Convert Point to dt::Vector2
    }

    dt::Delaunay<double>                    triangulation;
    const auto                              start = std::chrono::high_resolution_clock::now();
    const std::vector<dt::Triangle<double>> triangles =
        triangulation.triangulate(points);
    const auto                          end  = std::chrono::high_resolution_clock::now();
    const std::chrono::duration<double> diff = end - start;

    set_triangles(triangles); // Set the triangles in the graph

    nearCellulePoints.clear();                  // Clear the nearCellulePoints vector to prepare for new data
    nearCellulePointsList.clear();              // Clear the nearCellulePointsList vector to prepare for new data
    celluleBorder.clear();                      // Clear the celluleBorder vector to prepare for new data
    nearCellulePoints.resize(pointList.size()); // Resize the nearCellulePoints vector to match the number of points in the graph
    kNearestPoints.clear();                     // Clear the kNearestPoints vector to prepare for new data

    calculateCenterFromDelaunayTriangles(idxTriangles); // Calculate the centers of the circumcircles of the triangles and store them in nearCellulePoints

    // Determine if an original point is a border point

    // idxPointBorder.clear(); // Clear the idxPointBorder vector to prepare for new data
    // if (!hasDetectedBorder)
    findBorderPoints(); // Find the border points in the graph

    triesNearCellulePoints();
}

int Graphe::flipDelaunayTriangles()
{
    const auto start = std::chrono::high_resolution_clock::now();

    double timeGetIdx   = 0.0; // Initialize timeGetIndex to 0
    double timeFindSame = 0.0; // Initialize timeFindSame to 0
    double timeFlip     = 0.0; // Initialize timeFlip to 0

    nbrFlips = 0; // Reset the number of flips performed during the Delaunay triangulation

    for (int i = 0; i < idxTriangles.size(); ++i)
    {
        auto start2 = std::chrono::high_resolution_clock::now();

        int idxA = idxTriangles[i][0]; // Get the index of point A in the triangle
        int idxB = idxTriangles[i][1]; // Get the index of point B in the triangle
        int idxC = idxTriangles[i][2]; // Get the index of point C in the triangle

        Circle circle = triangleCircles[i];

        auto end2 = std::chrono::high_resolution_clock::now();
        timeGetIdx += std::chrono::duration<double>(end2 - start2).count(); // Accumulate the time taken to get indices

        if (idxA == -1 || idxB == -1 || idxC == -1)
        {
            continue; // Skip this triangle if any point is not found
        }

        start2 = std::chrono::high_resolution_clock::now();

        std::vector<int> adjacentPointsAB = {};
        for (const int& AneighborIdx : pointsAdjacentsIdx[idxA])
        {
            std::vector<int> adjacentPointsB = pointsAdjacentsIdx[idxB]; // Get the adjacent points of point B
            if (AneighborIdx != idxB
                && AneighborIdx != idxC
                && std::find(adjacentPointsB.begin(), adjacentPointsB.end(), AneighborIdx) != adjacentPointsB.end()) // Exclude points b and c
            {
                adjacentPointsAB.push_back(AneighborIdx);
            }
        }

        std::vector<int> adjacentPointsBC = {};
        for (const auto& Bneighbor : pointsAdjacentsIdx[idxB])
        {
            std::vector<int> adjacentPointsC = pointsAdjacentsIdx[idxC]; // Get the adjacent points of point C
            if (Bneighbor != idxC
                && Bneighbor != idxA
                && std::find(adjacentPointsC.begin(), adjacentPointsC.end(), Bneighbor) != adjacentPointsC.end()) // Exclude points c and a
            {
                adjacentPointsBC.push_back(Bneighbor);
            }
        }

        std::vector<int> adjacentPointsCA = {};
        for (const auto& Cneighbor : pointsAdjacentsIdx[idxC])
        {
            std::vector<int> adjacentPointsA = pointsAdjacentsIdx[idxA]; // Get the adjacent points of point A
            if (Cneighbor != idxA
                && Cneighbor != idxB
                && std::find(adjacentPointsA.begin(), adjacentPointsA.end(), Cneighbor) != adjacentPointsA.end()) // Exclude points a and b
            {
                adjacentPointsCA.push_back(Cneighbor);
            }
        }

        end2 = std::chrono::high_resolution_clock::now();
        timeFindSame += std::chrono::duration<double>(end2 - start2).count();

        start2 = std::chrono::high_resolution_clock::now();

        if (adjacentPointsAB.size() == 0 || adjacentPointsBC.size() == 0 || adjacentPointsCA.size() == 0)
        {
            continue; // Skip this triangle if any point is not found
        }

        // std::cout << "Idx of adjacent points AB: " << adjacentPointsAB;
        // std::cout << " Idx of adjacent points BC: " << adjacentPointsBC;
        // std::cout << " Idx of adjacent points CA: " << adjacentPointsCA << "\n";

        // Check if the circumcircle contains any of the adjacent points

        auto fct_flip = [&](const std::vector<int>& adjList) -> void {
            for (auto& neighborIdx : adjList)
            {
                if (distance(pointList[neighborIdx], circle.first) < circle.second) // Check if the neighbor is inside the circumcircle
                {
                    for (int j = 0; j < idxTriangles.size(); ++j)
                    {
                        if (std::find(idxTriangles[j].begin(), idxTriangles[j].end(), neighborIdx) != idxTriangles[j].end()
                            && std::find(idxTriangles[j].begin(), idxTriangles[j].end(), idxA) != idxTriangles[j].end()
                            && std::find(idxTriangles[j].begin(), idxTriangles[j].end(), idxB) != idxTriangles[j].end())
                        {
                            idxTriangles[i] = {idxC, neighborIdx, idxA}; // Swap the neighbor with point A in the current triangle
                            idxTriangles[j] = {idxC, neighborIdx, idxB}; // Swap the neighbor with point B in the adjacent triangle

                            auto computeCircumcircle = [&](const std::array<int, 3>& triangle) -> Circle {
                                Point p1 = pointList[triangle[0]];
                                Point p2 = pointList[triangle[1]];
                                Point p3 = pointList[triangle[2]];

                                std::array<std::array<float, 3>, 3> matForX{{{1.f, p1.second, static_cast<float>((std::pow(p1.second, 2) + std::pow(p1.first, 2)) / 2.)}, {1.f, p2.second, static_cast<float>((std::pow(p2.second, 2) + std::pow(p2.first, 2)) / 2.)}, {1.f, p3.second, static_cast<float>((std::pow(p3.second, 2) + std::pow(p3.first, 2)) / 2.)}}};
                                auto                                x = static_cast<float>(-determinant3x3(matForX));

                                std::array<std::array<float, 3>, 3> matForY{{{1.f, static_cast<float>(p1.first), static_cast<float>((std::pow(p1.second, 2) + std::pow(p1.first, 2)) / 2.)}, {1.f, static_cast<float>(p2.first), static_cast<float>((std::pow(p2.second, 2) + std::pow(p2.first, 2)) / 2.)}, {1.f, static_cast<float>(p3.first), static_cast<float>((std::pow(p3.second, 2) + std::pow(p3.first, 2)) / 2.)}}};
                                auto                                y = static_cast<float>(determinant3x3(matForY));

                                std::array<std::array<float, 3>, 3> matForW{{{1.f, static_cast<float>(p1.first), static_cast<float>(p1.second)}, {1.f, static_cast<float>(p2.first), static_cast<float>(p2.second)}, {1.f, static_cast<float>(p3.first), static_cast<float>(p3.second)}}};
                                auto                                w = static_cast<float>(determinant3x3(matForW));

                                Point center(x / w, y / w);
                                float radius = std::sqrt(std::pow(p1.first - center.first, 2) + std::pow(p1.second - center.second, 2));
                                return Circle(center, radius);
                            };

                            std::array<int, 3> triangleI = idxTriangles[i];
                            std::array<int, 3> triangleJ = idxTriangles[j];
                            triangleCircles[i]           = computeCircumcircle(triangleI);
                            triangleCircles[j]           = computeCircumcircle(triangleJ); // Recompute the circumcircle for the new triangles

                            nbrFlips++; // Increment the number of flips performed
                        }
                    }
                }
            }
        };

        fct_flip(adjacentPointsAB); // Flip triangles for adjacent points AB
        fct_flip(adjacentPointsBC); // Flip triangles for adjacent points BC
        fct_flip(adjacentPointsCA); // Flip triangles for adjacent points CA

        end2 = std::chrono::high_resolution_clock::now();
        timeFlip += std::chrono::duration<double>(end2 - start2).count();
    }

    const auto                          end  = std::chrono::high_resolution_clock::now();
    const std::chrono::duration<double> diff = end - start;
    std::cout << "Delaunay search flip completed in " << diff.count() << " seconds.\n";

    std::cout << "Time taken to get indices: " << timeGetIdx << " seconds.\n";
    std::cout << "Time taken to find adjacent points: " << timeFindSame << " seconds.\n";
    std::cout << "Time taken to flip triangles: " << timeFlip << " seconds.\n";

    std::cout << "Number of flips performed: " << nbrFlips << "\n";

    return nbrFlips; // Return the number of flips performed during the Delaunay triangulation
}

void Graphe::doDelaunayFlipVersion(int& nbrFlips)
{
    nbrFlips = flipDelaunayTriangles(); // Perform Delaunay triangulation and flip triangles

    set_triangle_v2(); // Set the triangles in the graph

    // Calculate the circumcenter of each triangle and add this center for Voronoil cellule points

    // std::cout << "Calcul for Voronoil : " << "\n";

    nearCellulePoints.clear();                  // Clear the nearCellulePoints vector to prepare for new data
    nearCellulePointsList.clear();              // Clear the nearCellulePointsList vector to prepare for new data
    celluleBorder.clear();                      // Clear the celluleBorder vector to prepare for new data
    nearCellulePoints.resize(pointList.size()); // Resize the nearCellulePoints vector to match the number of points in the graph
    kNearestPoints.clear();                     // Clear the kNearestPoints vector to prepare for new data

    calculateCenterFromDelaunayTriangles(idxTriangles); // Calculate the centers of the circumcircles of the triangles and store them in nearCellulePoints

    // Determine if an original point is a border point

    // idxPointBorder.clear(); // Clear the idxPointBorder vector to prepare for new data
    // if (!hasDetectedBorder)
    findBorderPoints(); // Find the border points in the graph

    triesNearCellulePoints();
}
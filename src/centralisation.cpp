#include <chrono>
#include <iostream>
#include "LlyodCentralisation.hpp"

using Point = std::pair<float, float>; // Représente un point (x, y)

// Fonction utilitaire pour calculer le centre (barycentre simple) des points
Point computeCenter(const std::vector<Point>& points)
{
    float cx = 0.0f, cy = 0.0f;
    for (const auto& p : points)
    {
        cx += p.first;
        cy += p.second;
    }
    cx /= points.size();
    cy /= points.size();
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
    double cx = 0.0, cy = 0.0;
    int    n = vertices.size();

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

void Graphe::centralisation()
{
    std::cout << "Centralisation of points in the graph...\n";

    // if (!hasDetectedBorder)
    // {
    //     std::cout << "Border Center Points :\n";
    //     for (auto& center : celluleBorder)
    //     {
    //         std::cout << "Center: (" << center.first << ", " << center.second << ")\n";
    //     }

    //     for (int i = 0; i < pointList.size(); ++i)
    //     {
    //         Point              currentPoint = pointList[i];         // Get the current point from the graph
    //         std::vector<Point> neighbors    = nearCellulePoints[i]; // Get the neighbors of the current point

    //         std::cout << "------ Processing point (" << currentPoint.first << ", " << currentPoint.second << ") ------\n";
    //         for (auto& neighbor : neighbors)
    //         {
    //             std::cout << "Neighbor: (" << neighbor.first << ", " << neighbor.second << ")\n";
    //         }

    //         if (neighbors.empty())
    //         {
    //             std::cerr << "No neighbors found for point (" << currentPoint.first << ", " << currentPoint.second << "). Skipping centralisation.\n";
    //             continue; // Skip if no neighbors are found
    //         }

    //         if (neighbors.size() < 3)
    //         {
    //             std::cerr << "Not enough neighbors for point (" << currentPoint.first << ", " << currentPoint.second << "). It's a border point.\n";
    //             idxPointBorder.emplace_back(i); // Add the current point to the border points if it has no neighbors
    //             continue;                       // Skip if there are not enough neighbors to form a polygon
    //         }

    //         int compteurOfBorderNeighbors = 0; // Counter for border neighbors
    //         for (auto& neighbor : neighbors)
    //         {
    //             if (std::find(celluleBorder.begin(), celluleBorder.end(), neighbor) != celluleBorder.end())
    //                 compteurOfBorderNeighbors++; // Increment the counter if the neighbor is a border point
    //         }

    //         if (compteurOfBorderNeighbors == 2)
    //         {
    //             std::cerr << "Point (" << currentPoint.first << ", " << currentPoint.second << ") has " << compteurOfBorderNeighbors
    //                       << " border neighbors. It's a border point.\n";
    //             idxPointBorder.emplace_back(i); // Add the current point to the border points if it has no neighbors
    //             continue;                       // Skip if there are border neighbors
    //         }

    //         sortPointsCCW(neighbors); // Sort neighbors in counter-clockwise order around the current point

    //         pointList[i] = computeCentroid(neighbors); // Update the current point to the centroid
    //     }
    //     hasDetectedBorder = true; // Set the flag to indicate that border points have been detected
    // }
    // else
    // {
    for (int i = 0; i < pointList.size(); ++i)
    {
        if (std::find(idxPointBorder.begin(), idxPointBorder.end(), i) != idxPointBorder.end())
        {
            std::cerr << "Skipping centralisation for border point (" << pointList[i].first << ", " << pointList[i].second << ").\n";
            continue; // Skip centralisation for border points
        }

        std::vector<Point> neighbors = nearCellulePoints[i]; // Get the neighbors of the current point

        sortPointsCCW(neighbors); // Sort neighbors in counter-clockwise order around the current point

        pointList[i] = computeCentroid(neighbors); // Update the current point to the centroid
    }
    // }

    std::cout << "Centralisation completed.\n";
}

void Graphe::doDelaunayAndCalculateCenters()
{
    std::vector<dt::Vector2<double>> points;
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

    std::cout << triangles.size() << " triangles generated in " << diff.count()
              << "s\n";
    std::vector<dt::Edge<double>> edges = triangulation.getEdges();

    std::cout
        << "--------------------------------------------------------------\n";

    set_triangles(triangles); // Set the triangles in the graph

    // Calculate the circumcenter of each triangle and add this center for Voronoil cellule points

    std::cout << "Calcul for Voronoil : " << "\n";

    nearCellulePoints.clear();                  // Clear the nearCellulePoints vector to prepare for new data
    nearCellulePointsList.clear();              // Clear the nearCellulePointsList vector to prepare for new data
    celluleBorder.clear();                      // Clear the celluleBorder vector to prepare for new data
    nearCellulePoints.resize(pointList.size()); // Resize the nearCellulePoints vector to match the number of points in the graph

    calculateCenterFromDelaunayTriangles(triangles); // Calculate the centers of the circumcircles of the triangles and store them in nearCellulePoints

    // Determine if an original point is a border point

    if (!hasDetectedBorder)
        findBorderPoints(); // Find the border points in the graph
}
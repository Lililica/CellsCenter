#include <algorithm>
#include <iostream>
#include "LlyodCentralisation.hpp"
#include "utils.hpp"

void Graphe::calculateCenterFromDelaunayTriangles(const std::vector<dt::Triangle<double>>& triangles)
{
    for (const auto& triangle : triangles)
    {
        std::array<std::array<float, 3>, 3> matForX{{{1.f, static_cast<float>(triangle.a->y), static_cast<float>((std::pow(triangle.a->y, 2) + std::pow(triangle.a->x, 2)) / 2.)}, {1.f, static_cast<float>(triangle.b->y), static_cast<float>((std::pow(triangle.b->y, 2) + std::pow(triangle.b->x, 2)) / 2.)}, {1.f, static_cast<float>(triangle.c->y), static_cast<float>((std::pow(triangle.c->y, 2) + std::pow(triangle.c->x, 2)) / 2.)}}};

        auto x = static_cast<float>(-determinant3x3(matForX));

        std::array<std::array<float, 3>, 3> matForY{{{1.f, static_cast<float>(triangle.a->x), static_cast<float>((std::pow(triangle.a->y, 2) + std::pow(triangle.a->x, 2)) / 2.)}, {1.f, static_cast<float>(triangle.b->x), static_cast<float>((std::pow(triangle.b->y, 2) + std::pow(triangle.b->x, 2)) / 2.)}, {1.f, static_cast<float>(triangle.c->x), static_cast<float>((std::pow(triangle.c->y, 2) + std::pow(triangle.c->x, 2)) / 2.)}}};

        auto y = static_cast<float>(determinant3x3(matForY));

        std::array<std::array<float, 3>, 3> matForW{{{1.f, static_cast<float>(triangle.a->x), static_cast<float>(triangle.a->y)}, {1.f, static_cast<float>(triangle.b->x), static_cast<float>(triangle.b->y)}, {1.f, static_cast<float>(triangle.c->x), static_cast<float>(triangle.c->y)}}};

        auto w = static_cast<float>(determinant3x3(matForW));

        Point center(x / w, y / w); // Calculate the center of the circumcircle
        // std::cout << "Center of circumcircle for triangle with points ("
        //           << triangle.a->x << ", " << triangle.a->y << "), ("
        //           << triangle.b->x << ", " << triangle.b->y << "), ("
        //           << triangle.c->x << ", " << triangle.c->y << ") is at ("
        //           << center.first << ", " << center.second << ")\n";

        int aIdx = getIndexFromPoint(Point(triangle.a->x, triangle.a->y)); // Get the index of the point in the graph
        int bIdx = getIndexFromPoint(Point(triangle.b->x, triangle.b->y)); // Get the index of the point in the graph
        int cIdx = getIndexFromPoint(Point(triangle.c->x, triangle.c->y)); // Get the index of the point in the graph

        if (aIdx == -1 || bIdx == -1 || cIdx == -1)
        {
            std::cerr << "Error: Point not found in the graph." << '\n';
            continue; // Skip this triangle if any point is not found
        }

        nearCellulePoints[aIdx].emplace_back(center); // Add the center to the nearCellulePoints of point a
        nearCellulePoints[bIdx].emplace_back(center); // Add the center to the nearCellulePoints of point b
        nearCellulePoints[cIdx].emplace_back(center); // Add the center to the nearCellulePoints of point c

        nearCellulePointsList.emplace_back(center); // Add the center to the nearCellulePointsList
    }
}

bool Graphe::hasOtherTriangleForSegment(const std::vector<std::array<Point, 3>>& trianglesPoints, const Point& p1, const Point& p2, const Point& excluded)
{
    for (const auto& triangle : trianglesPoints)
    {
        if (pointEqual(triangle[0], excluded) || pointEqual(triangle[1], excluded) || pointEqual(triangle[2], excluded))
            continue;
        if ((pointEqual(triangle[0], p1) && pointEqual(triangle[1], p2))
            || (pointEqual(triangle[0], p2) && pointEqual(triangle[1], p1))
            || (pointEqual(triangle[0], p1) && pointEqual(triangle[2], p2))
            || (pointEqual(triangle[0], p2) && pointEqual(triangle[2], p1))
            || (pointEqual(triangle[1], p1) && pointEqual(triangle[2], p2))
            || (pointEqual(triangle[1], p2) && pointEqual(triangle[2], p1)))
        {
            // std::cout << "Found triangle : (" << triangle[0].first << ", " << triangle[0].second << "), ("
            //           << triangle[1].first << ", " << triangle[1].second << "), ("
            //           << triangle[2].first << ", " << triangle[2].second << ") which work\n";
            return true;
        }
    }
    return false;
}

void Graphe::findBorderPoints()
{
#if 0
    for (auto& point : pointList)
    {
        float limitX = 10.f; // Define the limit for X coordinate
        float limitY = 10.f; // Define the limit for Y coordinate

        if (point.first < -limitX || point.first > limitX || point.second < -limitY || point.second > limitY)
        {
            // std::cerr << "Point (" << point.first << ", " << point.second << ") is outside the defined limits. Skipping centralisation.\n";
            point.first  = std::max(point.first, -limitX);         // Clamp the point to the limit
            point.first  = std::min(point.first, limitX);          // Clamp the point to the limit
            point.second = std::max(point.second, -limitY);        // Clamp the point to the limit
            point.second = std::min(point.second, limitY);         // Clamp the point to the limit
            idxPointBorder.emplace_back(getIndexFromPoint(point)); // Add the index of the border point to the list
            continue;                                              // Skip points that are outside the defined limits
        }
    }
#else
    for (auto& point : pointList)
    {
        float radius = 10.f; // Define the radius of the circle

        float dist = std::sqrt(point.first * point.first + point.second * point.second);
        if (dist > radius - 0.001f)
        {
            // Clamp the point to the circle boundary
            float angle  = std::atan2(point.second, point.first);  // Calculate the angle of the point
            point.first  = std::cos(angle) * radius;               // Scale the point to the circle boundary
            point.second = std::sin(angle) * radius;               // Scale the point to the circle boundary
            idxPointBorder.emplace_back(getIndexFromPoint(point)); // Add the index of the border point to the list
            continue;                                              // Skip points that are outside the defined circle
        }
    }
#endif

    // for (int i = 0; i < nearCellulePointsList.size(); ++i)
    // {
    //     Point center = nearCellulePointsList[i]; // Get the current center point

    //     // std::cout << "__________________________________________________________\n";
    //     // std::cout << "Center of circumcircle at (" << center.first << ", " << center.second << ")\n";

    //     std::array<Point, 3> currentTriangle;
    //     int                  IdxTriangle = 0; // Index for triangle points

    //     for (int j = 0; j < nearCellulePoints.size(); ++j)
    //     {
    //         std::vector<Point> currentCenterList = nearCellulePoints[j];

    //         if (std::find(currentCenterList.begin(), currentCenterList.end(), center) != currentCenterList.end())
    //         {
    //             currentTriangle[IdxTriangle] = pointList[j]; // Add the first point of the triangle
    //             IdxTriangle++;                               // Increment the index for the next triangle point
    //         }
    //     }

    //     if (IdxTriangle < 3) // If we don't have enough points to form a triangle
    //     {
    //         std::cout << "ERROR : Not enough points to form a triangle for center at (" << center.first << ", " << center.second << ").\n";
    //         continue; // Skip this center
    //     }

    //     Point a = currentTriangle[0]; // First point of the triangle
    //     Point b = currentTriangle[1]; // Second point of the triangle
    //     Point c = currentTriangle[2]; // Third point of the triangle

    //     // std::cout << "Triangle points: (" << a.first << ", " << a.second << "), ("
    //     //           << b.first << ", " << b.second << "), (" << c.first << ", " << c.second << ")\n";

    //     bool isBorder = true;

    //     // Check for each segment if there is another triangle not containing the third point

    //     // std::cout << "Checking segments for border points...\n";
    //     // std::cout << "Segment AB: (" << a.first << ", " << a.second << ") to (" << b.first << ", " << b.second << ")\n";
    //     if (hasOtherTriangleForSegment(trianglesPoints, a, b, c))
    //         isBorder = false;
    //     if (isBorder)
    //     {
    //         // std::cout << "Center of circumcircle at (" << center.first << ", " << center.second << ") is a border point.\n";
    //         // std::cout << "__________________________________________________________\n";
    //         celluleBorder.emplace_back(center); // Add the index of the border center to the list
    //         if (std::find(idxPointBorder.begin(), idxPointBorder.end(), getIndexFromPoint(a)) == idxPointBorder.end())
    //             idxPointBorder.emplace_back(getIndexFromPoint(a)); // Add the index of point a to the border points if not already present
    //         if (std::find(idxPointBorder.begin(), idxPointBorder.end(), getIndexFromPoint(b)) == idxPointBorder.end())
    //             idxPointBorder.emplace_back(getIndexFromPoint(b)); // Add the index of point b to the border points if not already present
    //         continue;
    //     }
    //     isBorder = true;
    //     if (hasOtherTriangleForSegment(trianglesPoints, a, c, b))
    //         isBorder = false;
    //     if (isBorder)
    //     {
    //         // std::cout << "Center of circumcircle at (" << center.first << ", " << center.second << ") is a border point.\n";
    //         // std::cout << "__________________________________________________________\n";
    //         celluleBorder.emplace_back(center); // Add the index of the border center to the list
    //         if (std::find(idxPointBorder.begin(), idxPointBorder.end(), getIndexFromPoint(a)) == idxPointBorder.end())
    //             idxPointBorder.emplace_back(getIndexFromPoint(a)); // Add the index of point a to the border points if not already present
    //         if (std::find(idxPointBorder.begin(), idxPointBorder.end(), getIndexFromPoint(c)) == idxPointBorder.end())
    //             idxPointBorder.emplace_back(getIndexFromPoint(c)); // Add the index of point c to the border points if not already present
    //         continue;
    //     }
    //     isBorder = true;
    //     if (hasOtherTriangleForSegment(trianglesPoints, b, c, a))
    //         isBorder = false;
    //     if (isBorder)
    //     {
    //         // std::cout << "Center of circumcircle at (" << center.first << ", " << center.second << ") is a border point.\n";
    //         celluleBorder.emplace_back(center); // Add the index of the border center to the list
    //         if (std::find(idxPointBorder.begin(), idxPointBorder.end(), getIndexFromPoint(b)) == idxPointBorder.end())
    //             idxPointBorder.emplace_back(getIndexFromPoint(b)); // Add the index of point b to the border points if not already present
    //         if (std::find(idxPointBorder.begin(), idxPointBorder.end(), getIndexFromPoint(c)) == idxPointBorder.end())
    //             idxPointBorder.emplace_back(getIndexFromPoint(c)); // Add the index of point c to the border points if not already present
    //     }

    //     // std::cout << "__________________________________________________________\n";
    // }
}

void Graphe::findBorderPoints_BROKEN_Function()
{
    // Calculate the nearest point from a polygon list point doesn't give the neighborhood of the point in a polygon... This function doesn't do what we want.

    for (const auto& center : nearCellulePointsList)
    {
        std::cout << "__________________________________________________________\n";
        std::cout << "Center of circumcircle at (" << center.first << ", " << center.second << ")\n";
        bool                                   isBorder = true; // Flag to check if the center is a border point
        std::array<std::pair<Point, float>, 2> firstTwoNeighbors{
            std::make_pair(Point(0.f, 0.f), std::numeric_limits<float>::max()), // First neighbor initialized to a point at (0,0) with max distance
            std::make_pair(Point(0.f, 0.f), std::numeric_limits<float>::max())  // Second neighbor initialized to a point at (0,0) with max distance
        };

        for (int i = 0; i < pointList.size(); ++i)
        {
            if (!isBorder) // If the center is already determined to be a border point, skip further checks
                break;

            std::vector<Point> currentCenterList = nearCellulePoints[i];

            if (std::find(currentCenterList.begin(), currentCenterList.end(), center) != currentCenterList.end())
            {
                if (currentCenterList.size() == 1 || currentCenterList.size() == 2) // If there are no centers for this point, skip to the next point
                    break;

                if (firstTwoNeighbors[0].second == std::numeric_limits<float>::max() && firstTwoNeighbors[1].second == std::numeric_limits<float>::max()) // If the first two neighbors are not set
                {
                    std::cout << "First two neighbors not set, calculating... \n";

                    if (!pointEqual(currentCenterList[0], center)) // If the center is found in the neighborhood of the point
                    {
                        firstTwoNeighbors[0].first  = currentCenterList[0];                   // Set the first neighbor
                        firstTwoNeighbors[0].second = distance(center, currentCenterList[0]); // Set the distance to the first neighbor
                    }
                    else
                    {
                        firstTwoNeighbors[0].first  = currentCenterList[2]; // Set the first neighbor
                        firstTwoNeighbors[0].second = distance(center, currentCenterList[2]);
                    }

                    if (!pointEqual(currentCenterList[1], center)) // If the center is found in the neighborhood of the point
                    {
                        firstTwoNeighbors[1].first  = currentCenterList[1];                   // Set the first neighbor
                        firstTwoNeighbors[1].second = distance(center, currentCenterList[1]); // Set the distance to the first neighbor
                    }
                    else
                    {
                        firstTwoNeighbors[1].first  = currentCenterList[2]; // Set the first neighbor
                        firstTwoNeighbors[1].second = distance(center, currentCenterList[2]);
                    }

                    if (firstTwoNeighbors[0].second > firstTwoNeighbors[1].second) // If the first neighbor is further than the second neighbor
                    {
                        std::swap(firstTwoNeighbors[0], firstTwoNeighbors[1]); // Swap the neighbors
                    }

                    std::cout << "First choice set: (" << firstTwoNeighbors[0].first.first << ", " << firstTwoNeighbors[0].first.second << ") "
                              << "and (" << firstTwoNeighbors[1].first.first << ", " << firstTwoNeighbors[1].first.second << ")\n";

                    // Determine his neighborhood
                    for (const auto& neighbor : currentCenterList)
                    {
                        if (pointEqual(neighbor, center) || pointEqual(neighbor, firstTwoNeighbors[0].first)) // If the center is found in the neighborhood of the point
                            continue;                                                                         // Skip the center itself

                        float dist = distance(center, neighbor); // Calculate the distance to the neighbor

                        if (dist < firstTwoNeighbors[0].second)      // If the distance is less than the first neighbor
                            firstTwoNeighbors[0] = {neighbor, dist}; // Set the new first neighbor

                        else if (dist < firstTwoNeighbors[1].second) // If the distance is less than the second neighbor
                            firstTwoNeighbors[1] = {neighbor, dist}; // Set the new second neighbor
                    }

                    Point firstNeighbor  = firstTwoNeighbors[0].first; // Get the first neighbor
                    Point secondNeighbor = firstTwoNeighbors[1].first; // Get the second neighbor

                    std::cout << "First neighbor: (" << firstNeighbor.first << ", " << firstNeighbor.second << ") "
                              << "Second neighbor: (" << secondNeighbor.first << ", " << secondNeighbor.second << ")\n";
                    std::cout << "______________                     ________\n";
                }
                else
                {
                    std::array<std::pair<Point, float>, 2> firstTwoNeighborsTEMP;

                    if (!pointEqual(currentCenterList[0], center)) // If the center is found in the neighborhood of the point
                    {
                        firstTwoNeighborsTEMP[0].first  = currentCenterList[0];                   // Set the first neighbor
                        firstTwoNeighborsTEMP[0].second = distance(center, currentCenterList[0]); // Set the distance to the first neighbor
                    }
                    else
                    {
                        firstTwoNeighborsTEMP[0].first  = currentCenterList[2]; // Set the first neighbor
                        firstTwoNeighborsTEMP[0].second = distance(center, currentCenterList[2]);
                    }

                    if (!pointEqual(currentCenterList[1], center)) // If the center is found in the neighborhood of the point
                    {
                        firstTwoNeighborsTEMP[1].first  = currentCenterList[1];                   // Set the first neighbor
                        firstTwoNeighborsTEMP[1].second = distance(center, currentCenterList[1]); // Set the distance to the first neighbor
                    }
                    else
                    {
                        firstTwoNeighborsTEMP[1].first  = currentCenterList[2]; // Set the first neighbor
                        firstTwoNeighborsTEMP[1].second = distance(center, currentCenterList[2]);
                    }

                    if (firstTwoNeighborsTEMP[0].second > firstTwoNeighborsTEMP[1].second) // If the first neighbor is further than the second neighbor
                    {
                        std::swap(firstTwoNeighborsTEMP[0], firstTwoNeighborsTEMP[1]); // Swap the neighbors
                    }

                    // Determine his neighborhood
                    for (const auto& neighbor : currentCenterList)
                    {
                        if (pointEqual(neighbor, center) || pointEqual(neighbor, firstTwoNeighborsTEMP[0].first)) // If the center is found in the neighborhood of the point
                            continue;                                                                             // Skip the center itself

                        float dist = distance(center, neighbor); // Calculate the distance to the neighbor

                        if (dist < firstTwoNeighborsTEMP[0].second)      // If the distance is less than the first neighbor
                            firstTwoNeighborsTEMP[0] = {neighbor, dist}; // Set the new first neighbor

                        else if (dist < firstTwoNeighborsTEMP[1].second) // If the distance is less than the second neighbor
                            firstTwoNeighborsTEMP[1] = {neighbor, dist}; // Set the new second neighbor
                    }

                    Point firstNeighbor      = firstTwoNeighbors[0].first;     // Get the first neighbor
                    Point secondNeighbor     = firstTwoNeighbors[1].first;     // Get the second neighbor
                    Point firstNeighborTEMP  = firstTwoNeighborsTEMP[0].first; // Get the first neighbor temporary
                    Point secondNeighborTEMP = firstTwoNeighborsTEMP[1].first; // Get the second neighbor temporary

                    std::cout << "First neighbor: (" << firstNeighbor.first << ", " << firstNeighbor.second << ") "
                              << "Second neighbor: (" << secondNeighbor.first << ", " << secondNeighbor.second << ")\n";
                    std::cout << "First neighbor TEMP: (" << firstNeighborTEMP.first << ", " << firstNeighborTEMP.second << ") "
                              << "Second neighbor TEMP: (" << secondNeighborTEMP.first << ", " << secondNeighborTEMP.second << ")\n";

                    // If the first neighbor is not the same as the first neighbor temporary
                    if (!pointEqual(firstNeighbor, firstNeighborTEMP) || !pointEqual(secondNeighbor, secondNeighborTEMP)
                        || !pointEqual(firstNeighbor, secondNeighborTEMP) || !pointEqual(secondNeighbor, firstNeighborTEMP))
                        isBorder = false; // The center is not a border point
                }
            }
        }

        if (isBorder) // If the center is a border point
        {
            std::cout << "Center of circumcircle at (" << center.first << ", " << center.second << ") is a border point.\n";
            std::cout << "__________________________________________________________\n";
        }
    }

    // TO DO:
    // Add the border points to a list or vector for further processing
}

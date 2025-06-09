#include "LlyodCentralisation.hpp"
#include "utils.hpp"




void Graphe::calculateCenterFromDelaunayTriangles(const std::vector<dt::Triangle<double>>& triangles){
    for(const auto& triangle : triangles)
    {
        std::array<std::array<float,3>,3> matForX {{
            {1.f, static_cast<float>(triangle.a->y), static_cast<float>((std::pow(triangle.a->y,2) + std::pow(triangle.a->x,2))/2.)},
            {1.f, static_cast<float>(triangle.b->y), static_cast<float>((std::pow(triangle.b->y,2) + std::pow(triangle.b->x,2))/2.)},
            {1.f, static_cast<float>(triangle.c->y), static_cast<float>((std::pow(triangle.c->y,2) + std::pow(triangle.c->x,2))/2.)}
        }};

        float x = -determinant3x3(matForX);

        std::array<std::array<float,3>,3> matForY {{
            { 1.f, static_cast<float>(triangle.a->x), static_cast<float>((std::pow(triangle.a->y,2) + std::pow(triangle.a->x,2))/2.)},
            { 1.f, static_cast<float>(triangle.b->x), static_cast<float>((std::pow(triangle.b->y,2) + std::pow(triangle.b->x,2))/2.)},
            { 1.f,static_cast<float>(triangle.c->x), static_cast<float>((std::pow(triangle.c->y,2) + std::pow(triangle.c->x,2))/2.)}
        }};

        float y = determinant3x3(matForY);

        std::array<std::array<float,3>,3> matForW {{
            { 1.f, static_cast<float>(triangle.a->x), static_cast<float>(triangle.a->y)},
            { 1.f, static_cast<float>(triangle.b->x), static_cast<float>(triangle.b->y)},
            { 1.f, static_cast<float>(triangle.c->x), static_cast<float>(triangle.c->y)}
        }};

        float w = determinant3x3(matForW);

        Point center(x/w, y/w); // Calculate the center of the circumcircle
        std::cout << "Center of circumcircle for triangle with points ("
                  << triangle.a->x << ", " << triangle.a->y << "), ("
                  << triangle.b->x << ", " << triangle.b->y << "), ("
                  << triangle.c->x << ", " << triangle.c->y << ") is at ("
                  << center.first << ", " << center.second << ")\n";



        int aIdx = getIndexFromPoint(Point(triangle.a->x, triangle.a->y)); // Get the index of the point in the graph
        int bIdx = getIndexFromPoint(Point(triangle.b->x, triangle.b->y)); // Get the index of the point in the graph
        int cIdx = getIndexFromPoint(Point(triangle.c->x, triangle.c->y)); // Get the index of the point in the graph

        if (aIdx == -1 || bIdx == -1 || cIdx == -1)
        {
            std::cerr << "Error: Point not found in the graph." << std::endl;
            continue; // Skip this triangle if any point is not found
        }

        nearCellulePoints[aIdx].emplace_back(center); // Add the center to the nearCellulePoints of point a
        nearCellulePoints[bIdx].emplace_back(center); // Add the center to the nearCellulePoints of point b
        nearCellulePoints[cIdx].emplace_back(center); // Add the center to the nearCellulePoints of point c

        nearCellulePointsList.emplace_back(center); // Add the center to the nearCellulePointsList
    }
}


void Graphe::findBorderPoints()
{
   for(const auto& center : nearCellulePointsList)
    {
        std::cout << "Center of circumcircle at (" << center.first << ", " << center.second << ")\n";
        bool isBorder = true; // Flag to check if the center is a border point
        std::array<std::pair<Point,float>, 2> firstTwoNeighbors
        {
            std::make_pair(Point(0.f, 0.f), std::numeric_limits<float>::max()), // First neighbor initialized to a point at (0,0) with max distance
            std::make_pair(Point(0.f, 0.f), std::numeric_limits<float>::max())  // Second neighbor initialized to a point at (0,0) with max distance
        };

        for(int i = 0; i < pointList.size(); ++i)
        {

            if(!isBorder) // If the center is already determined to be a border point, skip further checks
                break;

            std::vector<Point> currentCenterList = nearCellulePoints[i];
            
 


            if (std::find(currentCenterList.begin(), currentCenterList.end(), center) != currentCenterList.end())
            {
                if(currentCenterList.size() == 1 || currentCenterList.size() == 2) // If there are no centers for this point, skip to the next point
                    break;

                if(firstTwoNeighbors[0].second == std::numeric_limits<float>::max() && firstTwoNeighbors[1].second == std::numeric_limits<float>::max()) // If the first two neighbors are not set
                {
                    firstTwoNeighbors[0].first = currentCenterList[0]; // Set the first neighbor
                    firstTwoNeighbors[0].second = distance(center, currentCenterList[0]); // Set the distance to the first neighbor

                    firstTwoNeighbors[1].first = currentCenterList[1]; // Set the second neighbor
                    firstTwoNeighbors[1].second = distance(center, currentCenterList[1]); // Set the distance to the second neighbor

                    if(firstTwoNeighbors[0].second > firstTwoNeighbors[1].second) // If the first neighbor is further than the second neighbor
                    {
                        std::swap(firstTwoNeighbors[0], firstTwoNeighbors[1]); // Swap the neighbors
                    }

                    // Determine his neighborhood
                    for (const auto& neighbor : currentCenterList)
                    {
                        if (pointEqual(neighbor, center)) // If the center is found in the neighborhood of the point
                            continue; // Skip the center itself
                        
                        float dist = distance(center, neighbor); // Calculate the distance to the neighbor

                        if (dist < firstTwoNeighbors[0].second) // If the distance is less than the first neighbor
                            firstTwoNeighbors[0] = {neighbor, dist}; // Set the new first neighbor

                        else if (dist < firstTwoNeighbors[1].second) // If the distance is less than the second neighbor
                            firstTwoNeighbors[1] = {neighbor, dist}; // Set the new second neighbor
                    }

                    Point firstNeighbor = firstTwoNeighbors[0].first; // Get the first neighbor
                    Point secondNeighbor = firstTwoNeighbors[1].first; // Get the second neighbor

                    std::cout << "First neighbor: (" << firstNeighbor.first << ", " << firstNeighbor.second << ") "
                              << "Second neighbor: (" << secondNeighbor.first << ", " << secondNeighbor.second << ")\n";
                }else
                {
                    std::array<std::pair<Point,float>, 2> firstTwoNeighborsTEMP;

                    firstTwoNeighborsTEMP[0].first = currentCenterList[0]; // Set the first neighbor
                    firstTwoNeighborsTEMP[0].second = distance(center, currentCenterList[0]); // Set the distance to the first neighbor

                    firstTwoNeighborsTEMP[1].first = currentCenterList[1]; // Set the second neighbor
                    firstTwoNeighborsTEMP[1].second = distance(center, currentCenterList[1]); // Set the distance to the second neighbor

                    // Determine his neighborhood
                    for (const auto& neighbor : currentCenterList)
                    {
                        if (pointEqual(neighbor, center)) // If the center is found in the neighborhood of the point
                            continue; // Skip the center itself
                        
                        float dist = distance(center, neighbor); // Calculate the distance to the neighbor

                        if (dist < firstTwoNeighborsTEMP[0].second) // If the distance is less than the first neighbor
                            firstTwoNeighborsTEMP[0] = {neighbor, dist}; // Set the new first neighbor

                        else if (dist < firstTwoNeighborsTEMP[1].second) // If the distance is less than the second neighbor
                            firstTwoNeighborsTEMP[1] = {neighbor, dist}; // Set the new second neighbor
                    }

                    Point firstNeighbor = firstTwoNeighbors[0].first; // Get the first neighbor
                    Point secondNeighbor = firstTwoNeighbors[1].first; // Get the second neighbor
                    Point firstNeighborTEMP = firstTwoNeighborsTEMP[0].first; // Get the first neighbor temporary
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

        if(isBorder) // If the center is a border point
        {
            std::cout << "Center of circumcircle at (" << center.first << ", " << center.second << ") is a border point.\n";
        }
    }

    // TO DO:
    // Add the border points to a list or vector for further processing
    
}

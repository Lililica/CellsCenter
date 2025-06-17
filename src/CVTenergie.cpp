#include <cmath>
#include "LlyodCentralisation.hpp"

float Graphe::calcul_CVT_energie(const int& idxPoint)
{
    Point              point         = pointList[idxPoint];
    float              energie       = 0.f;
    std::vector<Point> cellulePoints = nearCellulePoints[idxPoint];

    // Determine all triangles in a celulle

    const int                         nbrTriangles = static_cast<int>(cellulePoints.size()); // Assuming a triangle is formed by 3 points, we have n-2 triangles in a polygon with n points
    std::vector<std::array<Point, 2>> segmentsTriangles(nbrTriangles);

    for (int i = 0; i < nbrTriangles; ++i)
    {
        Point pivot          = cellulePoints[i];
        Point VectorDirector = {point.first - pivot.first, point.second - pivot.second}; // Vector from the pivot to the point

        float minAngle = 10.f; // Initialize the minimum angle to 0

        for (Point& p : cellulePoints)
        {
            if (pivot == p)
                continue; // Skip the pivot point

            if (VectorDirector.first * p.first - VectorDirector.second * p.second < 0)
            {
                Point pVector = {p.first - point.first, p.second - point.second}; // Vector from the pivot to the point p

                float angle = std::acos((VectorDirector.first * pVector.first + VectorDirector.second * pVector.second) / (std::sqrt(VectorDirector.first * VectorDirector.first + VectorDirector.second * VectorDirector.second) * std::sqrt(pVector.first * pVector.first + pVector.second * pVector.second)));
                if (angle < minAngle)
                {
                    minAngle             = angle;
                    segmentsTriangles[i] = {pivot, p}; // Store the segment formed by the pivot and the point
                }
            }
        }

        if (minAngle == 10.f)
        {
            for (Point& p : cellulePoints)
            {
                if (pivot == p)
                    continue; // Skip the pivot point

                if (VectorDirector.first * p.first - VectorDirector.second * p.second > 0)
                {
                    Point pVector = {p.first - point.first, p.second - point.second}; // Vector from the pivot to the point p

                    float angle = std::acos((VectorDirector.first * pVector.first + VectorDirector.second * pVector.second) / (std::sqrt(VectorDirector.first * VectorDirector.first + VectorDirector.second * VectorDirector.second) * std::sqrt(pVector.first * pVector.first + pVector.second * pVector.second)));
                    if (angle < minAngle)
                    {
                        minAngle             = angle;
                        segmentsTriangles[i] = {pivot, p}; // Store the segment formed by the pivot and the point
                    }
                }
            }
        }

        if (minAngle == 10.f)
        {
            std::cerr << "No segment found for point: (" << point.first << ", " << point.second << ") in cell with pivot: (" << pivot.first << ", " << pivot.second << ")" << '\n';
            continue; // No segment found, skip to the next iteration
        }
    }

    // Calculate the energy for each segment

    float totalEnergy = 0.f;

    for (const auto& segment : segmentsTriangles)
    {
        Point p1 = segment[0];
        Point p2 = segment[1];

        // For u and v vector director of the triangle, we have to calculate : Area(u, v) * (u.u + v.v - u.v) / 6

        Point u     = {p1.first - point.first, p1.second - point.second}; // Vector from the point to p1
        Point v     = {p2.first - point.first, p2.second - point.second};
        float uNorm = std::sqrt(u.first * u.first + u.second * u.second);
        float vNorm = std::sqrt(v.first * v.first + v.second * v.second);
        float area  = 0.5f * uNorm * vNorm; // Area of the triangle formed by the point, p1, and p2

        totalEnergy += area * (uNorm * uNorm + vNorm * vNorm - (u.first * v.first + u.second * v.second)) / 6.f; // Add the energy of the triangle to the total energy
    }

    return totalEnergy; // Return the total energy calculated for the point
}

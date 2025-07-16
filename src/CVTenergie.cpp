#include <cmath>
#include "LlyodCentralisation.hpp"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

void Graphe::triesNearCellulePoints() // Sort the near cell points based on their distance to the current energy point
{
    if (nearCellulePoints.empty())
    {
        std::cerr << "Error: nearCellulePoints is empty. Cannot sort points." << '\n';
        return; // Exit if there are no near cell points
    }

    nearCellulePointsTriees.clear();                  // Clear the sorted list before populating it
    nearCellulePointsTriees.resize(pointList.size()); // Resize to match the number of points

    for (int idxPoint = 0; idxPoint < pointList.size(); ++idxPoint)
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
            Point VectorDirector = {pivot.first - point.first, pivot.second - point.second}; // Vector from the pivot to the point

            float minAngle = 10.f; // Initialize the minimum angle to 0

            for (Point& p : cellulePoints)
            {
                if (pivot == p)
                    continue; // Skip the pivot point

                Point pVector = {p.first - point.first, p.second - point.second}; // Vector from the pivot to the point p

                float angle      = std::acos((VectorDirector.first * pVector.first + VectorDirector.second * pVector.second) / (std::sqrt(VectorDirector.first * VectorDirector.first + VectorDirector.second * VectorDirector.second) * std::sqrt(pVector.first * pVector.first + pVector.second * pVector.second)));
                bool  isNegative = (VectorDirector.first * pVector.second - VectorDirector.second * pVector.first) < 0; // Check if the angle is negative
                if (isNegative)
                    angle = 2.f * M_PI - angle; // Adjust the angle to be in the range [0, 2π]
                if (angle < minAngle)
                {
                    minAngle             = angle;
                    segmentsTriangles[i] = {pivot, p}; // Store the segment formed by the pivot and the point
                }
            }
        }

        nearCellulePointsTriees[idxPoint] = segmentsTriangles; // Store the sorted segments for the current point
    }
}

float Graphe::calcul_CVT_energie(const int& idxPoint)
{
    Point              point         = pointList[idxPoint];
    float              energie       = 0.f;
    std::vector<Point> cellulePoints = nearCellulePoints[idxPoint];

    // Determine all triangles in a celulle

    const int                         nbrTriangles      = static_cast<int>(cellulePoints.size()); // Assuming a triangle is formed by 3 points, we have n-2 triangles in a polygon with n points
    std::vector<std::array<Point, 2>> segmentsTriangles = nearCellulePointsTriees[idxPoint];      // Get the sorted segments for the current point

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

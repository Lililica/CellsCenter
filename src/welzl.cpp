#include <iostream>
#include <random>
#include "utils.hpp"

// Define a random number generator
static std::mt19937 gen(std::random_device{}());

std::pair<Point, float> getCircle(std::vector<Point>& points)
{
    if (points.size() == 0)
        return {{0.f, 0.f}, 0.f}; // No points, return center at origin with radius 0

    if (points.size() == 1)
    {
        // std::cout << "Calculating circle from 1 point: (" << points[0].first << ", " << points[0].second << ")\n";
        return {points[0], 0.f}; // One point, return that point as center with radius 0
    }

    if (points.size() == 2)
    {
        // std::cout << "Calculating circle from 2 points: (" << points[0].first << ", " << points[0].second << "), ("
        //           << points[1].first << ", " << points[1].second << ")\n";
        Point p1 = points[0];
        Point p2 = points[1];
        Point center((p1.first + p2.first) / 2., (p1.second + p2.second) / 2.);
        float radius = distance(p1, center);
        return {center, radius};
    }
    if (points.size() == 3)
    {
        Point a = points[0];
        Point b = points[1];
        Point c = points[2];

        // Calcul du dénominateur
        double d = 2 * (a.first * (b.second - c.second) + b.first * (c.second - a.second) + c.first * (a.second - b.second));

        if (std::abs(d) < 1e-6)
        {
            // Les points sont probablement colinéaires → pas de cercle circonscrit
            return {{0.f, 0.f}, 0.f};
        }

        // Helper: norme² = x² + y²
        auto norm = [](const Point& p) {
            return p.first * p.first + p.second * p.second;
        };

        double x = (norm(a) * (b.second - c.second) + norm(b) * (c.second - a.second) + norm(c) * (a.second - b.second)) / d;

        double y = (norm(a) * (c.first - b.first) + norm(b) * (a.first - c.first) + norm(c) * (b.first - a.first)) / d;

        Point center(static_cast<float>(x), static_cast<float>(y));
        float radius = distance(center, a);

        return {center, radius};
    }
    std::cerr << "Error: Invalid number of points for circle calculation." << '\n';
    return {{0.f, 0.f}, 0.f}; // Return a default value in case of error
}

std::pair<Point, float> welzl(std::vector<Point>& points, std::vector<Point>& boundary)
{
    std::pair<Point, float> circle = {{0.f, 0.f}, 0.f}; // Initialize the circle with center at origin and radius 0

    if (points.empty() || boundary.size() == 3)
    {
        // Base case: if there are 3 or fewer points, return the boundary
        return getCircle(boundary);
    }
    else
    {
        Point pt = points.back(); // Take the last point from the shuffled list
        points.pop_back();        // Remove the point from the list
        circle = welzl(points, boundary);
        if (distance(circle.first, pt) > circle.second + 1e-6)
        {
            // If the point is inside the circle, return the current circle
            boundary.push_back(pt);
            circle = welzl(points, boundary);
            boundary.pop_back(); // Remove the point from the boundary after recursion
        }
        points.push_back(pt); // Add the point back to the list after recursion
    }

    return circle;
}

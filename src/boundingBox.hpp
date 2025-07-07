#pragma once

#include <Eigen/Dense>
#include <iostream>
#include <limits>
#include <vector>

using Point2D = Eigen::Vector2f;
using namespace std;

// Compute centroid of a set of 2D points
Point2D computeCentroid(const std::vector<Point2D>& points)
{
    Point2D c(0.f, 0.f);
    for (const auto& p : points)
        c += p;
    c /= static_cast<float>(points.size());
    return c;
}

// Oriented Minimum Bounding Box via SVD
std::vector<Point2D> computeOrientedBoundingBox(const std::vector<Point2D>& points)
{
    if (points.size() < 2)
        return points;

    // 1. Compute centroid
    Point2D centroid = computeCentroid(points);

    // 2. Center the point cloud
    Eigen::MatrixXf centered(points.size(), 2);
    for (int i = 0; i < points.size(); ++i)
        centered.row(i) = points[i] - centroid;

    // 3. Compute covariance matrix
    Eigen::Matrix2f cov = (centered.transpose() * centered) / points.size();

    // 4. SVD (or PCA)
    Eigen::JacobiSVD<Eigen::Matrix2f> svd(cov, Eigen::ComputeFullU);
    Eigen::Matrix2f                   rotation = svd.matrixU(); // Columns = principal axes

    // 5. Rotate all points to align with principal axes
    std::vector<Point2D> rotatedPoints;
    for (const auto& p : points)
        rotatedPoints.push_back(rotation.transpose() * (p - centroid));

    // 6. Find min/max bounds in rotated space
    Point2D minPt(std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
    Point2D maxPt(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());

    for (const auto& p : rotatedPoints)
    {
        minPt = minPt.cwiseMin(p);
        maxPt = maxPt.cwiseMax(p);
    }

    // 7. Define corners in rotated space (axis-aligned box)
    std::vector<Point2D> boxCorners = {
        {minPt.x(), minPt.y()},
        {maxPt.x(), minPt.y()},
        {maxPt.x(), maxPt.y()},
        {minPt.x(), maxPt.y()}
    };

    // 8. Rotate box corners back to original space
    for (auto& corner : boxCorners)
        corner = rotation * corner + centroid;

    return boxCorners;
}

// Debug print
void printBox(const std::vector<Point2D>& box)
{
    cout << "Oriented Bounding Box Corners:\n";
    for (const auto& p : box)
        cout << "(" << p.x() << ", " << p.y() << ")\n";
}

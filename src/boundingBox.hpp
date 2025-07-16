#pragma once

#include <Eigen/Dense>
#include <iostream>
#include <limits>
#include <vector>

using Point2D = Eigen::Vector2f;
using namespace std;

// Compute centroid of a set of 2D points
Point2D computeMean(const std::vector<Point2D>& points)
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
    Point2D centroid = computeMean(points);

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

Point2D computeIntersection(Point2D v1, Point2D v2, Point2D pA, Point2D pB)
{
    // v1 = BC, v2 = GH, pA = B, pB = C
    float x1 = v1.x();
    float y1 = v1.y();
    float a  = pA.x();
    float b  = pA.y();

    float x2 = v2.x();
    float y2 = v2.y();
    float c  = pB.x();
    float d  = pB.y();

    float denom = x1 * y2 - y1 * x2;
    if (denom == 0)
    {
        std::cerr << "Lines are parallel, no intersection found.\n";
        return Point2D(0.f, 0.f); // No intersection
    }
    float u = (x2 * (b - d) - y2 * (a - c)) / denom; // Calculate the intersection parameter

    return pB + u * v2;
}

// Oriented Minimum Bounding Box via SVD
std::array<Point2D, 8> computeOrientedOctagon(const std::vector<Point2D>& points)
{
    if (points.size() < 2)
        return {};

    // 1. Compute centroid
    Point2D centroid = computeMean(points);

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
    rotatedPoints.reserve(points.size());
    for (const auto& p : points)
        rotatedPoints.emplace_back(rotation.transpose() * (p - centroid));

    std::vector<Point2D> rotatedPoints45deg;
    rotatedPoints45deg.reserve(rotatedPoints.size());
    for (const auto& p : rotatedPoints)
    {
        // Rotate 45 degrees to align with octagon axes
        float x = (p.x()) * std::cos(M_PI / 4) - (p.y()) * std::sin(M_PI / 4);
        float y = (p.x()) * std::sin(M_PI / 4) + (p.y()) * std::cos(M_PI / 4);
        rotatedPoints45deg.emplace_back(x, y);
    }

    // 6. Find min/max bounds in rotated space
    Point2D minPt(std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
    Point2D maxPt(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());

    for (const auto& p : rotatedPoints)
    {
        minPt = minPt.cwiseMin(p);
        maxPt = maxPt.cwiseMax(p);
    }

    Point2D minPt45deg(std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
    Point2D maxPt45deg(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());

    for (const auto& p : rotatedPoints45deg)
    {
        minPt45deg = minPt45deg.cwiseMin(p);
        maxPt45deg = maxPt45deg.cwiseMax(p);
    }

    // 7. Define corners in rotated space (axis-aligned box)
    std::vector<Point2D> boxCorners = {
        {minPt.x(), minPt.y()}, // A        B - - - C    y
        {maxPt.x(), minPt.y()}, // B        |       |    ^
        {maxPt.x(), maxPt.y()}, // C        |       |    |
        {minPt.x(), maxPt.y()}  // D        A - - - D    --> x
    };

    std::vector<Point2D> boxCorners45deg = {
        {minPt45deg.x(), minPt45deg.y()}, // E    (between A & D)  // F - - - G    y
        {maxPt45deg.x(), minPt45deg.y()}, // F    (between A & B)  // |       |    ^
        {maxPt45deg.x(), maxPt45deg.y()}, // G    (between B & C)  // |       |    |
        {minPt45deg.x(), maxPt45deg.y()}  // H    (between D & C)  // E - - - H    --> x
    };

    // Invert the 45deg rotation
    for (auto& corner : boxCorners45deg)
    {
        float x = (corner.x()) * std::cos(-M_PI / 4) - (corner.y()) * std::sin(-M_PI / 4);
        float y = (corner.x()) * std::sin(-M_PI / 4) + (corner.y()) * std::cos(-M_PI / 4);
        corner  = Point2D(x, y);
    }

    // 8. Determine the corners of the octagon

    std::array<Point2D, 8> octagonCorners;

    if (1)
    { // Line BC will intersect with line GH and GF

        Point2D BC = boxCorners[2] - boxCorners[1];           // Vector BC
        Point2D GH = boxCorners45deg[3] - boxCorners45deg[2]; // Vector GH
        Point2D GF = boxCorners45deg[1] - boxCorners45deg[2]; // Vector GF

        Point2D intersectionBC_GH = computeIntersection(BC, GH, boxCorners[1], boxCorners[2]);      // Intersection point of BC and GH
        Point2D intersectionBC_GF = computeIntersection(BC, GF, boxCorners[1], boxCorners45deg[2]); // Intersection point of BC and GF

        octagonCorners[0] = intersectionBC_GF; // A'
        octagonCorners[1] = intersectionBC_GH; // B'

        // Line CD will intersect with line HG and HE

        Point2D CD = boxCorners[3] - boxCorners[2];           // Vector CD
        Point2D HG = -GH;                                     // Vector HG (reverse of GH)
        Point2D HE = boxCorners45deg[0] - boxCorners45deg[3]; // Vector HE

        Point2D intersectionCD_HG = computeIntersection(CD, HG, boxCorners[2], boxCorners[3]);      // Intersection point of CD and HG
        Point2D intersectionCD_HE = computeIntersection(CD, HE, boxCorners[2], boxCorners45deg[3]); // Intersection point of CD and HE

        octagonCorners[2] = intersectionCD_HG; // C'
        octagonCorners[3] = intersectionCD_HE; // D'

        // Line DA will intersect with line EF and EH

        Point2D DA = boxCorners[0] - boxCorners[3];           // Vector DA
        Point2D EF = boxCorners45deg[1] - boxCorners45deg[0]; // Vector EF
        Point2D EH = -HE;                                     // Vector EH (reverse of HE)

        Point2D intersectionDA_EF = computeIntersection(DA, EF, boxCorners[3], boxCorners45deg[0]); // Intersection point of DA and EF
        Point2D intersectionDA_EH = computeIntersection(DA, EH, boxCorners[3], boxCorners45deg[3]); // Intersection point of DA and EH

        octagonCorners[4] = intersectionDA_EH; // E'
        octagonCorners[5] = intersectionDA_EF; // F'

        // Line AB will intersect with line FG and FE

        Point2D AB = boxCorners[1] - boxCorners[0]; // Vector AB
        Point2D FG = -GF;                           // Vector FG (reverse of GF)
        Point2D FE = -EF;                           // Vector FE

        Point2D intersectionAB_FG = computeIntersection(AB, FG, boxCorners[0], boxCorners[1]);      // Intersection point of AB and FG
        Point2D intersectionAB_FE = computeIntersection(AB, FE, boxCorners[0], boxCorners45deg[1]); // Intersection point of AB and FE

        octagonCorners[6] = intersectionAB_FG; // G'
        octagonCorners[7] = intersectionAB_FE; // H'
    }

    else
    { // DEBUG

        octagonCorners[0] = boxCorners[0];
        octagonCorners[1] = boxCorners[1];
        octagonCorners[2] = boxCorners[2];
        octagonCorners[3] = boxCorners[3];
        octagonCorners[4] = boxCorners45deg[0];
        octagonCorners[5] = boxCorners45deg[1];
        octagonCorners[6] = boxCorners45deg[2];
        octagonCorners[7] = boxCorners45deg[3];
    }

    // 9. Rotate box corners back to original space
    for (auto& corner : octagonCorners)
        corner = rotation * corner + centroid;

    return octagonCorners;
}
#include "stdafx.h"

// Find the are of intersection between 2 circles, with a precision of 1x10e-4

#include <iostream>
#include <cinttypes>
#include <cmath>


/// <summary>
/// Defines a circle in coordinates XY.
/// </summary>
struct Circle
{
    int x; // centre coordinate X
    int y; // centre coordinate Y
    uint16_t radius;
};


/// <summary>
/// Calculates the numeric solution.
/// </summary>
/// <param name="circle1">The definition of circle 1.</param>
/// <param name="circle2">The definition of circle 2.</param>
/// <returns>The calculated area of intersecion when positive, and negative
/// when there was no convergence within specified precision.</returns>
static double calcNumericSolution(const Circle &circle1, const Circle &circle2)
{
    int circlesRadiDiff = abs(circle1.radius - circle2.radius);
    int touchDistance = circle1.radius + circle2.radius;

    long long distX = circle1.x - circle2.x;
    long long distY = circle1.y - circle2.y;

    long long squareDistBetwCircles = distX * distX + distY * distY;

    // No intersection or only touching?
    if (squareDistBetwCircles >= touchDistance * touchDistance)
        return 0.0;

    auto &smallerCircle = (circle1.radius <= circle2.radius) ? circle1 : circle2;

    // Complete overlap?
    if (squareDistBetwCircles <= circlesRadiDiff * circlesRadiDiff)
        return 3.14159265358979323846 * smallerCircle.radius * smallerCircle.radius;

    auto &biggerCircle = (circle1.radius >= circle2.radius) ? circle1 : circle2;

    const long long sqRadiusBigger = biggerCircle.radius * biggerCircle.radius;
    const long long sqRadiusSmaller = smallerCircle.radius * smallerCircle.radius;

    const int yRangeEnd = smallerCircle.y + smallerCircle.radius;
    const int xRangeEnd = smallerCircle.x + smallerCircle.radius;

    double area = 0.0, previousResult;

    // calculate area in each iteration, until result converges
    for (double delta = smallerCircle.radius / 64.0; delta != 0.0; delta /= 2.0)
    {
        double sqDelta = delta * delta;

        previousResult = area;
        area = 0.0;
        
        // Scan circle area in axis X
        for (double x = smallerCircle.x - smallerCircle.radius; x <= xRangeEnd; x += delta)
        {
            // Scan circle area in axis Y
            for (double y = smallerCircle.y - smallerCircle.radius; y <= yRangeEnd; y += delta)
            {
                distX = x - smallerCircle.x;
                distY = y - smallerCircle.y;

                long long sqDistToCentreSmall = distX * distX + distY * distY;

                distX = x - biggerCircle.x;
                distY = y - biggerCircle.y;

                long long sqDistToCentreBig = distX * distX + distY * distY;

                // Test for existence inside/outside intersection:
                if (sqDistToCentreSmall >= sqRadiusSmaller || sqDistToCentreBig >= sqRadiusBigger)
                    continue;
                else
                    area += sqDelta; // increment area
            }
        }

        // convergence:
        if (abs(area - previousResult) <= 1e-4)
            return area;
    }

    return -1.0; // did not converge with specified precision!
}


/// <summary>
/// Calculates the analytic solution.
/// </summary>
/// <param name="circle1">The definition of circle 1.</param>
/// <param name="circle2">The definition of circle 2.</param>
/// <returns>The calculated area of intersecion when positive, and negative
/// when there was no convergence within specified precision.</returns>
static double calcAnalyticSolution(const Circle &circle1, const Circle &circle2)
{
    const double sqRadius1 = circle1.radius * circle1.radius;
    const double sqRadius2 = circle2.radius * circle2.radius;

    long long distX = circle1.x - circle2.x;
    long long distY = circle1.y - circle2.y;

    // distance between circles
    const double d = sqrt(distX * distX + distY * distY);

    // a common constant to ease calculation
    const double k = circle1.radius + circle2.radius - d;

    // distance from centre of circle 1 to the line defined by the points where the circles intersect
    const double b = k + (sqRadius1 - sqRadius2) / (2 * k);

    // width of line bisecting the intersection
    const double a = sqrt(sqRadius1 - b * b);

    // distance from centre of circle 2 to the line defined by the points where the circles intersect
    const double c = k - b;

    double area(0.0), previousResult;

    // amount of slices to divide the intersection area for integration
    int sliceCount(64);

    double dx = 2 * a / sliceCount;

    // calculate area inside this loop until convergence:
    while (dx != 0.0)
    {
        previousResult = area;
        area = 0.0;

        double x = -a + dx / 2;

        // do numeric integration of y:
        for (int idx = 0; idx < sliceCount; ++idx)
        {
            double squareX = x * x;
            double y = sqrt(sqRadius1 - squareX) + sqrt(sqRadius2 - squareX) - (b + c);
            area += y * dx;
            x += dx;
        }

        // reached convergence?
        if (abs(area - previousResult) > 1e-4)
        {
            // no, try again with more slices
            sliceCount *= 2;
            dx = 2 * a / sliceCount;
        }
        else
        {
            std::cout << "Analytic solution converged Riemann sum of " << sliceCount
                      << " slices: area is " << area << std::endl;

            return area; // yes
        }
    }

    return -1.0; // did not converge with specified precision!
}
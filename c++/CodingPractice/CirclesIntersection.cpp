#include "stdafx.h"

// Find the are of intersection between 2 circles, with a precision of 1x10e-4

#include <iostream>
#include <cinttypes>
#include <cmath>

#define PRECISION 1e-4

/// <summary>
/// Defines a circle in coordinates XY.
/// </summary>
struct Circle
{
    int x; // centre coordinate X
    int y; // centre coordinate Y
    uint16_t radius;

    void dump() const
    {
        std::cout << "circle (x = " << x
                  << ", y = " << y
                  << ") radius = " << radius << std::endl;
    }
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
    {
        std::cout << "do not intersect!" << std::endl;
        return 0.0;
    }

    auto &smallerCircle = (circle1.radius <= circle2.radius) ? circle1 : circle2;

    // Complete overlap?
    if (squareDistBetwCircles <= circlesRadiDiff * circlesRadiDiff)
    {
        std::cout << "complete overlap" << std::endl;
        return 3.14159265358979323846 * smallerCircle.radius * smallerCircle.radius;
    }

    auto &biggerCircle = (circle1.radius >= circle2.radius) ? circle1 : circle2;

    const long long sqRadiusBigger = biggerCircle.radius * biggerCircle.radius;
    const long long sqRadiusSmaller = smallerCircle.radius * smallerCircle.radius;

    double area(0.0), previousResult;

    int divCount(128);

    double delta = 2 * smallerCircle.radius / divCount;

    do // calculate area in each iteration, until result converges
    {
        double sqDelta = delta * delta;

        previousResult = area;
        area = 0.0;

        double x = smallerCircle.x - smallerCircle.radius + delta / 2;

        // Scan circle area in axis X
        for (int i = 0; i < divCount; ++i)
        {
            double y = smallerCircle.y - smallerCircle.radius + delta / 2;

            // Scan circle area in axis Y
            for (int j = 0; j < divCount; ++j)
            {
                distX = x - smallerCircle.x;
                distY = y - smallerCircle.y;

                long long sqDistToCentreSmall = distX * distX + distY * distY;

                distX = x - biggerCircle.x;
                distY = y - biggerCircle.y;

                long long sqDistToCentreBig = distX * distX + distY * distY;

                // in intersection?
                if (sqDistToCentreSmall >= sqRadiusSmaller || sqDistToCentreBig >= sqRadiusBigger)
                    area += sqDelta; // increment area

                y += delta;
            }

            x += delta;
        }

        // convergence:
        if (abs(area - previousResult) < PRECISION)
        {
            std::cout << "converged with axis divided into " << divCount << " pieces" << std::endl;
            return area;
        }

        divCount *= 2;
        delta /= 2;

    } while (delta != 0.0);

    std::cout << "did not converge!" << std::endl;

    return -1.0; // did not converge with specified precision!
}


/// <summary>
/// Calculates the analytic solution.
/// </summary>
/// <param name="circle1">The definition of circle 1.</param>
/// <param name="circle2">The definition of circle 2.</param>
/// <returns>The calculated area of intersecion when positive,
/// and negative when there was no convergence within specified
/// precision or could not be calculated..</returns>
static double calcAnalyticSolution(const Circle &circle1, const Circle &circle2)
{
    int circlesRadiDiff = abs(circle1.radius - circle2.radius);
    int touchDistance = circle1.radius + circle2.radius;

    long long distX = circle1.x - circle2.x;
    long long distY = circle1.y - circle2.y;

    long long squareDistBetwCircles = distX * distX + distY * distY;

    // No intersection or only touching?
    if (squareDistBetwCircles >= touchDistance * touchDistance)
    {
        std::cout << "do not intersect!" << std::endl;
        return 0.0;
    }

    auto &smallerCircle = (circle1.radius <= circle2.radius) ? circle1 : circle2;

    // Complete overlap?
    if (squareDistBetwCircles <= circlesRadiDiff * circlesRadiDiff)
    {
        std::cout << "complete overlap" << std::endl;
        return 3.14159265358979323846 * smallerCircle.radius * smallerCircle.radius;
    }

    const double sqRadius1 = circle1.radius * circle1.radius;
    const double sqRadius2 = circle2.radius * circle2.radius;

    // distance between circles
    const double d = sqrt(squareDistBetwCircles);

    // how deep the circles overlap, measured in the line that links both centres
    const double k = circle1.radius + circle2.radius - d;

    // distance from centre of circle 1 to the line defined by the points where the circles intersect
    const double b = k + (sqRadius1 - sqRadius2) / (2 * k);

    // distance from centre of circle 2 to the line defined by the points where the circles intersect
    const double c = k - b;

    // does the overlap reaches the centre of any of the circles?
    if (abs(b) * smallerCircle.radius > PRECISION
        || abs(c) * smallerCircle.radius > PRECISION)
    {
        // analytic approach does not apply:
        std::cout << "not applicable!" << std::endl;
        return -1.0;
    }

    // width of line bisecting the intersection
    const double a = sqrt(sqRadius1 - b * b);

    double area(0.0), previousResult;

    // amount of slices to divide the intersection area for integration
    int sliceCount(64);

    double dx = 2 * a / sliceCount;

    do // calculate area inside this loop until convergence:
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
        if (abs(area - previousResult) > PRECISION)
        {
            sliceCount *= 2; // no, try again with more slices
            dx = 2 * a / sliceCount;
        }
        else
        {
            std::cout << "Analytic solution converged with Riemann sum of " << sliceCount << " slices" << std::endl;
            return area; // yes
        }

    } while (dx != 0.0);

    std::cout << "did not converge!" << std::endl;

    return -1.0; // did not converge with specified precision!
}
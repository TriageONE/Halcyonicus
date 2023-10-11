//
// Created by Triage on 10/3/2023.
//

#ifndef HALCYONICUS_WORLEY_H
#define HALCYONICUS_WORLEY_H

#include <iostream>
#include <vector>
#include <cmath>
#include <random>

class worleyNoise{
    std::mt19937 generator;
    double euclideanDistance(double x1, double y1, double x2, double y2) {
        double dx = x2 - x1;
        double dy = y2 - y1;
        return sqrt(dx * dx + dy * dy);
    }
public:

    explicit worleyNoise(unsigned int seed) {
        generator = std::mt19937(seed);
    };
    // Function to generate Worley noise at a specific point (x, y) with a seed
    double noise2D(double x, double y) {
        std::uniform_real_distribution<double> distribution(0.0, 1.0);

        const int numPoints = 10;  // Adjust the number of points as needed
        double minDistance = 1.0;

        for (int i = 0; i < numPoints; ++i) {
            double px = distribution(generator);  // Random point x-coordinate
            double py = distribution(generator);  // Random point y-coordinate

            double distance = euclideanDistance(x, y, px, py);

            if (distance < minDistance) {
                minDistance = distance;
            }
        }

        // You can apply fractal noise by recursively calling worleyNoise for more octaves

        return minDistance;
    }

};



#endif //HALCYONICUS_WORLEY_H

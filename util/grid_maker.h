#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <utility>
#include "config.h"

inline std::vector<sf::Vector2f> make_point_grid(const Config& c) {
    float l_between = 4.f * static_cast<float>(c.l) + 1.f;
    std::vector<sf::Vector2f> grid_points;
    for (int y_iter = -c.grid_radius; y_iter <= c.grid_radius; y_iter++) {
        for (int x_iter = -c.grid_radius; x_iter <= c.grid_radius; x_iter++) {
            sf::Vector2f grid_point = {l_between * x_iter, l_between * y_iter};
            grid_points.push_back(grid_point);
        }
    }
    return grid_points;
}

inline std::vector<std::pair<double, double>> make_angle_grid(const Config& c) {
    float angle_between = static_cast<float>(c.grid_angle_interval);
    std::vector<std::pair<double, double>> grid_angles;
    for (int y_iter = -c.grid_radius; y_iter <= c.grid_radius; y_iter++) {
        for (int x_iter = -c.grid_radius; x_iter <= c.grid_radius; x_iter++) {
            double theta1 = angle_between * x_iter * (M_PI / 180.0);
            double theta2 = angle_between * y_iter * (M_PI / 180.0);
            std::pair<double, double> grid_angle =  {theta1, theta2};
            grid_angles.push_back(grid_angle);
        }
    }
    return grid_angles;
}
#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <utility>
#include <cmath>
#include "config.h"

struct Axes {
    sf::VertexArray lines{sf::PrimitiveType::Lines, 4};
    std::vector<sf::Text> texts;

    Axes() {}

    Axes(const Config& c, const sf::Font& font) {
        float l_between = 4.f * static_cast<float>(c.l) + 1.f;
        float angle_between = static_cast<float>(c.grid_angle_interval);
        float dot_size = static_cast<float>(c.dot_size);

        //lines
        lines[0].position = {-(c.grid_radius + 1) * l_between, 0.f};
        lines[1].position = {(c.grid_radius + 1) * l_between, 0.f};

        lines[2].position = {0.f, -(c.grid_radius + 1) * l_between};
        lines[3].position = {0.f, (c.grid_radius + 1) * l_between};

        for (int i = 0; i < 4; i++) {
            lines[i].color = sf::Color(100, 100, 100);
        }

        //dots and texts
        unsigned int char_size = static_cast<unsigned int>(c.l * 0.2);
        //x
        for (int x_iter = -c.grid_radius; x_iter <= c.grid_radius; x_iter++) {
            sf::Vector2f x_axis_postion = {l_between * x_iter, 0.f};
            double theta1 = angle_between * x_iter;

            sf::Text text(font);

            double rounded = std::round(theta1 * 1000) / 1000;
            text.setString(std::to_string(rounded));

            sf::FloatRect bounds = text.getLocalBounds();
            text.setPosition(x_axis_postion + sf::Vector2f{0.f, -(bounds.size.y)});
            text.setCharacterSize(char_size);

            texts.push_back(text);
        }
        //y
        for (int y_iter = -c.grid_radius; y_iter <= c.grid_radius; y_iter++) {
            if (y_iter != 0) {
                sf::Vector2f y_axis_postion = {0.f, l_between * y_iter};
                double theta1 = angle_between * y_iter;

                sf::Text text(font);

                double rounded = std::round(theta1 * 1000) / 1000;
                text.setString(std::to_string(rounded));

                sf::FloatRect bounds = text.getLocalBounds();
                text.setPosition(y_axis_postion + sf::Vector2f{0.f, -(bounds.size.y)});
                text.setCharacterSize(char_size);

                texts.push_back(text);
            }
        }
    }

    void draw_axes(sf::RenderWindow& window) {
        window.draw(lines);
        for (const sf::Text& text : texts) {
            window.draw(text);
        }
    }
};

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
#pragma once

#include <SFML/Graphics.hpp>
#include <vector>

#include "util/config.h"

typedef std::vector<double> state_type;

struct Pendulum {
    sf::Vector2f origin;
    sf::Vector2f p1;
    sf::Vector2f p2;

    state_type state;

    Pendulum(const Config& c, double theta1, double theta2, sf::Vector2f origin_) {
        float x1 = static_cast<float>(c.l * std::sin(theta1));
        float y1 = static_cast<float>(c.l * std::cos(theta1));
        float x2 = static_cast<float>(c.l * std::sin(theta2));
        float y2 = static_cast<float>(c.l * std::cos(theta2));

        origin = origin_;
        p1 = origin_ + sf::Vector2f{x1, y1};
        p2 = origin_ + sf::Vector2f{x1 + x2, y1 + y2};

        state = {
            theta1,
            theta2,
            0.0,
            0.0
        };
    }
    
    void draw_pendulum(const Config& c, sf::RenderWindow& window) {
        //dots
        float dot_size = static_cast<float>(c.dot_size);
        sf::Vector2f dot_offset{dot_size, dot_size};

        sf::CircleShape dot_origin(dot_size);
        dot_origin.setPosition(origin - dot_offset);
        sf::CircleShape dot_p1(dot_size);
        dot_p1.setPosition(p1 - dot_offset);
        sf::CircleShape dot_p2(dot_size);
        dot_p2.setPosition(p2 - dot_offset);

        //lines
        sf::VertexArray line1(sf::PrimitiveType::Lines, 2);
        line1[0].position = origin;
        line1[1].position = p1;

        sf::VertexArray line2(sf::PrimitiveType::Lines, 2);
        line2[0].position = p1;
        line2[1].position = p2;

        //draw
        window.draw(dot_origin);
        window.draw(dot_p1);
        window.draw(dot_p2);
        window.draw(line1);
        window.draw(line2);
    }
};

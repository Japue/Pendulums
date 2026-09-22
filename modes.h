#pragma once

#include <SFML/Graphics.hpp>
#include "util/config.h"

void single_pendulum(const Config& c, sf::RenderWindow& window);

void grid_pendulums(const Config& c, sf::RenderWindow& window, sf::View& view);
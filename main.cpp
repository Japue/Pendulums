#include <SFML/Graphics.hpp>
#include <toml++/toml.hpp>
#include <boost/numeric/odeint.hpp>

#include <vector>
#include <cmath>
#include <utility>
#include <list>
#include <iostream>

#include "util/config.h"
#include "util/de_solver.h"
#include "modes.h"

int main(){
    //window setup
    sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "Tree Fractal");
    sf::View view = window.getDefaultView();
    view.setCenter({0, 0});
    window.setView(view);

    //config and mode
    const Config c("config.toml");
    std::string mode = c.mode;
    
    if (mode == "single") {
        single_pendulum(c, window);
    } else if (mode == "grid") {
        grid_pendulums();
    } else {
        std::cout << mode << " is not valid mode" << '\n';
    }
}   
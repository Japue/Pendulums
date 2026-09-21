#include "modes.h"

#include <SFML/Graphics.hpp>
#include <boost/numeric/odeint.hpp>
#include "util/config.h"
#include "util/de_solver.h"
#include "util/pendulum.h"

void single_pendulum(const Config& c, sf::RenderWindow& window) {
    //initialize
    sf::Clock clock;
    double time = 0.0;
    int iteration = 0;
    bool on_off = false;

    //object initialisation
    Pendulum single(c, c.angles.first, c.angles.second, {0.0, 0.0});

    std::list<sf::CircleShape> trace;

    //trajectory with an observer
    std::vector<std::pair<double, double>> trajectory;
    auto observer = [&trajectory](const state_type& x, double t) {
        trajectory.push_back({x[0], x[1]});
    };

    //run the solver with the observer
    auto system = [&c](const state_type& x, state_type& dxdt, const double t) {
        double_pendulum_solver(x, dxdt, t, c);
    };
    boost::numeric::odeint::runge_kutta4<state_type> stepper;
    boost::numeric::odeint::integrate_const(stepper, system, single.state, 0.0, c.runtime, c.timestep, observer);

    //gameloop
    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            //event handling
            if (event -> is<sf::Event::Closed>()) {
                window.close();
            }

            if (const auto* keyPressed = event -> getIf<sf::Event::KeyPressed>()) {
                if (keyPressed->code == sf::Keyboard::Key::Escape) {
                    window.close();
                }
            }

            if (const auto* mousePressed = event -> getIf<sf::Event::MouseButtonPressed>()) {
                if (mousePressed->button == sf::Mouse::Button::Left) {
                    on_off = true;
                }
            }
        }

        //check the clock
        float dt = clock.restart().asSeconds();
        if (on_off) {
            time += dt;
            while(time >= c.timestep && iteration < static_cast<int>(trajectory.size())) {
                std::pair<double, double> angle_pair = trajectory.at(iteration);

                single = Pendulum(c, angle_pair.first, angle_pair.second, {0.0, 0.0}); 

                iteration++;
                time -= c.timestep;
            }
        }

        window.clear();
        //drawing
        single.draw_pendulum(c, window);
        //
        window.display();
    }
}

void grid_pendulums(sf::RenderWindow& window) {
    //initialization
    sf::Clock clock;
    double time = 0.0;
    int iteration = 0;
    bool on_off = false;

    //object initialisation
    std::vector<Pendulum> pendulums;

    //gameLoop
    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            //event handling
            if (event -> is<sf::Event::Closed>()) {
                window.close();
            }

            if (const auto* keyPressed = event -> getIf<sf::Event::KeyPressed>()) {
                if (keyPressed->code == sf::Keyboard::Key::Escape) {
                    window.close();
                }
            }

            if (const auto* mousePressed = event -> getIf<sf::Event::MouseButtonPressed>()) {
                if (mousePressed->button == sf::Mouse::Button::Left) {
                    on_off = true;
                }
            }
        }

        //check the clock
        float dt = clock.restart().asSeconds();
        if (on_off) {
            time += dt;
            while(time >= c.timestep && iteration < static_cast<int>(trajectory.size())) {
                std::pair<double, double> angle_pair = trajectory.at(iteration); 
                theta1 = angle_pair.first;
                theta2 = angle_pair.second;

                x1 = static_cast<float>(c.l * std::sin(theta1));
                y1 = static_cast<float>(c.l * std::cos(theta1));
                x2 = static_cast<float>(c.l * std::sin(theta2));
                y2 = static_cast<float>(c.l * std::cos(theta2));

                p1 = {x1, y1};
                p2 = {x1 + x2, y1 + y2};

                iteration++;
                time -= c.timestep;
            }
        }

        window.clear();
        //drawing
        
        //
        window.display();
    }
}
#include <SFML/Graphics.hpp>
#include <toml++/toml.hpp>
#include <boost/numeric/odeint.hpp>

#include <vector>
#include <cmath>
#include <utility>

#include "config.h"
#include "de_solver.h"

int main(){
    //window setup
    sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "Tree Fractal");
    sf::View view = window.getDefaultView();
    view.setCenter({0, 0});
    window.setView(view);

    sf::Clock clock;
    double time = 0.0;

    //calculate all the points
    //config
    const Config c("config.toml");
    state_type x = {
        c.angles.first, 
        c.angles.second, 
        0.0, 
        0.0 
    };
    bool on_off = false;
    int time_iteration = 0;

    //trajectory
    std::vector<std::pair<double, double>> trajectory;
    auto observer = [&trajectory](const state_type& x, double t) {
        trajectory.push_back({x[0], x[1]});
    };

    //run the solver
    auto system = [&c](const state_type& x, state_type& dxdt, const double t) {
        double_pendulum(x, dxdt, t, c);
    };
    boost::numeric::odeint::runge_kutta4<state_type> stepper;
    boost::numeric::odeint::integrate_const(stepper, system, x, 0.0, c.runtime, c.timestep, observer);

    //make the lines and points
    double theta1 = c.angles.first;
    double theta2 = c.angles.second;
    float x1 = static_cast<float>(c.l * std::sin(theta1));
    float y1 = static_cast<float>(c.l * std::cos(theta1));
    float x2 = static_cast<float>(c.l * std::sin(theta2));
    float y2 = static_cast<float>(c.l * std::cos(theta2));

    sf::Vector2f origin{0.f, 0.f};
    sf::Vector2f p1{x1, y1};
    sf::Vector2f p2{x1 + x2, y1 + y2};

    float dot_size = static_cast<float>(c.dot_size);
    sf::Vector2f dot_offset{dot_size, dot_size};

    sf::CircleShape dot_origin(dot_size);
    sf::CircleShape dot_p1(dot_size);
    sf::CircleShape dot_p2(dot_size);

    std::vector<sf::CircleShape> nodes = {
        dot_origin,
        dot_p1,
        dot_p2
    };
    nodes[0].setPosition(origin - dot_offset);
    sf::VertexArray line1(sf::PrimitiveType::Lines, 2);
    line1[0].position = origin;
    sf::VertexArray line2(sf::PrimitiveType::Lines, 2);

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

        float dt = clock.restart().asSeconds();
        if (on_off) {
            time += dt;
            while(time >= c.timestep && time_iteration < static_cast<int>(trajectory.size())) {
                std::pair<double, double> angle_pair = trajectory.at(time_iteration); 
                theta1 = angle_pair.first;
                theta2 = angle_pair.second;

                x1 = static_cast<float>(c.l * std::sin(theta1));
                y1 = static_cast<float>(c.l * std::cos(theta1));
                x2 = static_cast<float>(c.l * std::sin(theta2));
                y2 = static_cast<float>(c.l * std::cos(theta2));

                p1 = {x1, y1};
                p2 = {x1 + x2, y1 + y2};

                time_iteration++;
                time -= c.timestep;
            }
        }

        window.clear();
        //drawing
        nodes[1].setPosition(p1 - dot_offset);
        nodes[2].setPosition(p2 - dot_offset);

        line1[1].position = p1;

        line2[0].position = p1;
        line2[1].position = p2;
        
        for (sf::CircleShape node : nodes) {
            window.draw(node);
        }
        window.draw(line1);
        window.draw(line2);
        //
    
        window.display();
    }
}   
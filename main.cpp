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

    bool is_dragging = false;
    sf::Vector2i last_mouse_pos = sf::Mouse::getPosition(window);

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
    float theta1 = c.angles.first;
    float theta2 = c.angles.second;
    float x1 = static_cast<float>(c.l * std::cos(theta1));
    float y1 = static_cast<float>(c.l * std::sin(theta1));
    float x2 = static_cast<float>(c.l * std::cos(theta2));
    float y2 = static_cast<float>(c.l * std::sin(theta2));

    sf::Vector2f origin{0.f, 0.f};
    sf::Vector2f p1{x1, y1};
    sf::Vector2f p2{x1 + x2, y1 + y2};

    sf::VertexArray nodes(sf::PrimitiveType::Points, 3);
    sf::VertexArray line1(sf::PrimitiveType::Lines, 2);
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
                    is_dragging = true;
                    on_off = true;
                }
            }
            
            if (const auto* mouseReleased = event -> getIf<sf::Event::MouseButtonReleased>()) {
                if (mouseReleased->button == sf::Mouse::Button::Left) {
                    is_dragging = false;
                }
            }

            if (const auto* scrolled = event -> getIf<sf::Event::MouseWheelScrolled>()) {
                if (scrolled->delta > 0) {
                    view.zoom(0.9f);
                } else {
                    view.zoom(1.1f);
                }
            }
        }

        window.clear();

        //dragging
        sf::Vector2i new_mouse_pos = sf::Mouse::getPosition(window);
        if (is_dragging) {
            sf::Vector2i delta_pos = last_mouse_pos - new_mouse_pos;

            sf::Vector2u window_size_u = window.getSize();
            sf::Vector2f window_size(static_cast<float>(window_size_u.x), static_cast<float>(window_size_u.y));
            sf::Vector2f view_size = view.getSize();

            sf::Vector2f scale_and_float(static_cast<float>(delta_pos.x) * (view_size.x / window_size.x), static_cast<float>(delta_pos.y) * (view_size.y / window_size.y));
            view.setCenter(view.getCenter() + scale_and_float);
        }
        last_mouse_pos = new_mouse_pos;
        window.setView(view);

        //drawing
        nodes[0].position = origin;
        nodes[1].position = p1;
        nodes[2].position = p2;

        line1[0].position = origin;
        line1[1].position = p1;

        line2[0].position = p1;
        line2[1].position = p2;
        
        window.draw(nodes);
        window.draw(line1);
        window.draw(line2);

        if (on_off && time_iteration << static_cast<int>(trajectory.size())){
            std::pair<double, double> angle_pair = trajectory.at(time_iteration); 
            theta1 = angle_pair.first;
            theta2 = angle_pair.second;

            x1 = static_cast<float>(c.l * std::cos(theta1));
            y1 = static_cast<float>(c.l * std::sin(theta1));
            x2 = static_cast<float>(c.l * std::cos(theta2));
            y2 = static_cast<float>(c.l * std::sin(theta2));

            p1 = {x1, y1};
            p2 = {x1 + x2, y1 + y2};
            time_iteration++;
        }
        //
    
        window.display();
    }
}
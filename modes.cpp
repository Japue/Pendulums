#include "modes.h"

#include <vector>
#include <utility>

#include <SFML/Graphics.hpp>
#include <boost/numeric/odeint.hpp>

#include "util/config.h"
#include "util/de_solver.h"
#include "util/pendulum.h"
#include "util/grid_maker.h"

void single_pendulum(const Config& c, sf::RenderWindow& window) {
    //initialize
    sf::Clock clock;
    double time = 0.0;
    int iteration = 0;
    bool on_off = false;

    //object initialisation
    Pendulum single(c, c.angles.first, c.angles.second, {0.0, 0.0});

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

                single.update_pendulum(c, angle_pair.first, angle_pair.second); 

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

void grid_pendulums(const Config& c, sf::RenderWindow& window, sf::View& view) {
    //initialization
    bool is_dragging = false;
    sf::Vector2i last_mouse_pos = sf::Mouse::getPosition(window);
    bool on_off = false;

    sf::Clock clock;
    double time = 0.0;
    int iteration = 0;

    Axes axes;
    sf::Font font;
    if (c.grid_axes) {
        if (!font.openFromFile("util/Roboto-Light.ttf")) {
            std::cerr << "Failed to load font\n";
        } else {
            axes = Axes(c, font);
        }
    }

    //grid initialisation
    std::vector<sf::Vector2f> point_grid = make_point_grid(c);
    std::vector<std::pair<double, double>> angle_grid = make_angle_grid(c);
    int grid_size = 2 * c.grid_radius;

    //object initialisation
    std::vector<Pendulum> pendulums;
    for (int y_iter = 0; y_iter <= grid_size; y_iter++) {
        for (int x_iter = 0; x_iter <= grid_size; x_iter++) {
            int index = (grid_size + 1) * y_iter + x_iter;
            sf::Vector2f point = point_grid.at(index);
            std::pair<double, double> angles = angle_grid.at(index);
            pendulums.push_back(Pendulum(c, angles.first, angles.second, point));
        }
    }

    //trajectory with an observer

    //run the solver with the observer
    auto system = [&c](const state_type& x, state_type& dxdt, const double t) {
        double_pendulum_solver(x, dxdt, t, c);
    };
    boost::numeric::odeint::runge_kutta4<state_type> stepper;

    for (Pendulum& pend : pendulums) {
        auto observer = [&pend](const state_type& x, double t) {
            pend.trajectory.push_back({x[0], x[1]});
        };
        boost::numeric::odeint::integrate_const(stepper, system, pend.state, 0.0, c.runtime, c.timestep, observer);
        std::cout << "Calculated pendulum with origin (" << pend.origin.x << ", " << pend.origin.y << ")'\n";
    }

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
                    is_dragging = true;
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

        //check the clock
        float dt = clock.restart().asSeconds();
        if (on_off) {
            time += dt;
            while(time >= c.timestep && iteration < static_cast<int>(pendulums[0].trajectory.size())) {
                for (Pendulum& pend: pendulums) {
                    std::pair<double, double> angle_pair = pend.trajectory.at(iteration); 
                    pend.update_pendulum(c, angle_pair.first, angle_pair.second);
                }
                iteration++;
                time -= c.timestep;
            }
        }
        //drawing
        axes.draw_axes(window);
        for (Pendulum& pend : pendulums) {
            pend.draw_pendulum(c, window);
        }
        //
        window.display();
    }
}

void graph(const Config& c, sf::RenderWindow& window, sf::View& view) {
    bool is_dragging = false;
    sf::Vector2i last_mouse_pos = sf::Mouse::getPosition(window);
    bool on_off = false;

    sf::Clock clock;
    double time = 0.0;
    int iteration = 0;

    //object initialisation
    float l_between = 4.f * static_cast<float>(c.l) + 1.f;
    Pendulum single(c, c.angles.first, c.angles.second, {0.0, 0.0});

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

    //points initialisation
    sf::VertexArray dots(sf::PrimitiveType::Points, trajectory.size());
    sf::Vector2f position_begin = sf::Vector2f{static_cast<float>(c.angles.first) * l_between, static_cast<float>(c.angles.second) * l_between};
    dots[0].position = position_begin;

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
                    is_dragging = true;
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

        //check the clock
        float dt = clock.restart().asSeconds();
        if (on_off) {
            time += dt;
            while(time >= c.timestep && iteration < static_cast<int>(trajectory.size())) {
                std::pair<double, double> angle_pair = trajectory.at(iteration);
                
                sf::Vector2f position = sf::Vector2f{static_cast<float>(angle_pair.first) * l_between, static_cast<float>(angle_pair.second) * l_between};
                dots[iteration].position = position;

                iteration++;
                time -= c.timestep;
            }
        }
        //drawing
        window.draw(dots);
        //
        window.display();
    }
}
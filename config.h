#pragma once

#include <SFML/Graphics.hpp>
#include <toml++/toml.hpp>

#include <utility>
#include <string>
#include <cmath>

struct Config {
    //constants
    double g = 9.81;
    double physics_l = 1.0;

    //parameters
    double runtime = 10.0;
    double timestep = 0.1;
    //this l is for scaling the pendulum
    double l = 100.0;
    double dot_size = 10.0;
    std::pair<double, double> angles = {};

    Config(std::string config_file_name) {
        toml::table tbl = toml::parse_file(config_file_name);

        runtime = tbl["runtime"].value_or(runtime);
        timestep = tbl["timestep"].value_or(timestep);
        l = tbl["l"].value_or(l);
        dot_size = tbl["dot_size"].value_or(dot_size);

        if (auto arr = tbl["angles"].as_array(); arr && arr->size() >= 2) {
            auto first_val = arr->at(0).value<double>();
            auto second_val = arr->at(1).value<double>();

            if (first_val && second_val) {
                angles.first  = *first_val * M_PI / 180.0;
                angles.second = *second_val * M_PI / 180.0;
            }
        }
    }
};
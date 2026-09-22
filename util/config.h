#pragma once

#include <SFML/Graphics.hpp>
#include <toml++/toml.hpp>

#include <utility>
#include <string>
#include <cmath>

struct Config {
    //constants
    double g = -9.81;
    double physics_l = 1.0;

    //mode
    std::string mode = "single";

    //general parameters
    double runtime = 10.0;
    double timestep = 0.1;
    //this l is for scaling the pendulum
    double l = 100.0;
    double dot_size = 10.0;

    //single parameters
    std::pair<double, double> angles = {};

    //grid parameters
    int grid_radius = 1;
    double grid_angle_interval = 1;
    bool grid_axes = false;

    //options
    bool trace = false;
    int trace_length = 25;

    Config(std::string config_file_name) {
        toml::table tbl = toml::parse_file(config_file_name);

        mode = tbl["mode"].value_or(mode);

        runtime = tbl["runtime"].value_or(runtime);
        timestep = tbl["timestep"].value_or(timestep);
        l = tbl["l"].value_or(l);
        dot_size = tbl["dot_size"].value_or(dot_size);

        //single options
        if (auto arr = tbl["angles"].as_array(); arr && arr->size() >= 2) {
            auto first_val = arr->at(0).value<double>();
            auto second_val = arr->at(1).value<double>();

            if (first_val && second_val) {
                angles.first  = *first_val * M_PI / 180.0;
                angles.second = *second_val * M_PI / 180.0;
            }
        }

        //grid options
        grid_radius = tbl["grid_radius"].value_or(grid_radius);
        grid_angle_interval = tbl["grid_angle_interval"].value_or(grid_angle_interval);
        grid_axes = tbl["grid_axes"].value_or(grid_axes);

        //options
        trace = tbl["trace"].value_or(dot_size);
        trace_length = tbl["trace_length"].value_or(trace_length);
    }
};
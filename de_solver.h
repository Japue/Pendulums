#pragma once
#include <cmath>
#include "config.h"

typedef std::vector<double> state_type;

void double_pendulum(const state_type& x, state_type& dxdt, const double t, const Config& c){
    double theta1 = x[0];
    double theta2 = x[1];
    double omega1 = x[2];
    double omega2 = x[3];

    double delta = theta1 - theta2;
    double den = 2 - std::pow(std::cos(delta), 2.0);

    double e = 2 * c.g / c.physics_l * std::cos(theta1) - omega2 * omega2 * std::sin(delta);
    double f = c.g / c.physics_l * cos(theta2) + omega1 * omega1 * std::sin(delta);

    dxdt[0] = omega1;
    dxdt[1] = omega2;
    dxdt[2] = e / den - std::cos(delta) * f / den;
    dxdt[3] = 2 * f / den - std::cos(delta) * e / den;
}
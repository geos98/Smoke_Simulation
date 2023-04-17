#include "particle.hh"

void Particle::update(double delta_t) {
    double a = (((double )rand() / (double) RAND_MAX) - 0.5) * 0.05;
    double b = (((double )rand() / (double) RAND_MAX) - 0.5) * 0.05;
    double c = (((double )rand() / (double) RAND_MAX) - 0.5) * 0.05;
    pos += nanogui::Vector3f(a, b, c);
}
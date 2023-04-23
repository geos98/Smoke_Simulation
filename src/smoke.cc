#include "smoke.hh"

using namespace std;
using namespace nanogui;

void Smoke::update(double delta_t)
{
    auto p_it = particles.begin();
    while (p_it != particles.end()) {
        p_it->update(delta_t);
        if (p_it->lifespan <= 0) {
            particles.erase(p_it++);
        } else {
            p_it++;
        }
    }
}

void Smoke::generateParticles(const Emittor& emittor, int num_particles)
{
    for (int i = 0; i < num_particles; ++i)
    {
        particles.emplace_back(emittor.emit());
    }
}
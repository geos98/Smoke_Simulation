#include "smoke.hh"

using namespace std;
using namespace nanogui;

void Smoke::update(double delta_t)
{
    // TODO: optimize particles filtering
    vector<Particle> particles_copy;
    for (int i = 0; i < particles.size(); ++i)
    {
        particles[i].update(delta_t);
        if (particles[i].lifespan > 0)
        {
            particles_copy.emplace_back(particles[i]);
        }
    }
    particles = particles_copy;
}

void Smoke::generateParticles(Emittor *emittor, int num_particles)
{
    for (int i = 0; i < num_particles; ++i)
    {
        particles.emplace_back(emittor->emit());
    }
}
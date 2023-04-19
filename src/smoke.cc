#include "smoke.hh"

Smoke::Smoke()
{
}

void Smoke::update(double delta_t)
{
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
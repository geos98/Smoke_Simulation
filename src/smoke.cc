#include "smoke.hh"

Smoke::Smoke()
{
    emittor = new Emittor(Vector3f(0, -1, 0), Vector3f(0, 0.5, 0), .5f);
}

void Smoke::update(double delta_t)
{
    // Generate new particles to the system
    for (int i = 0; i < 100; ++i)
    {
        particles.emplace_back(emittor->emit());
    }
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
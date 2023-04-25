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

    build_spatial_map();
    for (auto &pair : particle_map)
    {
        //cout << pair.second.begin()[0]->pos << endl;
        nsp.update(pair.second, delta_t);
        //cout << pair.second.begin()[0]->pos << endl;
    }
}

void Smoke::generateParticles(const Emittor& emittor, int num_particles)
{
    for (int i = 0; i < num_particles; ++i)
    {
        particles.emplace_back(emittor.emit());
    }
}

void Smoke::build_spatial_map()
{
    particle_map.clear();
    for (auto &p : particles)
    {
        uint64_t hash_pos = hash_position(p.pos);
        particle_map[hash_pos].emplace_back(&p);
    }
}

uint64_t Smoke::hash_position(nanogui::Vector3f pos)
{

    double w = (3 * width) / grid_width;
    double h = (3 * height) / grid_height;
    double t = std::max(w, h);

    uint64_t x_box = floor(pos[0] / w);
    uint64_t y_box = floor(pos[1] / h);
    uint64_t z_box = floor(pos[2] / t);

    // Hasing function taken from: http://www.beosil.com/download/CollisionDetectionHashing_VMV03.pdf
    uint64_t p1 = 73856093;
    uint64_t p2 = 19349663;
    uint64_t p3 = 83492791;

    return (x_box * p1) ^ (y_box * p2) ^ (z_box * p3);
}
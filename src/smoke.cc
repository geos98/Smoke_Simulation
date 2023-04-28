#include <omp.h>
#include "smoke.hh"

using namespace std;
using namespace nanogui;

void Smoke::update(double delta_t)
{
    build_spatial_map();   // build grid based on new particle positions
    update_avg_particle(); // calculate average particle attributes for new grid
    int layer = smoke_param->two_layers ? 2 : 1;
    for (auto &pair : particle_map)
    {
        // #pragma omp parallel for
        for (int dx = -layer; dx <= layer; ++dx) // loop over all neighbour cells
        {
            for (int dy = -layer; dy <= layer; ++dy)
            {
                for (int dz = -layer; dz <= layer; ++dz)
                {
                    nanogui::Vector3f pos_shift = nanogui::Vector3f(dx, dy, dz);
                    uint64_t key = hash_position(avg_particle_map[pair.first]->pos + pos_shift);
                    if (!avg_particle_map[key])
                    {
                        continue;
                    }
                    else
                    {
                        nsp.update_with_neighbour_cells(pair.second, avg_particle_map[key].get(), delta_t);
                    }
                }
            }
        }
    }

    // #pragma omp parallel
    //     {
    // #pragma omp sections
    //         {
    // #pragma omp section
    //             {
    for (auto p_it = particles.begin(); p_it != particles.end();)
    {
        p_it->update(delta_t);
        if (p_it->lifespan <= 0)
        {
            // #pragma omp critical
            particles.erase(p_it++);
        }
        else
        {
            p_it++;
        }
    }
}
// }
// }
// }

void Smoke::generateParticles(const Emittor emittor, int num_particles)
{
    // #pragma omp parallel for
    for (int i = 0; i < num_particles; ++i)
    {
        Particle new_particle = emittor.emit(smoke_param);
        // #pragma omp critical
        particles.emplace_back(new_particle);
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

void Smoke::update_avg_particle()
{
    avg_particle_map.clear();
    for (auto &pair : particle_map)
    {
        std::shared_ptr<Particle> p = std::make_shared<Particle>(smoke_param);
        avg_particle_map[pair.first] = p;
        nsp.update_avg_p(pair.second, avg_particle_map[pair.first].get());
    }
}

uint64_t Smoke::hash_position(nanogui::Vector3f pos)
{

    // double w = (3 * width) / grid_width;
    // double h = (3 * height) / grid_height;
    // double t = (3 * depth) / grid_depth;

    // uint64_t x_box = floor(pos[0] / w);
    // uint64_t y_box = floor(pos[1] / h);
    // uint64_t z_box = floor(pos[2] / t);

    // // Hasing function taken from: http://www.beosil.com/download/CollisionDetectionHashing_VMV03.pdf
    // uint64_t p1 = 73856093;
    // uint64_t p2 = 19349663;
    // uint64_t p3 = 83492791;

    // return (x_box * p1) ^ (y_box * p2) ^ (z_box * p3);

    int64_t ix = static_cast<int64_t>(std::floor(pos[0] * width / grid_width));
    int64_t iy = static_cast<int64_t>(std::floor(pos[1] * height / grid_height));
    int64_t iz = static_cast<int64_t>(std::floor(pos[2] * depth / grid_depth));

    // Combine the grid indices into a single 64-bit integer
    // by interleaving the bits of the x, y, and z indices.
    uint64_t hash = 0;
    for (int i = 0; i < 21; ++i)
    {
        hash |= ((ix & (1ULL << i)) << (2 * i)) |
                ((iy & (1ULL << i)) << (2 * i + 1)) |
                ((iz & (1ULL << i)) << (2 * i + 2));
    }
    return hash;
}
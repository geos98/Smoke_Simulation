#include <omp.h>
#include <unordered_set>
#include "smoke.hh"

#define SURFACE_OFFSET 0.05
#define Y_AXIS_OFFSET 1.5
#define Y_REBOUNCE_MULTIPLIER -0.05

using namespace std;
using namespace nanogui;

void Smoke::update(double delta_t, std::vector<CollisionObject*> objects, bool ifHideObject)
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

    if (!ifHideObject) {
        for (CollisionObject* co : objects) {
            this->collide_object(co);
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

void Smoke::collide_object(CollisionObject* plane) {
    std::unordered_set<uint64_t>* hash_set = new std::unordered_set<uint64_t>();
    MatrixXf plane_positions = plane->return_positions();
    int64_t max_x = max({ plane_positions.coeff(0,0),plane_positions.coeff(0,1), plane_positions.coeff(0,2), plane_positions.coeff(0,3)});
    int64_t max_y = max({ plane_positions.coeff(1,0),plane_positions.coeff(1,1), plane_positions.coeff(1,2), plane_positions.coeff(1,3) }) + Y_AXIS_OFFSET;
    int64_t max_z = max({ plane_positions.coeff(2,0),plane_positions.coeff(2,1), plane_positions.coeff(2,2), plane_positions.coeff(2,3) });

    int64_t min_x = min({ plane_positions.coeff(0,0),plane_positions.coeff(0,1), plane_positions.coeff(0,2), plane_positions.coeff(0,3) });
    int64_t min_y = min({ plane_positions.coeff(1,0),plane_positions.coeff(1,1), plane_positions.coeff(1,2), plane_positions.coeff(1,3) }) - Y_AXIS_OFFSET;
    int64_t min_z = min({ plane_positions.coeff(2,0),plane_positions.coeff(2,1), plane_positions.coeff(2,2), plane_positions.coeff(2,3) });

    for (int64_t x = min_x; x <= max_x; x++) {
        for (int64_t y = min_y; y <= max_y; y++) {
            for (int64_t z = min_z; z <= max_z; z++) {
                Vector2f AB = Vector2f(plane_positions.coeff(0, 1), plane_positions.coeff(2, 1)) - Vector2f(plane_positions.coeff(0, 0), plane_positions.coeff(2, 0));
                Vector2f AD = Vector2f(plane_positions.coeff(0, 3), plane_positions.coeff(2, 3)) - Vector2f(plane_positions.coeff(0, 0), plane_positions.coeff(2, 0));
                Vector2f AM = Vector2f(x, z) - Vector2f(plane_positions.coeff(0, 0), plane_positions.coeff(2, 0));
                if ( (AB.dot(AB) > AM.dot(AB)) && (AM.dot(AB) > 0) && (AD.dot(AD) > AM.dot(AD)) && (AM.dot(AD) > 0))
                    hash_set->insert(this->hash_position(Vector3f(x, y, z)));
            }
        }
    }

    for (auto& pair : particle_map)
    {
        if (hash_set->find(pair.first) != hash_set->end()) {
            // do collision
            for (Particle* particle : pair.second) {
                Vector3f normal = static_cast<Plane*>(plane)->normal;
                Vector3f pos = static_cast<Plane*>(plane)->point;
                double friction = static_cast<Plane*>(plane)->friction;
                Vector3f cur_pos = particle->pos;
                Vector3f plane_to_point = cur_pos - pos;
                if (normal.dot(plane_to_point) < 0.) {
                    //find t such that (x+ta,y+tb,z+tc), (x, y, z),and (d, e, f) form right angled triangle
                    //point->(d,e,f)      normal->(a,b,c)       point_mass->(x,y,z)

                    //double t = (normal.dot(pos) - normal.dot(cur_pos)) / (normal.norm() * normal.norm());
                    //Vector3f tangent_pos = cur_pos + t * normal;

                    //Vector3f correction = tangent_pos + (normal / normal.norm()) * SURFACE_OFFSET - particle->pos;
                    //particle->pos = particle->pos + correction * (1. - friction);
                    //particle->velocity = -1*particle->velocity;

                    particle->pos = particle->pos + Vector3f(0, -(particle->pos[1] - pos[1]) - SURFACE_OFFSET, 0);
                    //particle->velocity = Vector3f(-1 * particle->velocity[0], -1 * particle->velocity[1], -1 * particle->velocity[2]);
                    auto v_scale = particle->velocity.norm();
                    particle->velocity.y() =  particle->velocity.y() * Y_REBOUNCE_MULTIPLIER;
                    particle->velocity.normalize();
                    particle->velocity *= v_scale;
                }

            }
        }
    }

    delete hash_set;
}

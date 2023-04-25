#include <nanogui/nanogui.h>
#include <list>
#include "emittor.hh"
#include "navier_stoke_solver.hh"

class Smoke
{

public:
    Smoke(){};
    Smoke(int width, int height, int depth, int grid_width, int grid_height, int grid_depth)
    {
        this->width = width;
        this->height = height;
        this->depth = depth;
        this->grid_width = grid_width;
        this->grid_height = grid_height;
        this->grid_depth = grid_depth;
        this->nsp = NavierStokeSolver();
    };

    ~Smoke(){};

    NavierStokeSolver nsp;
    std::list<Particle> particles;

    int width;
    int height;
    int depth;
    int grid_width;
    int grid_height;
    int grid_depth;

    void generateParticles(Emittor emittor, int num_particles);
    void update(double delta_t);
    void build_spatial_map();
    uint64_t hash_position(nanogui::Vector3f pos);

private:
    std::unordered_map<uint64_t, std::vector<Particle *>> particle_map;
};
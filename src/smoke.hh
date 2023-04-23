#include <nanogui/nanogui.h>
#include "emittor.hh"
#include "navier_stoke_solver.hh"

class Smoke
{

public:
    Smoke(){};
    Smoke(int width, int height, int grid_width, int grid_height)
    {
        this->width = width;
        this->height = height;
        this->grid_width = grid_width;
        this->grid_height = grid_height;
        this->nsp = new NavierStokeSolver();
    };
    ~Smoke(){};

    NavierStokeSolver *nsp;
    std::vector<Particle> particles;

    int width;
    int height;
    int grid_width;
    int grid_height;

    void generateParticles(Emittor *emittor, int num_particles);
    void update(double delta_t);
    void build_spatial_map();
    uint64_t hash_position(nanogui::Vector3f pos);

private:
    std::unordered_map<uint64_t, std::vector<Particle *>> particle_map;
    // unordered_map<uint64_t, vector<NavierStokeSolver *>> nsp_map;
};
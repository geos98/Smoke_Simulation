#include <nanogui/nanogui.h>
#include <list>
#include "emittor.hh"
#include "navier_stoke_solver.hh"

class Smoke
{

public:
    Smoke(){};
    Smoke(int width, int height, int grid_width, int grid_height){
        this->width = width;
        this->height = height;
        this->grid_width = grid_width;
        this->grid_height = grid_height;
        this->nsp = NavierStokeSolver();
    };

    ~Smoke(){};

    std::list<Particle> particles;

    void generateParticles(const Emittor& emittor, int num_particles);
    void update(double delta_t);

    NavierStokeSolver nsp;

    int width;
    int height;
    int grid_width;
    int grid_height;

    void build_spatial_map();
    uint64_t hash_position(nanogui::Vector3f pos);

private:
    std::unordered_map<uint64_t, std::vector<Particle *>> particle_map;
};
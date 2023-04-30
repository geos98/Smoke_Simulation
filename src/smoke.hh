#include <nanogui/nanogui.h>
#include <list>
#include "emittor.hh"
#include "navier_stoke_solver.hh"
#include "smoke_param.hh"
#include "collision/collisionObject.hh"

using namespace std;
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

    // ---------------------------------------------------------
    // Grid attributes
    // ---------------------------------------------------------
    int width;
    int height;
    int depth;
    int grid_width;
    int grid_height;
    int grid_depth;

    // ---------------------------------------------------------
    // Smoke attributes
    // ---------------------------------------------------------
    std::list<Particle> particles;
    SmokeParameter *smoke_param = new SmokeParameter();

    // ---------------------------------------------------------
    // Methods
    // ---------------------------------------------------------
    void generateParticles(Emittor emittor, int num_particles);
    void update(double delta_t, vector<CollisionObject*> objects, bool ifHideObject);
    void build_spatial_map();
    void update_avg_particle();

    void collide_object(CollisionObject* plane);

private:
    uint64_t hash_position(nanogui::Vector3f pos);
    std::unordered_map<uint64_t, std::vector<Particle *>> particle_map;
    std::unordered_map<uint64_t, std::shared_ptr<Particle>> avg_particle_map;
};
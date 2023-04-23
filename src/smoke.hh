#include <nanogui/nanogui.h>
#include "emittor.hh"

class Smoke
{

public:
    Smoke(){};
    ~Smoke(){};

    std::vector<Particle> particles;

    void generateParticles(Emittor *emittor, int num_particles);
    void update(double delta_t);
};
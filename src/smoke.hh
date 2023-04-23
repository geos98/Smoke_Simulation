#include <nanogui/nanogui.h>
#include <list>
#include "emittor.hh"

class Smoke
{

public:
    Smoke(){};
    ~Smoke(){};

    std::list<Particle> particles;

    void generateParticles(const Emittor& emittor, int num_particles);
    void update(double delta_t);
};
#include <nanogui/nanogui.h>

#include "emittor.hh"

using namespace nanogui;
using namespace std;

class Smoke
{

public:
    Smoke();
    ~Smoke(){};

    vector<Particle> particles;

    void generateParticles(Emittor *emittor, int num_particles);
    void update(double delta_t);
};
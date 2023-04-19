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
    Emittor *emittor;

    void update(double delta_t);
};
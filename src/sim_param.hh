#ifndef SIM_PARAM_HH
#define SIM_PARAM_HH
#include <nanogui/nanogui.h>

class SimParameter
{

public:
    SimParameter(){};
    ~SimParameter(){};

    // ---------------------------------------------------------
    // Sim parameters
    // ---------------------------------------------------------
    bool hide_plane = false;                                                // whether hide the plane barrier
};
#endif
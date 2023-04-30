#ifndef COLLISIONOBJECT
#define COLLISIONOBJECT

#include <nanogui/nanogui.h>
#include "particle.hh"

using namespace std;
using namespace nanogui;

class CollisionObject {
//public:
//  virtual void render(GLShader &shader) = 0;
//  virtual void collide(list<Particle>& particles) = 0;

private:
  double friction;
};

#endif /* COLLISIONOBJECT */

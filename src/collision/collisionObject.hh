#ifndef COLLISIONOBJECT
#define COLLISIONOBJECT

#include <nanogui/nanogui.h>
#include "../particle.hh"


class CollisionObject {
public:
  virtual void render(nanogui::GLShader &shader) = 0;
  virtual void collide(std::list<Particle>& particles) = 0;

private:
  double friction;
};

#endif /* COLLISIONOBJECT */

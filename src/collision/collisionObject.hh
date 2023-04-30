#ifndef COLLISIONOBJECT
#define COLLISIONOBJECT

#include <nanogui/nanogui.h>
#include "../particle.hh"

using namespace nanogui;

class CollisionObject {
public:
	virtual void render(nanogui::GLShader& shader) {

	}
	virtual MatrixXf return_positions() { 
		return MatrixXf(); 
	}

  Vector3f point;
  Vector3f normal;
  double friction;
};

#endif /* COLLISIONOBJECT */

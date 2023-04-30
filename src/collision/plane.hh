#ifndef COLLISIONOBJECT_PLANE_H
#define COLLISIONOBJECT_PLANE_H

#include <nanogui/nanogui.h>
#include "collisionObject.hh"

using namespace nanogui;
using namespace std;

struct Plane : public CollisionObject {
public:
    Plane(const Vector3f& point, const Vector3f& normal, double friction)
        : point(point), normal(normal), friction(friction) {}

    void render(GLShader& shader);
    void collide(list<Particle>& particle);

    Vector3f point;
    Vector3f normal;

    double friction;
};

#endif /* COLLISIONOBJECT_PLANE_H */

#include "iostream"
#include <nanogui/nanogui.h>

#include "plane.hh"

using namespace std;
using namespace nanogui; 

#define SURFACE_OFFSET 0.0001

void Plane::collide(list<Particle>& particle) {
	//// TODO (Part 3): Handle collisions with planes.
	//Vector3D cur_pos = pm.position;
	//Vector3D plane_to_point = cur_pos - point;
	//if (dot(normal, plane_to_point) < 0.) {
	//	//find t such that (x+ta,y+tb,z+tc), (x, y, z),and (d, e, f) form right angled triangle
	//	//point->(d,e,f)      normal->(a,b,c)       point_mass->(x,y,z)

	//	double t = (dot(normal, point) - dot(normal, cur_pos)) / normal.norm2();
	//	Vector3D tangent_pos = cur_pos + t * normal;
	//	/*Vector3D correction = tangent_pos - pm.last_position;
	//	correction += correction.unit() * SURFACE_OFFSET;*/
	//	Vector3D correction = tangent_pos + normal.unit() * SURFACE_OFFSET - pm.last_position;
	//	pm.position = pm.last_position + correction * (1. - friction);
	//}
}

void Plane::render(GLShader& shader) {
	nanogui::Color color(0.7f, 0.7f, 0.7f, 1.0f);

	Vector3f sPoint(point.x, point.y, point.z);
	Vector3f sNormal(normal.x, normal.y, normal.z);
	Vector3f sParallel(normal.y - normal.z, normal.z - normal.x,
		normal.x - normal.y);
	sParallel.normalize();
	Vector3f sCross = sNormal.cross(sParallel);

	MatrixXf positions(3, 4);
	MatrixXf normals(3, 4);

	positions.col(0) << sPoint + 2 * (sCross + sParallel);
	positions.col(1) << sPoint + 2 * (sCross - sParallel);
	positions.col(2) << sPoint + 2 * (-sCross + sParallel);
	positions.col(3) << sPoint + 2 * (-sCross - sParallel);

	normals.col(0) << sNormal;
	normals.col(1) << sNormal;
	normals.col(2) << sNormal;
	normals.col(3) << sNormal;

	if (shader.uniform("u_color", false) != -1) {
		shader.setUniform("u_color", color);
	}
	shader.uploadAttrib("in_position", positions);
	if (shader.attrib("in_normal", false) != -1) {
		shader.uploadAttrib("in_normal", normals);
	}

	shader.drawArray(GL_TRIANGLE_STRIP, 0, 4);
}

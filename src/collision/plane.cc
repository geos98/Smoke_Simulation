#include "iostream"
#include <nanogui/nanogui.h>

#include "plane.hh"

using namespace std;
using namespace nanogui; 

#define SURFACE_OFFSET 0.0001

MatrixXf Plane::return_positions() {
	Vector3f sPoint(point[0], point[1], point[2]);
	Vector3f sNormal(normal[0], normal[1], normal[2]);
	Vector3f sParallel(normal[1] - normal[2], normal[2] - normal[0],
		normal[0] - normal[1]);
	sParallel.normalize();
	Vector3f sCross = sNormal.cross(sParallel);

	MatrixXf positions(3, 4);

	positions.col(0) << sPoint + 3 * (sCross + sParallel);
	positions.col(1) << sPoint + 3 * (sCross - sParallel);
	positions.col(2) << sPoint + 3 * (-sCross + sParallel);
	positions.col(3) << sPoint + 3 * (-sCross - sParallel);

	return positions;
}

void Plane::render(GLShader& shader) {
	//nanogui::Color color(1.0f, 1.0f, 1.0f, 0.5f);

	Vector3f sPoint(point[0], point[1], point[2]);
	Vector3f sNormal(normal[0], normal[1], normal[2]);
	Vector3f sParallel(normal[1] - normal[2], normal[2] - normal[0],
		normal[0] - normal[1]);
	sParallel.normalize();
	Vector3f sCross = sNormal.cross(sParallel);

	MatrixXf positions(3, 4);
	MatrixXf normals(3, 4);

	positions.col(0) << sPoint + 4 * (sCross + sParallel);
	positions.col(1) << sPoint + 4 * (sCross - sParallel);
	positions.col(2) << sPoint + 4 * (-sCross + sParallel);
	positions.col(3) << sPoint + 4 * (-sCross - sParallel);

	normals.col(0) << sNormal;
	normals.col(1) << sNormal;
	normals.col(2) << sNormal;
	normals.col(3) << sNormal;

	//if (shader.uniform("u_color", false) != -1) {
	//	shader.setUniform("u_color", color);
	//}
	shader.uploadAttrib("in_position", positions);
	if (shader.attrib("in_normal", false) != -1) {
		shader.uploadAttrib("in_normal", normals);
	}

	shader.drawArray(GL_TRIANGLE_STRIP, 0, 4);
}

#include "camera.hh"
#include "misc/misc.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <nanogui/nanogui.h>

using std::cout;
using std::endl;
using std::ifstream;
using std::max;
using std::min;
using std::ofstream;

using namespace MISC;
using Collada::CameraInfo;

void Camera::configure(const CameraInfo &info, size_t screenW, size_t screenH)
{
  this->screenW = screenW;
  this->screenH = screenH;
  nClip = info.nClip;
  fClip = info.fClip;
  hFov = info.hFov;
  vFov = info.vFov;

  double ar1 = tan(radians(hFov) / 2) / tan(radians(vFov) / 2);
  ar = static_cast<double>(screenW) / screenH;
  if (ar1 < ar)
  {
    // hFov is too small
    hFov = 2 * degrees(atan(tan(radians(vFov) / 2) * ar));
  }
  else if (ar1 > ar)
  {
    // vFov is too small
    vFov = 2 * degrees(atan(tan(radians(hFov) / 2) / ar));
  }
  screenDist = ((double)screenH) / (2.0 * tan(radians(vFov) / 2));
}

void Camera::place(const nanogui::Vector3f &targetPos, const double phi,
                   const double theta, const double r, const double minR,
                   const double maxR)
{
  double r_ = min(max(r, minR), maxR);
  double phi_ = (sin(phi) == 0) ? (phi + EPS_F) : phi;
  this->targetPos = targetPos;
  this->phi = phi_;
  this->theta = theta;
  this->r = r_;
  this->minR = minR;
  this->maxR = maxR;
  compute_position();
}

void Camera::copy_placement(const Camera &other)
{
  pos = other.pos;
  targetPos = other.targetPos;
  phi = other.phi;
  theta = other.theta;
  minR = other.minR;
  maxR = other.maxR;
  c2w = other.c2w;
}

void Camera::set_screen_size(const size_t screenW, const size_t screenH)
{
  this->screenW = screenW;
  this->screenH = screenH;
  ar = 1.0 * screenW / screenH;
  hFov = 2 * degrees(atan(((double)screenW) / (2 * screenDist)));
  vFov = 2 * degrees(atan(((double)screenH) / (2 * screenDist)));
}

void Camera::move_by(const double dx, const double dy, const double d)
{
  const double scaleFactor = d / screenDist;
  const nanogui::Vector3f &displacement =
      c2w.row(0) * (dx * scaleFactor) + c2w.row(1) * (dy * scaleFactor);
  pos += displacement;
  targetPos += displacement;
}

void Camera::move_forward(const double dist)
{
  double newR = min(max(r - dist, minR), maxR);
  pos = targetPos + ((pos - targetPos) * (newR / r));
  r = newR;
}

void Camera::rotate_by(const double dPhi, const double dTheta)
{
  phi = clamp(phi + dPhi, 0.0, (double)PI);
  theta += dTheta;
  compute_position();
}

void Camera::compute_position()
{
  double sinPhi = sin(phi);
  if (sinPhi == 0)
  {
    phi += EPS_F;
    sinPhi = sin(phi);
  }
  const nanogui::Vector3f dirToCamera(r * sinPhi * sin(theta), r * cos(phi),
                                      r * sinPhi * cos(theta));
  pos = targetPos + dirToCamera;
  nanogui::Vector3f upVec(0, sinPhi > 0 ? 1 : -1, 0);
  nanogui::Vector3f screenXDir = upVec.cross(dirToCamera).normalized();
  screenXDir.normalize();
  nanogui::Vector3f screenYDir = dirToCamera.cross(screenXDir).normalized();
  screenYDir.normalize();

  c2w.row(0) = screenXDir;
  c2w.row(1) = screenYDir;
  c2w.row(2) = dirToCamera.normalized(); // camera's view direction is the
                                         // opposite of of dirToCamera, so
                                         // directly using dirToCamera as
                                         // column 2 of the matrix takes [0 0 -1]
                                         // to the world space view direction
}

void Camera::dump_settings(string filename)
{
  ofstream file(filename);
  file << hFov << " " << vFov << " " << ar << " " << nClip << " " << fClip
       << endl;
  for (int i = 0; i < 3; ++i)
    file << pos[i] << " ";
  for (int i = 0; i < 3; ++i)
    file << targetPos[i] << " ";
  file << endl;
  file << phi << " " << theta << " " << r << " " << minR << " " << maxR << endl;
  for (int i = 0; i < 9; ++i)
    file << c2w(i / 3, i % 3) << " ";
  file << endl;
  file << screenW << " " << screenH << " " << screenDist << endl;
  cout << "[Camera] Dumped settings to " << filename << endl;
}

void Camera::load_settings(string filename)
{
  ifstream file(filename);

  file >> hFov >> vFov >> ar >> nClip >> fClip;
  for (int i = 0; i < 3; ++i)
    file >> pos[i];
  for (int i = 0; i < 3; ++i)
    file >> targetPos[i];
  file >> phi >> theta >> r >> minR >> maxR;
  for (int i = 0; i < 9; ++i)
    file >> c2w(i / 3, i % 3);
  file >> screenW >> screenH >> screenDist;
  cout << "[Camera] Loaded settings from " << filename << endl;
}

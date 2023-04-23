#ifndef CGL_COLLADA_CAMERAINFO_H
#define CGL_COLLADA_CAMERAINFO_H

#include <nanogui/nanogui.h>
#include "collada_info.h"

namespace Collada
{

  /*
    Note that hFov_ and vFov_ are expected to be in DEGREES.
  */
  struct CameraInfo : public Instance
  {

    nanogui::Vector3f view_dir;
    nanogui::Vector3f up_dir;

    float hFov, vFov, nClip, fClip;
  };

  std::ostream &operator<<(std::ostream &os, const CameraInfo &camera);

} // namespace Collada

#endif // CGL_COLLADA_CAMERAINFO_H

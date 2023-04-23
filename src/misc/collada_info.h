#ifndef COLLADA_COLLADAINFO_H
#define COLLADA_COLLADAINFO_H

#include <string>
#include <vector>
#include <nanogui/nanogui.h>

using std::string;
using std::vector;

namespace Collada
{

  /*
    Subclassed with type-specific info, with a type tag for easy unpacking.
  */
  class Instance
  {
  public:
    /*
      There are a variety of different types of objects
      that can appear in a scene (lights, cameras, etc.);
      the e_InstanceType enum flags a node in the scene
      hierarchy as being one of these types.
    */
    enum Type
    {
      CAMERA,
      LIGHT,
      SPHERE,
      POLYMESH,
      MATERIAL
    };

    Type type;   ///< type of instance
    string id;   ///< instance ID
    string name; ///< instance name
  };             // struct Instance

  /*
    An instance, plus a transformation matrix.
  */
  struct Node
  {

    string id;
    string name;

    Instance *instance;          ///< instance
    nanogui::Matrix4f transform; ///< transformation

    Node() : instance(nullptr), transform(nanogui::Matrix4f::Identity()) {}

  }; // struct Node

  /*
    The scene that ColladaParser generates and passes to MeshEdit.
  */
  struct SceneInfo
  {
    vector<Node> nodes;
  };

} // namespace Collada

#endif // CGL_COLLADA_COLLADAINFO_H

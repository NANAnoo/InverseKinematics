#ifndef BVHMODELMACROS_H
#define BVHMODELMACROS_H

#include <functional>
#include "Eigen/Dense"

namespace BVH
{
#define MAX_COLOR static_cast<unsigned int>(255)
#define RED_COLOR (MAX_COLOR << 24 | MAX_COLOR)
#define GREEN_COLOR (MAX_COLOR << 16 | MAX_COLOR)
#define BLUE_COLOR (MAX_COLOR << 8 | MAX_COLOR)
#define CYAN_COLOR (BLUE_COLOR | GREEN_COLOR)
#define YELLOW_COLOR (RED_COLOR | GREEN_COLOR)
#define PURPLE_COLOR (BLUE_COLOR | RED_COLOR)
#define WHITE_COLOR (RED_COLOR | BLUE_COLOR | GREEN_COLOR)
#define FLOAT_RED(color) (static_cast<float>((color & RED_COLOR) >> 24) / 255.f)
#define FLOAT_GREEN(color) (static_cast<float>((color & GREEN_COLOR) >> 16) / 255.f)
#define FLOAT_BLUE(color) (static_cast<float>((color & BLUE_COLOR) >> 8) / 255.f)

enum RenderType {
    DefaultType,
    SelectedType,
    LockedType
};
struct BVHMetaNode
{
    std::string name;
    std::string parent;
    unsigned int level;
};
typedef std::function<void(Eigen::Vector4f &bottle, Eigen::Vector4f &top, RenderType type, double radious)> boneRenderHandler;
typedef std::function<void(Eigen::Vector4f &center, RenderType type, double radious)> jointRenderHandler;


};

#endif // BVHMODELMACROS_H

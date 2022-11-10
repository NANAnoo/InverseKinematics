#ifndef BVHMODELMACROS_H
#define BVHMODELMACROS_H

#include <functional>
#include <vector>
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
#define MYPI 3.1415926535f
#define DELTA_ANGLE 0.01
#define DAMPED_LAMBDA 0.0001
#define MAX_STEP 50

enum RenderType {
    DefaultType      = 0,
    SelectedType     = 1,
    LockedType       = 1 << 1,
    DesitinationType = 1 << 2
};
struct BVHMetaNode
{
    std::string name;
    std::string parent;
    unsigned int level;
};

typedef std::function<void(Eigen::Vector4d &bottle, Eigen::Vector4d &top, RenderType type, double radious)> boneRenderHandler;
typedef std::function<void(Eigen::Vector4d &center, RenderType type, double radious)> jointRenderHandler;

// type define
enum  ChannelEnum
{
    X_ROTATION, Y_ROTATION, Z_ROTATION,
    X_POSITION, Y_POSITION, Z_POSITION
};

struct BVHJoint
{
    // is end joint?
    bool                        has_site;
    // offset and site data
    double                       offset[3];
    double                       site[3];
    // joint name
    std::string                 name;
    // parent joint
    BVHJoint *                  parent;
    // children joints
    std::vector< BVHJoint * >   children;
    // how data applied
    std::vector<ChannelEnum>    channels;
    // channel index offset
    unsigned int                channel_index_offset;
    // render size
    BVH::RenderType render_type;
    BVH::RenderType site_render_type;
};

struct BVHJointDisplayInfo
{
    double offset[3];
    std::vector<ChannelEnum> channels;
    std::string name;
    unsigned int channel_index_offset;
    bool is_end;
};

};

#endif // BVHMODELMACROS_H

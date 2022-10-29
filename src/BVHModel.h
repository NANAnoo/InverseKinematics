#ifndef BVHMODEL_H
#define BVHMODEL_H

#include <map>
#include <string>
#include <vector>
#include <tuple>
#include <functional>
#include "Eigen/Dense"

class BVHModel
{
public:

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

    typedef std::function<void(Eigen::Vector4f &bottle, Eigen::Vector4f &top, unsigned int color, float radious)> boneRenderHandler;
    typedef std::function<void(Eigen::Vector4f &center, unsigned int color, float radious)> jointRenderHandler;
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
        float                       offset[3];
        float                       site[3];
        // joint name
        std::string                 name;
        // parent joint
        BVHJoint *                  parent;
        // children joints
        std::vector< BVHJoint * >   children;
        // how data applied
        std::vector<ChannelEnum>    channels;
        // color
        unsigned int                RGBAColor;
        // channel index offset
        unsigned int                channel_index_offset;
    };

    // build model with file
    BVHModel(const std::string filename);
    // build a model from any rootjoint, has no motion data
    BVHModel(BVHJoint *other) : skeleton(other), is_valid(true){}
    // dealloc
    ~BVHModel();

    // get position of a joint
    Eigen::Vector4f jointPositionAt(unsigned int frame_ID, const std::string joint_name, float scale);
    // get the joint from name
    BVHJoint *getJointFromName(const std::string joint_name) {return joint_map[joint_name];}

    // render skeleton
    void renderModelWith(boneRenderHandler &boneRender, jointRenderHandler &jointRender, unsigned int frame_ID, float scale);

    // write to file
    void writeToFile(const std::string file_name);

    // motion information
    unsigned int allFrameCount() {return motion_datas->size();}
    unsigned int channel_num;
    double frame_duration;

    // valid information
    bool isValid(){return is_valid;}

private:
    // root Joint
    BVHJoint *skeleton;
    std::map<std::string, BVHJoint *> joint_map;
    // frame info
    std::vector<std::vector<float> *> *motion_datas;
    // render all joints
    void renderJoint(BVHJoint *joint,
                     boneRenderHandler &boneRender,
                     jointRenderHandler &jointRender,
                     Eigen::MatrixXf current_transition,
                     std::vector<float>::iterator &it,
                     float scale);
    // valid status;
    bool is_valid;
    // private functions:
    // get motion data at frame
    std::vector<float> *motionDataAt(unsigned int frame_ID);
    std::vector<float> *defaultMotion;
    // rotation util
    Eigen::Matrix4f getRotationMatrix(ChannelEnum type, float alpha);
    Eigen::Matrix4f getTraslationMatrix(float x, float y, float z);
};


#endif // BVHMODEL_H

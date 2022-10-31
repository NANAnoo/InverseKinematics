#ifndef BVHMODEL_H
#define BVHMODEL_H

#include <map>
#include <string>
#include <vector>
#include <fstream>
#include "BVHModelMacros.h"

class BVHModel
{
public:

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
    };

    // build model with file
    BVHModel(const std::string filename);
    // build a model from any rootjoint, has no motion data
    BVHModel(BVHJoint *other) : skeleton(other), is_valid(true){}
    // dealloc
    ~BVHModel();

    // get position of a joint
    Eigen::Vector4f jointPositionAt(unsigned int frame_ID, const std::string joint_name, double scale);
    // get the joint from name
    BVHJoint *getJointFromName(const std::string joint_name) {return joint_map[joint_name];}

    // render skeleton
    void renderModelWith(BVH::boneRenderHandler boneRender, BVH::jointRenderHandler jointRender, unsigned int frame_ID, double scale);

    // write to file
    bool writeToFile(const std::string file_name);

    // get meta info list
    std::vector<BVH::BVHMetaNode> getMetaList();

    // motion information
    unsigned int allFrameCount() {return motion_datas->size();}
    unsigned int channel_num;
    double frame_duration;

    // valid information
    bool isValid(){return is_valid;}

    // root Joint
    BVHJoint *skeleton;

private:

    std::map<std::string, BVHJoint *> joint_map;
    // frame info
    std::vector<std::vector<double> *> *motion_datas;
    // render all joints
    void renderJoint(BVHJoint *joint,
                     BVH::boneRenderHandler &boneRender,
                     BVH::jointRenderHandler &jointRender,
                     Eigen::MatrixXf current_transition,
                     std::vector<double>::iterator &it,
                     double scale);
    // valid status;
    bool is_valid;
    // private functions:
    // get motion data at frame
    std::vector<double> *motionDataAt(unsigned int frame_ID);
    std::vector<double> *defaultMotion;
    // rotation util
    Eigen::Matrix4f getRotationMatrix(ChannelEnum type, double alpha);
    Eigen::Matrix4f getTraslationMatrix(double x, double y, double z);
    // write joint to file
    void writeJointToFile(BVHJoint *joint, std::ofstream &file, unsigned int depth);

    void getMetaInfoFrom(BVHJoint *node, std::vector<BVH::BVHMetaNode> &list, unsigned int depth);
};


#endif // BVHMODEL_H

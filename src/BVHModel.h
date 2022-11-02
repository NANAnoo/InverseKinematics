#ifndef BVHMODEL_H
#define BVHMODEL_H

#include <map>
#include <fstream>
#include "BVHModelMacros.h"

using namespace BVH;

class BVHModel
{
public:

    // build model with file
    BVHModel(const std::string filename);
    // build a model from any rootjoint, has no motion data
    BVHModel(BVHJoint *other) : skeleton(other), is_valid(true){}
    // dealloc
    ~BVHModel();

    // get model snapshot at frame
    BVHModel *snapShotModelAt(unsigned int frame_ID) {
       std::vector<double> *data = new std::vector<double>(*motionDataAt(frame_ID));
       BVHModel *model = new BVHModel(skeleton);
       model->motion_datas = new std::vector<std::vector<double> *>();
       model->motion_datas->push_back(data);
       model->joint_map = joint_map;
       model->channel_num = channel_num;
       model->frame_duration = frame_duration;
       return model;
    }

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

    // get motion value
    double getMotionValue(unsigned int frame_ID, unsigned int index)
    {
        if (frame_ID < motion_datas->size() && index < channel_num)
            return (*motionDataAt(frame_ID))[index];
        else
            return 0;
    }

    // motion information
    unsigned int allFrameCount() {return motion_datas->size();}

    // valid information
    bool isValid(){return is_valid;}

    double frame_duration;
    unsigned int channel_num;
    // root Joint
    BVHJoint *skeleton;


private:

    std::map<std::string, BVHJoint *> joint_map;
    // frame info
    std::vector<std::vector<double> *> *motion_datas;
    // default motion
    std::vector<double> *default_motion;
    // valid status;
    bool is_valid;
    // render all joints
    void renderJoint(BVHJoint *joint,
                     BVH::boneRenderHandler &boneRender,
                     BVH::jointRenderHandler &jointRender,
                     Eigen::MatrixXf current_transition,
                     std::vector<double>::iterator &it,
                     double scale);
    // private functions:
    // get motion data at frame
    std::vector<double> *motionDataAt(unsigned int frame_ID);
    // rotation util
    Eigen::Matrix4f getRotationMatrix(ChannelEnum type, double alpha);
    Eigen::Matrix4f getTraslationMatrix(double x, double y, double z);
    // write joint to file
    void writeJointToFile(BVHJoint *joint, std::ofstream &file, unsigned int depth);

    void getMetaInfoFrom(BVHJoint *node, std::vector<BVH::BVHMetaNode> &list, unsigned int depth);
};


#endif // BVHMODEL_H

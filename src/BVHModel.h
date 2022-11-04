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
    Eigen::Vector4d jointPositionAt(unsigned int frame_ID, const std::string joint_name, double scale);
    // get the joint from name
    BVHJoint *getJointFromName(const std::string joint_name) {return joint_map[joint_name];}

    // render skeleton
    void renderModelWith(BVH::boneRenderHandler boneRender, BVH::jointRenderHandler jointRender, unsigned int frame_ID, double scale);

    // write to file
    bool writeToFile(const std::string file_name);

    // get meta info list
    std::vector<BVH::BVHMetaNode> getMetaList();

    // get motion data at frame
    std::vector<double> *motionDataAt(unsigned int frame_ID);

    // get motion value
    double getMotionValue(unsigned int frame_ID, unsigned int index)
    {
        if (frame_ID < motion_datas->size() && index < channel_num)
            return (*motionDataAt(frame_ID))[index];
        else
            return 0;
    }

    // use IK, insert a new motion that moves the joint to a point from any frame
    bool insertMotionFrom(unsigned int frame_ID, std::string joint_name, Eigen::Vector3d desitination, double scale);

    void insertMotionDataFrom(unsigned int frame_ID, std::vector<double> *data) {
        motion_datas->insert(motion_datas->begin() + frame_ID + 1, data);
    }

    // remove motion from ID
    void removeMotionFrom(unsigned int index) {
        int num = motion_datas->size() - index - 1;
        num = num < 0 ? 0 : num;
        while (num > 0) {
            motion_datas->pop_back();
            num --;
        }
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
                     Eigen::Matrix4d current_transition,
                     std::vector<double>::iterator &it,
                     double scale);
    // private functions:
    // rotation util
    Eigen::Matrix4d getRotationMatrix(ChannelEnum type, double alpha);
    Eigen::Matrix4d getTraslationMatrix(double x, double y, double z);
    // write joint to file
    void writeJointToFile(BVHJoint *joint, std::ofstream &file, unsigned int depth);
    // get all joints info
    void getMetaInfoFrom(BVHJoint *node, std::vector<BVH::BVHMetaNode> &list, unsigned int depth);
    // get joint position with a motion data
    Eigen::Vector4d jointPositionFromMotion(std::vector<double> motion, BVHJoint *joint, double scale);
    // IK step:
    std::vector<double> *stepIKMotionFrom(std::vector<BVHJoint *> &moved_joints,
                                                    std::vector<double> base_motion,
                                                    std::vector<Eigen::Vector3d> desitinations,
                                                    double scale);
};


#endif // BVHMODEL_H

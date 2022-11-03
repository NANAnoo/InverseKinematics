#include "BVHModel.h"

#include <iostream>
#include <cstring>

#define BUFFER_LENGTH 1024 * 4

using namespace std;

// build model with file
BVHModel::BVHModel(const std::string filename)
{
    ifstream file;
    char line[BUFFER_LENGTH];
    char *token;
    char separater[] = " :,\t";
    vector<BVHJoint *> joint_stack;
    BVHJoint *joint = nullptr;
    BVHJoint *new_joint = nullptr;
    bool is_site = false;
    double x, y, z;
    unsigned int i;
    channel_num = 0;
    is_valid = false;
    default_motion = nullptr;

    // open the file
    file.open(filename, ios::in);
    if (!file.is_open())
        return;

    while (!file.eof())
    {
        // get the data from a line
        file.getline(line, BUFFER_LENGTH);
        token = strtok(line, separater);
        if (token == nullptr)
            continue;

        // analyize first token of the line
        if (strcmp(token, "{") == 0)
        {
            // push previous joint, change to new joint
            joint_stack.push_back(joint);
            joint = new_joint;
            continue;
        }
        // all children have been built
        if (strcmp(token, "}") == 0)
        {
            // pop current joint
            joint = joint_stack.back();
            joint_stack.pop_back();
            is_site = false;
            continue;
        }

        // build new joint when meet ROOT or JOINT
        if ((strcmp(token, "ROOT") == 0) || (strcmp(token, "JOINT") == 0))
        {
            // create new joint
            new_joint = new BVHJoint();
            new_joint->parent = joint;
            new_joint->has_site = false;
            new_joint->offset[0] = 0.0;
            new_joint->offset[1] = 0.0;
            new_joint->offset[2] = 0.0;
            new_joint->site[0] = 0.0;
            new_joint->site[1] = 0.0;
            new_joint->site[2] = 0.0;
            new_joint->channel_index_offset = channel_num;
            new_joint->render_type = BVH::DefaultType;
            if (joint)
                joint->children.push_back(new_joint);
            else
                skeleton = new_joint;
            // parse name
            token = strtok(nullptr, "");
            while (*token == ' ')
                token++;
            new_joint->name = token;
            // create index of joint
            joint_map[new_joint->name] = new_joint;
            continue;
        }

        // end joint has site
        if ((strcmp(token, "End") == 0))
        {
            new_joint = joint;
            is_site = true;
            continue;
        }

        // parse OFFSET of current joint
        if (strcmp(token, "OFFSET") == 0)
        {
            // parse the value of offset or site
            token = strtok(nullptr, separater);
            x = token ? static_cast<double>(atof(token)) : 0.0;
            token = strtok(nullptr, separater);
            y = token ? static_cast<double>(atof(token)) : 0.0;
            token = strtok(nullptr, separater);
            z = token ? static_cast<double>(atof(token)) : 0.0;

            // set site info
            if (is_site)
            {
                joint->has_site = true;
                joint->site[0] = x;
                joint->site[1] = y;
                joint->site[2] = z;
            }
            else
            // set offset info
            {
                joint->offset[0] = x;
                joint->offset[1] = y;
                joint->offset[2] = z;
            }
            continue;
        }

        // start parse channels
        if (strcmp(token, "CHANNELS") == 0)
        {
            // get channel name
            token = strtok(nullptr, separater);
            // get channel count
            unsigned int channel_count = static_cast<unsigned int>(token ? atoi(token) : 0);

            // set channel name
            for (i = 0; i < channel_count; i++)
            {
                token = strtok(nullptr, separater);
                if (strcmp(token, "Xrotation") == 0)
                    joint->channels.push_back(X_ROTATION);
                else if (strcmp(token, "Yrotation") == 0)
                    joint->channels.push_back(Y_ROTATION);
                else if (strcmp(token, "Zrotation") == 0)
                    joint->channels.push_back(Z_ROTATION);
                else if (strcmp(token, "Xposition") == 0)
                    joint->channels.push_back(X_POSITION);
                else if (strcmp(token, "Yposition") == 0)
                    joint->channels.push_back(Y_POSITION);
                else if (strcmp(token, "Zposition") == 0)
                    joint->channels.push_back(Z_POSITION);
                channel_num++;
            }
        }
        // stop
        if (strcmp(token, "MOTION") == 0)
            break;
    }
    // start parse motions
    // get fram counts
    file.getline(line, BUFFER_LENGTH);
    token = strtok(line, separater);
    if (strcmp(token, "Frames") != 0)
    {
        file.close();
        return;
    }
    token = strtok(nullptr, separater);
    if (token == nullptr)
    {
        file.close();
        return;
    }
    unsigned int num_frame = static_cast<unsigned int>(atoi(token));

    file.getline(line, BUFFER_LENGTH);
    token = strtok(line, ":");
    // get frame duration
    if (strcmp(token, "Frame Time") != 0)
    {
        file.close();
        return;
    }
    token = strtok(nullptr, separater);
    if (token == nullptr)
    {
        file.close();
        return;
    }
    frame_duration = atof(token);
    // parse all motions
    motion_datas = new vector<vector<double> *>(num_frame);
    for (i = 0; i < num_frame; i++)
    {
        vector<double> *motion = new vector<double>(channel_num);
        file.getline(line, BUFFER_LENGTH);
        token = strtok(line, separater);
        for (double &value : *motion)
        {
            if (token == nullptr)
            {
                file.close();
                return;
            }
            value = static_cast<double>(atof(token));
            token = strtok(nullptr, separater);
        }
        (*motion_datas)[i] = motion;
    }
    // close file
    file.close();
    // validate
    is_valid = true;
}

// get position of a joint
Eigen::Vector4d BVHModel::jointPositionAt(unsigned int frame_ID, const std::string joint_name, double scale)
{
    return jointPositionFromMotion(*motionDataAt(frame_ID), joint_map[joint_name], scale);
}


Eigen::Vector4d BVHModel::jointPositionFromMotion(std::vector<double> motion, BVHJoint *joint, double scale)
{
    Eigen::Vector4d coords(0, 0, 0, 1);
    Eigen::Matrix4d transition = Eigen::Matrix4d::Identity();
    // do FK until joint
    while (joint != nullptr)
    {
        double ax = 0, ay = 0, az = 0, x = 0, y = 0, z = 0;
        unsigned int i = 0;
        for (ChannelEnum c : joint->channels)
        {
            double value = motion[joint->channel_index_offset + i];
            i ++;
            switch (c)
            {
            case X_ROTATION:
                ax = value;
                break;
            case Y_ROTATION:
                ay = value;
                break;
            case Z_ROTATION:
                az = value;
                break;
            case X_POSITION:
                x = value;
                break;
            case Y_POSITION:
                y = value;
                break;
            case Z_POSITION:
                z = value;
                break;
            }
        }
        Eigen::Matrix4d Traslation;
        if (joint->parent != nullptr)
        {
            Traslation = getTraslationMatrix(joint->offset[0] * scale, joint->offset[1] * scale, joint->offset[2] * scale);
        }
        else
        {
            Traslation = getTraslationMatrix(x * scale, y * scale, z * scale);
        }
        transition = Traslation *
                     getRotationMatrix(Z_ROTATION, az) *
                     getRotationMatrix(Y_ROTATION, ay) *
                     getRotationMatrix(X_ROTATION, ax) * transition;
        joint = joint->parent;
    }
    return transition * coords;
}


void BVHModel::insertMotionFrom(unsigned int frame_ID, std::string joint_name, Eigen::Vector3d desitination, double scale)
{
    // get current joint
    BVHJoint *target_joint = joint_map[joint_name], *joint = target_joint->parent;
    if (joint == nullptr)
        return;
    // trace to root, find all availible control points
    std::vector<BVHJoint *> control_joints;
    while (joint != nullptr) {
        // enable unlocked joints
        if ((joint->render_type & LockedType) == 0) {
            control_joints.push_back(joint);
        }
        joint = joint->parent;
    }
    std::vector<double> *base_motion = motionDataAt(frame_ID), *result_motion = nullptr;
    base_motion = new std::vector<double>(base_motion->begin(), base_motion->end());
    double gap_length = 0;
    int step = 0;
    // update motion until reach desitination
    do {
        result_motion = stepIKMotionFrom(control_joints, *base_motion, target_joint, desitination, scale);
        Eigen::Vector4d result_coords = jointPositionFromMotion(*result_motion, target_joint, scale);
        double dx = result_coords.x() - desitination.x();
        double dy = result_coords.y() - desitination.y();
        double dz = result_coords.z() - desitination.z();
        gap_length = std::sqrt(dx * dx + dy * dy + dz * dz);
        //std::cout <<"Frame " << frame_ID << " gap length : " << gap_length << std::endl;
        delete base_motion;
        base_motion = result_motion;
        step ++;
    } while(gap_length > 0.00001 && step < MAX_STEP);

    motion_datas->insert(motion_datas->begin() + frame_ID + 1, result_motion);
}

std::vector<double> *BVHModel::stepIKMotionFrom(std::vector<BVHJoint *> &control_joints,
                                                std::vector<double> base_motion,
                                                BVHJoint *target_joint,
                                                Eigen::Vector3d desitination,
                                                double scale)
{
    // base on base motion, find out the Jacobian Matrix
    Eigen::Vector4d base_coords = jointPositionFromMotion(base_motion, target_joint, scale);
    Eigen::MatrixXd Jacobian;
    Jacobian.resize(3, control_joints.size() * 3);
    Jacobian.setZero();
    unsigned int col_index = 0;
    for (BVHJoint *control_joint : control_joints) {
        unsigned int offset = control_joint->channel_index_offset;
        for (ChannelEnum c : control_joint->channels)
        {
            if (c == X_ROTATION || c == Y_ROTATION || c == Z_ROTATION) {
                std::vector<double> diff_motion(base_motion.begin(), base_motion.end());
                // add a small delta angle
                diff_motion[offset] = diff_motion[offset] + DELTA_ANGLE;
                // caculate the new coords at diff_motion;
                Eigen::Vector4d diff_coords = jointPositionFromMotion(diff_motion, target_joint, scale);
                Eigen::Vector4d delta_coords = diff_coords - base_coords;
                // assign value to Jacobian, d_x|y|z / d_theta
                Jacobian(0, col_index) = delta_coords.x() / DELTA_ANGLE;
                Jacobian(1, col_index) = delta_coords.y() / DELTA_ANGLE;
                Jacobian(2, col_index) = delta_coords.z() / DELTA_ANGLE;
                col_index ++;
            }
            offset ++;
        }

    }

    // calculate delta movement
    Eigen::Vector3d delta_movement(desitination.x() - base_coords.x(),
                                   desitination.y() - base_coords.y(),
                                   desitination.z() - base_coords.z());
    // damped IK
    // J * delta_theta = delta_movement
    // J_+ = J_T * inv(J_T * J + lamda^2 * I);
    // J_+ * delta_movement = delta_theta
    Eigen::MatrixXd Jacobian_T = Jacobian.transpose();
    Eigen::MatrixXd I;
    I.resize(3, 3);
    I.setIdentity();
    Eigen::MatrixXd Jacobian_prime = Jacobian_T * (Jacobian * Jacobian_T + I * (DAMPED_LAMBDA * DAMPED_LAMBDA)).inverse();
    Eigen::MatrixXd delta_theta = Jacobian_prime * delta_movement;
    // build motion data base on delta_theta
    unsigned int raw_index = 0;
    std::vector<double> *result_motion = new std::vector<double>(base_motion.begin(), base_motion.end());
    for (BVHJoint *control_joint : control_joints) {
        unsigned int offset = control_joint->channel_index_offset;
        for (ChannelEnum c : control_joint->channels)
        {
            if (c == X_ROTATION || c == Y_ROTATION || c == Z_ROTATION) {
                (*result_motion)[offset] = (*result_motion)[offset] + delta_theta(raw_index);
                raw_index ++;
            }
            offset ++;
        }
    }
    return result_motion;
}

// render skeleton
void BVHModel::renderModelWith(BVH::boneRenderHandler boneRender, BVH::jointRenderHandler jointRender, unsigned int frame_ID, double scale)
{
    if (skeleton != nullptr && isValid() && channel_num > 0)
    {
        std::vector<double> *motion = motionDataAt(frame_ID);
        vector<double>::iterator it = motion->begin();
        renderJoint(skeleton, boneRender, jointRender, Eigen::Matrix4d::Identity(), it, scale);
    }
}

// render all joints
void BVHModel::renderJoint(BVHJoint *joint,
                           BVH::boneRenderHandler &boneRender,
                           BVH::jointRenderHandler &jointRender,
                           Eigen::Matrix4d current_transition,
                           std::vector<double>::iterator &it,
                           double scale)
{
    // do FK
    // rotation along X, Y, Z
    double x = 0, y = 0, z = 0, ax = 0, ay = 0, az = 0;
    for (ChannelEnum c : joint->channels)
    {
        switch (c)
        {
        case X_POSITION:
            x = *(it)*scale;
            break;
        case Y_POSITION:
            y = *(it)*scale;
            break;
        case Z_POSITION:
            z = *(it)*scale;
            break;
        case X_ROTATION:
            ax = *(it);
            break;
        case Y_ROTATION:
            ay = *(it);
            break;
        case Z_ROTATION:
            az = *(it);
            break;
        }
        it++;
    }
    if (joint->parent != nullptr)
    {
        x = joint->offset[0] * scale;
        y = joint->offset[1] * scale;
        z = joint->offset[2] * scale;
    }
    Eigen::Matrix4d transition = current_transition *
                                 getTraslationMatrix(x, y, z) *
                                 getRotationMatrix(Z_ROTATION, az) *
                                 getRotationMatrix(Y_ROTATION, ay) *
                                 getRotationMatrix(X_ROTATION, ax);
    Eigen::Vector4d joint_center(0, 0, 0, 1);
    joint_center = transition * joint_center;
    // render joint
    jointRender(joint_center, joint->render_type, scale);

    unsigned int size = joint->children.size();
    if (joint->children.size() == 0)
    {
        // render site bone;
        Eigen::Vector4d site_end(joint->site[0] * scale, joint->site[1] * scale, joint->site[2] * scale, 1);
        site_end = transition * site_end;
        boneRender(joint_center, site_end, joint->render_type, scale);
    }
    else if ((joint->children.size() == 1))
    {
        BVHJoint *child = joint->children[0];
        Eigen::Vector4d child_center(child->offset[0] * scale, child->offset[1] * scale, child->offset[2] * scale, 1);
        child_center = transition * child_center;
        boneRender(joint_center, child_center, joint->render_type, scale);
        renderJoint(child, boneRender, jointRender, transition, it, scale);
    }
    else
    {
        // render bone
        Eigen::Vector4d child_center(0, 0, 0, 1);
        for (unsigned int i = 0; i < size; i++)
        {
            BVHJoint *child = joint->children[i];
            child_center += Eigen::Vector4d(child->offset[0] * scale, child->offset[1] * scale, child->offset[2] * scale, 1);
        }
        // get center of all joints
        child_center = child_center / (size + 1);
        child_center = transition * child_center;
        boneRender(joint_center, child_center, joint->render_type, scale);
        for (BVHJoint *child : joint->children)
        {
            Eigen::Vector4d child_joint_center(child->offset[0] * scale, child->offset[1] * scale, child->offset[2] * scale, 1);
            child_joint_center = transition * child_joint_center;
            boneRender(child_center, child_joint_center, joint->render_type, scale);
            renderJoint(child, boneRender, jointRender, transition, it, scale);
        }
    }
}

Eigen::Matrix4d BVHModel::getRotationMatrix(ChannelEnum type, double alpha)
{
    alpha = alpha / 180 * MYPI;
    Eigen::Matrix4d rotation;
    if (type == X_ROTATION)
        rotation << 1, 0, 0, 0,
            0, cosf(alpha), -sinf(alpha), 0,
            0, sinf(alpha), cosf(alpha), 0,
            0, 0, 0, 1;

    else if (type == Y_ROTATION)
        rotation << cosf(alpha), 0, sinf(alpha), 0,
            0, 1, 0, 0,
            -sinf(alpha), 0, cosf(alpha), 0,
            0, 0, 0, 1;

    else if (type == Z_ROTATION)
        rotation << cosf(alpha), -sinf(alpha), 0, 0,
            sinf(alpha), cosf(alpha), 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1;

    return rotation;
}

Eigen::Matrix4d BVHModel::getTraslationMatrix(double x, double y, double z)
{
    Eigen::Matrix4d t;
    t << 1, 0, 0, x,
        0, 1, 0, y,
        0, 0, 1, z,
        0, 0, 0, 1;
    return t;
}

// write to file
bool BVHModel::writeToFile(const std::string file_name)
{
    cout << "save file at:" << file_name << endl;
    ofstream file;
    file.open(file_name, ios::out);
    if (!file.is_open())
        return false;
    file.setf(ios::showpoint);
    file << fixed;
    file.precision(6);
    // write joints
    writeJointToFile(skeleton, file, 0);
    // wirte motions
    file << "MOTION\n";
    file.setf(ios::showbase);
    file << "Frames: " << motion_datas->size() << endl;
    file.setf(ios::showpoint);
    file << "Frame Time: " << frame_duration << endl;
    for (vector<double> *motion : *motion_datas)
    {
        for (double value : *motion)
            file << value << " ";
        file << endl;
    }
    file.close();
    return true;
}

void BVHModel::writeJointToFile(BVHJoint *joint, ofstream &file, unsigned int depth)
{
    // write head
    if (joint->parent == nullptr)
    {
        file << "HIERARCHY" << endl;
        file << "ROOT " << joint->name << endl;
        file << "{\n";
    }
    else
    {
        string spaces(depth, '\t');
        file << spaces << "JOINT " << joint->name << endl;
        file << spaces << "{\n";
    }
    string spaces((depth + 1), '\t');
    // write offset
    file << spaces << "OFFSET " << joint->offset[0] << " " << joint->offset[1] << " " << joint->offset[2] << endl;

    // write channels
    file << spaces << "CHANNELS " << joint->channels.size();
    for (ChannelEnum e : joint->channels)
    {
        switch (e)
        {
        case X_POSITION:
            file << " "
                 << "Xposition";
            break;
        case Y_POSITION:
            file << " "
                 << "Yposition";
            break;
        case Z_POSITION:
            file << " "
                 << "Zposition";
            break;
        case X_ROTATION:
            file << " "
                 << "Xrotation";
            break;
        case Y_ROTATION:
            file << " "
                 << "Yrotation";
            break;
        case Z_ROTATION:
            file << " "
                 << "Zrotation";
            break;
        }
    }
    if (joint->parent == nullptr) file << " ";
    file << endl;
    ;
    // write child joints
    if (joint->children.size() > 0)
        for (BVHJoint *child : joint->children)
            writeJointToFile(child, file, depth + 1);
    else // write end site
    {
        file << spaces << "End Site\n";
        string end_spaces((depth + 2), '\t');
        file << spaces << "{\n";
        file << end_spaces << "OFFSET " << joint->site[0] << " " << joint->site[1] << " " << joint->site[2] << endl;
        file << spaces << "}\n";
    }
    // write '}'
    if (joint->parent != nullptr)
    {
        string spaces(depth, '\t');
        file << spaces;
    }
    file << "}\n";
}

std::vector<double> *BVHModel::motionDataAt(unsigned int frame_ID)
{
    if (motion_datas != nullptr && motion_datas->size() > frame_ID)
        return (*motion_datas)[frame_ID];
    if (default_motion == nullptr)
        default_motion = new vector<double>(channel_num, 0);
    return default_motion;
}

std::vector<BVH::BVHMetaNode> BVHModel::getMetaList()
{
    vector<BVH::BVHMetaNode> res;
    getMetaInfoFrom(skeleton, res, 0);
    return res;
}

void BVHModel::getMetaInfoFrom(BVHJoint *node, std::vector<BVH::BVHMetaNode> &list, unsigned int depth)
{
    if (node) {
        list.push_back(BVH::BVHMetaNode({node->name, node->parent == nullptr ? "" : node->parent->name, depth}));
        for (BVHJoint *child : node->children) {
            getMetaInfoFrom(child, list, depth + 1);
        }
    }
}

// dealloc
BVHModel::~BVHModel()
{
}

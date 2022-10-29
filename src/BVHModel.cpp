#include "BVHModel.h"

#include <fstream>
#include <iostream>
#include <cstring>

#define  BUFFER_LENGTH  1024*4

using namespace std;

// build model with file
BVHModel::BVHModel (const std::string filename)
{
    ifstream  file;
    char      line[ BUFFER_LENGTH ];
    char *    token;
    char      separater[] = " :,\t";
    vector<BVHJoint *>   joint_stack;
    BVHJoint *   joint = nullptr;
    BVHJoint *   new_joint = nullptr;
    bool      is_site = false;
    float     x, y ,z;
    unsigned int i;
    channel_num = 0;
    is_valid = false;
    defaultMotion = nullptr;

    // open the file
    file.open( filename, ios::in );
    if (!file.is_open())  return;

    while (!file.eof())
    {
        // get the data from a line
        file.getline(line, BUFFER_LENGTH);
        token = strtok(line, separater);
        if (token == nullptr) continue;

        // analyize first token of the line
        if (strcmp(token, "{") == 0)
        {
            // push previous joint, change to new joint
            joint_stack.push_back( joint );
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
        if ((strcmp(token, "ROOT") == 0)||(strcmp(token, "JOINT") == 0) )
        {
            // create new joint
            new_joint = new BVHJoint();
            new_joint->parent = joint;
            new_joint->has_site = false;
            new_joint->offset[0] = 0.0;  new_joint->offset[1] = 0.0;  new_joint->offset[2] = 0.0;
            new_joint->site[0] = 0.0;  new_joint->site[1] = 0.0;  new_joint->site[2] = 0.0;
            new_joint->channel_index_offset = channel_num;
            if ( joint ){
                new_joint->RGBAColor = GREEN_COLOR;
                joint->children.push_back(new_joint);
            }
            else {
                new_joint->RGBAColor = RED_COLOR;
                skeleton = new_joint;
            }
            // parse name
            token = strtok(nullptr, "" );
            while ( *token == ' ' )  token ++;
            new_joint->name = token;
            // create index of joint
            joint_map[ new_joint->name ] = new_joint;
            continue;
        }

        // end joint has site
        if ( ( strcmp( token, "End" ) == 0 ) )
        {
            new_joint = joint;
            is_site = true;
            continue;
        }

        // parse OFFSET of current joint
        if ( strcmp( token, "OFFSET" ) == 0 )
        {
            // parse the value of offset or site
            token = strtok(nullptr, separater);
            x = token ? static_cast<float>(atof(token)) : 0.0f;
            token = strtok(nullptr, separater);
            y = token ? static_cast<float>(atof(token)) : 0.0f;
            token = strtok(nullptr, separater);
            z = token ? static_cast<float>(atof(token)) : 0.0f;

            // set site info
            if ( is_site )
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
        if ( strcmp( token, "CHANNELS" ) == 0 )
        {
            // get channel name
            token = strtok( nullptr, separater );
            // get channel count
            unsigned int channel_count = static_cast<unsigned int>(token ? atoi(token) : 0);

            // set channel name
            for ( i=0; i<channel_count; i++ )
            {
                token = strtok( nullptr, separater );
                if ( strcmp( token, "Xrotation" ) == 0 )
                    joint->channels.push_back(X_ROTATION);
                else if ( strcmp( token, "Yrotation" ) == 0 )
                    joint->channels.push_back(Y_ROTATION);
                else if ( strcmp( token, "Zrotation" ) == 0 )
                    joint->channels.push_back(Z_ROTATION);
                else if ( strcmp( token, "Xposition" ) == 0 )
                    joint->channels.push_back(X_POSITION);
                else if ( strcmp( token, "Yposition" ) == 0 )
                    joint->channels.push_back(Y_POSITION);
                else if ( strcmp( token, "Zposition" ) == 0 )
                    joint->channels.push_back(Z_POSITION);
                channel_num ++;
            }
        }
        // stop
        if ( strcmp( token, "MOTION" ) == 0 )
            break;
    }
    // start parse motions
    // get fram counts
    file.getline( line, BUFFER_LENGTH );
    token = strtok( line, separater );
    if ( strcmp( token, "Frames" ) != 0 ) {
        file.close();
        return;
    }
    token = strtok( nullptr, separater );
    if ( token == nullptr ) {
        file.close();
        return;
    }
    unsigned int num_frame = static_cast<unsigned int>(atoi(token));

    file.getline( line, BUFFER_LENGTH );
    token = strtok( line, ":" );
    // get frame duration
    if ( strcmp( token, "Frame Time" ) != 0 ) {
        file.close();
        return;
    }
    token = strtok( nullptr, separater );
    if ( token == nullptr ) {
        file.close();
        return;
    }
    frame_duration = atof( token );
    // parse all motions
    motion_datas = new vector<vector<float> *>(num_frame);
    for (i = 0; i < num_frame; i++) {
        vector<float> *motion = new vector<float>(channel_num);
        file.getline(line, BUFFER_LENGTH);
        token = strtok(line, separater);
        for (float &value : *motion) {
            if ( token == nullptr ){
                file.close();
                return;
            }
            value = static_cast<float>(atof(token));
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
Eigen::Vector4f BVHModel::jointPositionAt(unsigned int frame_ID, const std::string joint_name, float scale)
{
    BVHJoint *joint = joint_map[joint_name];
    vector<float> *motiondata = motionDataAt(frame_ID);
    Eigen::Vector4f coords(joint->offset[0], joint->offset[1], joint->offset[2], 1);
    Eigen::Matrix4f transition = Eigen::Matrix4f::Identity();
    // do FK until joint
    while(joint != nullptr) {
        float ax = 0, ay = 0, az = 0;
        for (ChannelEnum c : joint->channels) {
            switch (c) {
                case X_ROTATION: ax = (*motiondata)[joint->channel_index_offset];break;
                case Y_ROTATION: ay = (*motiondata)[joint->channel_index_offset + 1];break;
                case Z_ROTATION: az = (*motiondata)[joint->channel_index_offset + 2];break;
            default:break;
            }
        }
        transition = transition * getRotationMatrix(Z_ROTATION, az) *
                getRotationMatrix(Y_ROTATION, ay) *
                getRotationMatrix(X_ROTATION, ax);
        if (joint->parent != nullptr) {
            transition = transition *getTraslationMatrix(joint->offset[0] * scale, joint->offset[1] * scale, joint->offset[2] * scale);
        } else {
            transition = transition *getTraslationMatrix((*motiondata)[0] * scale, (*motiondata)[1] * scale, (*motiondata)[2] * scale);
        }
    }
    return Eigen::Vector4f(0, 0, 0, 1);
}

// render skeleton
void BVHModel::renderModelWith(boneRenderHandler &boneRender, jointRenderHandler &jointRender, unsigned int frame_ID, float scale)
{
    if (skeleton != nullptr && isValid() && channel_num > 0) {
        std::vector<float> *motion = motionDataAt(frame_ID);
        vector<float>::iterator it = motion->begin();
        renderJoint(skeleton, boneRender, jointRender, Eigen::Matrix4f::Identity(), it, scale);
    }
}

// render all joints
void BVHModel::renderJoint(BVHJoint *joint,
                           boneRenderHandler &boneRender,
                           jointRenderHandler &jointRender,
                           Eigen::MatrixXf current_transition,
                           std::vector<float>::iterator &it,
                           float scale)
{
    // do FK
    // rotation along X, Y, Z
    float x = 0, y = 0, z = 0, ax = 0, ay = 0, az = 0;
    for (ChannelEnum c : joint->channels) {
        switch (c) {
            case X_POSITION: x = *(it) * scale;break;
            case Y_POSITION: y = *(it) * scale;break;
            case Z_POSITION: z = *(it) * scale;break;
            case X_ROTATION: ax = *(it);break;
            case Y_ROTATION: ay = *(it);break;
            case Z_ROTATION: az = *(it);break;
        }
        it ++;
    }
    if (joint->parent != nullptr) {
        x = joint->offset[0] * scale;
        y = joint->offset[1] * scale;
        z = joint->offset[2] * scale;
    }
    Eigen::Matrix4f transition = current_transition *
                                 getTraslationMatrix(x, y, z) *
                                 getRotationMatrix(Z_ROTATION, az) *
                                 getRotationMatrix(Y_ROTATION, ay) *
                                 getRotationMatrix(X_ROTATION, ax);
    Eigen::Vector4f joint_center(0, 0, 0, 1);
    joint_center = transition * joint_center;
    // render joint
    jointRender(joint_center, joint->RGBAColor, .2f * scale);
    unsigned int size = joint->children.size();
    if (joint->children.size() == 0) {
        // render site bone;
        Eigen::Vector4f site_end(joint->site[0] * scale, joint->site[1] * scale, joint->site[2] * scale, 1);
        site_end = transition * site_end;
        boneRender(joint_center, site_end, CYAN_COLOR, .1f * scale);
    } else if ((joint->children.size() == 1)) {
        BVHJoint *child = joint->children[0];
        Eigen::Vector4f child_center(child->offset[0] * scale, child->offset[1] * scale, child->offset[2] * scale, 1);
        child_center = transition * child_center;
        boneRender(joint_center, child_center, CYAN_COLOR, .1f * scale);
        renderJoint(child, boneRender, jointRender, transition, it, scale);
    } else {
        // render bone
        Eigen::Vector4f child_center(0, 0, 0, 1);
        for (unsigned int i = 0; i < size; i ++) {
            BVHJoint *child = joint->children[i];
            child_center += Eigen::Vector4f(child->offset[0] * scale, child->offset[1] * scale, child->offset[2] * scale, 1);
        }
        // get center of all joints
        child_center = child_center / (size + 1);
        child_center = transition * child_center;
        boneRender(joint_center, child_center, CYAN_COLOR, .1f * scale);
        for (BVHJoint *child : joint->children) {
            Eigen::Vector4f child_joint_center(child->offset[0] * scale, child->offset[1] * scale, child->offset[2] * scale, 1);
            child_joint_center = transition * child_joint_center;
            boneRender(child_center, child_joint_center, CYAN_COLOR, .1f * scale);
            renderJoint(child, boneRender, jointRender, transition, it, scale);
        }
    }
}

Eigen::Matrix4f BVHModel::getRotationMatrix(ChannelEnum type, float alpha) {
    alpha = alpha / 180 * 3.1415926535f;
    Eigen::Matrix4f rotation;
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

Eigen::Matrix4f BVHModel::getTraslationMatrix(float x, float y, float z) {
    Eigen::Matrix4f t;
    t << 1, 0, 0, x,
         0, 1, 0, y,
         0, 0, 1, z,
         0, 0, 0, 1;
    return t;
}

// write to file
void BVHModel::writeToFile(const std::string file_name)
{
    cout << "save file at:" << file_name << endl;
}

std::vector<float> *BVHModel::motionDataAt(unsigned int frame_ID)
{
    if (motion_datas != nullptr && motion_datas->size() > frame_ID)
        return (*motion_datas)[frame_ID];
    if (defaultMotion == nullptr)
        defaultMotion = new vector<float>(channel_num, 0.f);
    return defaultMotion;
}


// dealloc
BVHModel::~BVHModel()
{

}

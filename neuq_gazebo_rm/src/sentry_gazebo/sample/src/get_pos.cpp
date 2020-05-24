/*
功能：获取gazebo里‘mrobot’模型的位置
话题名称：/gazebo/model_states
话题类型：gazebo_msgs/ModelStates
------------------------------------
*/
#include <ros/ros.h>
#include <gazebo_msgs/ModelStates.h>
#include <geometry_msgs/Pose.h>
#include <geometry_msgs/Point.h>
#include <stdio.h>
#include <iostream>
using namespace std;
//Callback 函数定义如下，根据模型的名称，可得对应模型的状态：
void modelStatesCallback(const gazebo_msgs::ModelStatesConstPtr &msg)
{
    int modelCount = msg->name.size();

    for(int modelInd = 0; modelInd < modelCount; ++modelInd)
    { 
        if(msg->name[modelInd] == "mrobot")//如果要换成其他的模型只要替换这里的mrobot即可
        {
             geometry_msgs::Pose pose = msg->pose[modelInd];
             
             
             double my_x = pose.position.x;
             double my_y = pose.position.y;
             double my_z = pose.position.z;
             cout<<"heard my position: "<<endl;
             cout<<my_x<<" , "<<my_y<<" , "<<my_z<<endl;
            break;
        }
    }
}

int main(int argc, char **argv)
{
  ros::init(argc, argv, "pose_listener");
  ros::NodeHandle nh;
  ros::Subscriber model_sub = nh.subscribe("/gazebo/model_states", 1, modelStatesCallback);
  ros::spin();

  return 0;
}

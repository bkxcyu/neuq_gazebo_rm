#include <ros/ros.h>
#include <gazebo_msgs/ModelStates.h>
#include <geometry_msgs/Pose.h>
#include <geometry_msgs/Point.h>
#include <stdio.h>
#include <iostream>
using namespace std;
//Callback 函数定义如下，根据模型的名称，可得对应模型的状态：
double my_x=0,my_y=0,my_z=0;
void modelStatesCallback(const gazebo_msgs::ModelStatesConstPtr &msg)
{
    int modelCount = msg->name.size();

    for(int modelInd = 0; modelInd < modelCount; ++modelInd)
    { 
        if(msg->name[modelInd] == "/swivel")
        {
             geometry_msgs::Pose pose = msg->pose[modelInd];
             
             
            my_x = pose.position.x;
             my_y = pose.position.y;
             my_z = pose.position.z;
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
ros::Rate loop_rate(20);
  ros::Subscriber model_sub = nh.subscribe("/gazebo/model_states", 1, modelStatesCallback);
ros::Publisher popub = nh.advertise<geometry_msgs::Point>("getpoint",100);
geometry_msgs::Point point;
    while(ros::ok())
	{
		  point.x = my_x;
		  point.y = my_y;
		  point.z = my_z;
		popub.publish(point);
		ros::spinOnce();
		loop_rate.sleep();
	}
return 0;

}

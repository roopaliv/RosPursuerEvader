
#include "ros/ros.h"
#include <geometry_msgs/Twist.h>
#include <sensor_msgs/LaserScan.h>
#include <tf/transform_broadcaster.h>
#include <tf2_msgs/TFMessage.h>
#include <nav_msgs/Odometry.h>

bool obstacle = false;
float ang_vel = 0.0;
float lin_vel = 0.0;
float x = 0.0;
float y = 0.0;
float z = 0.0;
bool locationReceived = false;

void detectedCallback(const sensor_msgs::LaserScan::ConstPtr& msg)
{
	for (int i = 0; i<360; i++)
	{
		if(msg->ranges[i] < 1.5){
		obstacle = true;
			}
	}
}
void odomCallback(const nav_msgs::Odometry::ConstPtr& msg)
{
	x = 	msg->pose.pose.position.x;
	y = 	msg->pose.pose.position.y;
	z =	msg->pose.pose.position.z;
 	locationReceived = true;
}


int main(int argc, char **argv)
{
  ros::init(argc, argv, "evader");
  ros::NodeHandle nh;
  tf::TransformBroadcaster br;
  tf::Transform transform;
  ros::Publisher vel_pub = nh.advertise<geometry_msgs::Twist>("cmd_vel", 2);
  ros::Subscriber laser_sub = nh.subscribe("/robot_0/base_scan", 10, detectedCallback);
  ros::Subscriber odom_sub = nh.subscribe("/robot_0/odom", 10, &odomCallback);
  ros::Rate loop_rate(10.0);
  lin_vel = 2.0;
  ang_vel = 0.0;  //go left for +ve 0.75// -0.75 for right
  while (ros::ok())
  {
	
	const float ArrayVel[4] = {-2.75, +2.75, +3.00,+2.50};
 	int rNo = rand()% 4;
	if(obstacle){
		ang_vel = ArrayVel[rNo];
		lin_vel = 0.0;
		obstacle = false;
        }
	else{
                ang_vel = 0.0;
		lin_vel = 2.0;
	}
	if(locationReceived){
	
	transform.setOrigin( tf::Vector3(x, y, z) );
	//ROS_ERROR("sent x is %f and sent y is %f",transform.getOrigin().x(), transform.getOrigin().y());
    	br.sendTransform(tf::StampedTransform(transform, ros::Time::now(), "/world", "/robot_0/odom"));
	locationReceived = false;
	}
    geometry_msgs::Twist vel;    
    vel.angular.z = ang_vel;
    vel.linear.x = lin_vel;
    vel_pub.publish(vel);
    
    ros::spinOnce();
    loop_rate.sleep();
  }
  return 0;
}


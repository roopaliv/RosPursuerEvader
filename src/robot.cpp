
#include "ros/ros.h"
#include <geometry_msgs/Twist.h>
#include "sensor_msgs/LaserScan.h"
#include <sstream>

bool obstacle = false;
float ang_vel = 0.0;
float lin_vel = 0.0;
void detectedCallback(const sensor_msgs::LaserScan::ConstPtr& msg)
{
	for (int i = 0; i<360; i++)
	{
          //ROS_INFO( "%f",msg->ranges[i]);
		if(msg->ranges[i] < 1.5){
		obstacle = true;
			}
	}
}

int main(int argc, char **argv)
{
  ros::init(argc, argv, "robot");
  ros::NodeHandle n;
  ros::Publisher vel_pub = n.advertise<geometry_msgs::Twist>("cmd_vel", 1000);
  ros::Subscriber sub = n.subscribe("base_scan", 1000, detectedCallback);
  ros::Rate loop_rate(10);
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


    geometry_msgs::Twist vel;    
    vel.angular.z = ang_vel;
    vel.linear.x = lin_vel;
    vel_pub.publish(vel);
    ros::spinOnce();

    loop_rate.sleep();
  }


  return 0;
}


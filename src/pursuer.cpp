#include <ros/ros.h>
#include <tf/transform_listener.h>
#include <geometry_msgs/Twist.h>
#include <nav_msgs/Odometry.h>
#include <tf/transform_broadcaster.h>
#include <sensor_msgs/LaserScan.h>

bool obstacleP = false;
float ang_velP = 0.0;
float lin_velP = 1.0;
float xp = 0.0;
float yp = 0.0;
float zp = 0.0;
bool locationReceivedP = false;

void detectedPCallback(const sensor_msgs::LaserScan::ConstPtr& msg)
{
	for (int i = 0; i<360; i++)
	{
		if(msg->ranges[i] < 2.5){
		obstacleP = true;
		}
	}
}
void odomPCallback(const nav_msgs::Odometry::ConstPtr& msg)
{
	
	xp = 	msg->pose.pose.position.x;
	yp = 	msg->pose.pose.position.y;
	zp =	msg->pose.pose.position.z;
 	locationReceivedP = true;
}


int main(int argc, char **argv)
{
  ros::init(argc, argv, "pursuer");
  ros::NodeHandle nhp;
  tf::TransformBroadcaster brP;
  tf::Transform transformP ;
  ros::Publisher velP_pub = nhp.advertise<geometry_msgs::Twist>("/robot_1/cmd_vel", 2);
  ros::Subscriber laserP_sub = nhp.subscribe("/robot_1/base_scan", 10, detectedPCallback); 
  ros::Subscriber odomP_sub = nhp.subscribe("/robot_1/odom", 10, &odomPCallback);
  tf::TransformListener listener;
  ros::Rate rate(10.0);
  ros::Time now = ros::Time::now();
  ros::Time past = ros::Time(0);
  bool transformSuccess = false;
  try{
     // past = now - ros::Duration(1.0);
  }
    catch (std::runtime_error &ex) {
      //ROS_ERROR("%s",ex.what());
      //ros::Duration(1.0).sleep();
    }
 
  //listener.waitForTransform("/robot_1/odom", "/robot_0/odom", past, ros::Duration(1.0));
 
  while (nhp.ok()){
    
    geometry_msgs::Twist velP;
    	
  
    tf::StampedTransform transform;
    try{
      
      listener.lookupTransform("/world", "/robot_0/odom", past, transform);
      transformSuccess = true;
    }
    catch (tf::TransformException &ex) {
      transformSuccess = false;
    }	

	if(locationReceivedP){
	transformP.setOrigin( tf::Vector3(xp, yp, zp) );
    	brP.sendTransform(tf::StampedTransform(transformP, ros::Time::now(), "/world", "/robot_1/odom"));
	locationReceivedP = false;
	}
	float xe;
	float ye;

 	
	if(transformSuccess ){
	     //ROS_ERROR("recd x is %f and recd y is %f",transform.getOrigin().x(), transform.getOrigin().y());
	     xe = transform.getOrigin().x();
	     ye = transform.getOrigin().y();
	     ang_velP = 0.5 * atan2((yp-ye),  (xp- xe));
	     lin_velP = 1.0 * sqrt(pow((xp-xe), 2) +pow((yp-ye), 2));
 	     transformSuccess = false;
	if(obstacleP){
		ang_velP = +1.75;
		lin_velP = 0.0;
		obstacleP = false;
        }
	}

	else{
                ang_velP = 0.0;
		lin_velP = 1.0;
	}
    velP.angular.z = ang_velP;
    velP.linear.x = lin_velP;
    //ROS_ERROR("ang vel is %f and lin vel is %f",ang_velP, lin_velP);
    velP_pub.publish(velP);
   
    ros::spinOnce();
    rate.sleep();
  }
  return 0;
};


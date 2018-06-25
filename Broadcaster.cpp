

#include <ros/ros.h>
#include <tf/transform_broadcaster.h>
#include <turtlesim/Pose.h>
#include <tf/tf.h>
#include <geometry_msgs/Twist.h>
#include <geometry_msgs/Pose.h>
#include <nav_msgs/Odometry.h>
#define PI 3.14159265
std::string turtle_name;

  tf::Transform transform;
  tf::Quaternion q;
  
void poseMessageReceived(const nav_msgs::Odometry& msg) 
{
	std::cout << std::setprecision(2) << std::fixed << /* output the pose information using standard output */
	  "Current position=(" << msg.pose.pose.position.x << ", " << msg.pose.pose.position.y << ") " << 
	  "Current direction=" << std::setprecision(2) << std::fixed << (atan(msg.pose.pose.orientation.z/msg.pose.pose.orientation.w)*180/PI)<<"\r";
	std::flush(std::cout);
} 
 
void poseCallback(const nav_msgs::Odometry::ConstPtr& msg){
  static tf::TransformBroadcaster br;

  transform.setOrigin( tf::Vector3(msg->pose.pose.position.x, msg->pose.pose.position.y, 0.0) );
  
  q.setRPY(0, 0, tf::getYaw(msg->pose.pose.orientation));
  transform.setRotation(q);
  
  br.sendTransform(tf::StampedTransform(transform, ros::Time::now(), "odom", "base_link"));
}

int main(int argc, char** argv){
  ros::init(argc, argv, "my_tf_broadcaster");
  transform.setOrigin( tf::Vector3(0.0, 0.0, 0.0) );
  q.setRPY(0, 0, 0);
  ros::NodeHandle node;
  ros::Subscriber sub = node.subscribe("RosAria/pose", 1000, &poseCallback);
  ros::Subscriber pose = node.subscribe("RosAria/pose", 1000, &poseMessageReceived);
  ros::spin();
  return 0;
};



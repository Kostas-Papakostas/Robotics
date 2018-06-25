#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include <tf/transform_listener.h>
#include <tf/transform_broadcaster.h>
#include <math.h>       /* sin */
#include <stdlib.h>     /* div, div_t */
#include <stdio.h>
#include <signal.h>
#include <std_msgs/Float32.h>
#include <std_msgs/Float64.h>
#include <std_msgs/Int8.h>
#include <std_msgs/Bool.h>
#include <nav_msgs/Odometry.h>
#include <iomanip>
#include <geometry_msgs/Pose2D.h>

#define PI 3.14159265
volatile sig_atomic_t flag = 0;
using namespace std;


int sgn(double v) {
  return (v < 0) ? -1 : ((v > 0) ? 1 : 0);
}

ros::Publisher pub;
void my_function(int sig){ // can be called asynchronously
  ros::shutdown();
  exit(1);
}


int main(int argc, char **argv)
{
  ros::init(argc, argv, "KaneiTroxia", ros::init_options::NoSigintHandler);
  ros::NodeHandle nh;
  
  pub = nh.advertise<geometry_msgs::Twist>("RosAria/cmd_vel", 1000);
  geometry_msgs::Twist msg;
  nav_msgs::Odometry Odmsg;
  double BASE_SPEED = PI/4, MOVE_TIME = 100, CLOCK_SPEED = 10;
  double count = 0.5;
  tf::TransformListener listener;
  ros::Rate rate(CLOCK_SPEED);
  signal(SIGINT, my_function); 

  
  // Make the robot stop (robot perhaps has a speed already)
  msg.linear.x = 0;
  msg.linear.y = 0;
  msg.linear.z = 0;
  msg.angular.x = 0;
  msg.angular.y = 0;
  msg.angular.z = 0;
  pub.publish(msg);
  double previousDistance=10000;
  double destX=6, destY=8, destinationDist, distanceFromOrigin;
  double desX=destX, desY=destY;
  double dest[5][2] = {{6,0},
						{6,6},
						{9,6},
						{9,0},
						{9,0}};//Edw ginetai i eisagwgi twn simeiwn
  int point=0;
  int c=0;
  double previousRotation=0;
  int largeRot=0;
  double previousAngle=0;
  while(ros::ok())
    {
	  tf::StampedTransform transform;
      try{
		listener.lookupTransform("odom","base_link", ros::Time(0), transform);
	  }
	  catch(tf::TransformException &ex){
		ROS_ERROR("%s", ex.what());
		ros::Duration(1.0).sleep();
		continue;
	  }
	  
	  destinationDist=sqrt(pow(dest[point][0],2)+pow(dest[point][1],2));
	  double rotate;
	  double roll, pitch, yaw;
	  bool end;
	  
	double tempDistance;
	div_t divresult;
	divresult = div (count,8);
	pub.publish(msg);
	
	distanceFromOrigin=sqrt(pow(dest[point][0]-transform.getOrigin().x(),2)+pow(dest[point][1]-transform.getOrigin().y(),2));

	
	tf::Quaternion quart(Odmsg.pose.pose.orientation.x, Odmsg.pose.pose.orientation.y, Odmsg.pose.pose.orientation.z, Odmsg.pose.pose.orientation.w);
	quart = transform.getRotation();
	tf::Matrix3x3 m(quart);
	
	m.getRPY(roll, pitch, yaw);
	
	rotate=(atan2((dest[point][1]-transform.getOrigin().y()),(dest[point][0]-transform.getOrigin().x()))); //prepei na vriskw apo ekei p eimai
	msg.linear.x = distanceFromOrigin/4;
	
	
	
	printf("---------------------\n");
	printf("0<=%f<=0.3   \n", distanceFromOrigin);
	printf("point=  %d   \n", point);
	printf("desired position: x=%f   y=%f\n",dest[point][0], dest[point][1]);
    printf("x=%f    y=%f\n",transform.getOrigin().x(), transform.getOrigin().y());
      
      bool turn=false;
      
      
      if(((transform.getOrigin().x()>=dest[point][0]-0.1 && transform.getOrigin().x()<=dest[point][0]+0.1) 
      && (transform.getOrigin().y()>=dest[point][1]-0.1 && transform.getOrigin().y()<=dest[point][1]+0.1))){
		  ROS_INFO_STREAM("MOVING FORWARD");
		  msg.linear.x=0;
		  msg.angular.z=0;
		  point++;
		  previousRotation=rotate;
		  previousAngle=rotate;
		  if(point>4){//Edw ginetai o elegxos gia to an perase apo ola ta simeia
			break;
		  }
	  }
	  printf("desired rotation    %f\n", (rotate)*180/PI);
		previousRotation=0; 
	  if(rotate<0){
		if((yaw)*180/PI<=(((rotate)*180/PI)-1)){
		  ROS_INFO_STREAM("ROTATING");
		  msg.angular.z=-(rotate+yaw)/2;
		  printf("yaw=%f   \n",(yaw)*180/PI);
		}
		else if((yaw)*180/PI>=(((rotate)*180/PI)-1)){
		  ROS_INFO_STREAM("ROTATING BACK");
		  msg.angular.z=(rotate+yaw)/2;
		  printf("yaw=%f   \n",(yaw)*180/PI);
		}
	  }else{
		if((yaw)*180/PI<=(((rotate)*180/PI)-1)){
		  ROS_INFO_STREAM("ROTATING");
		  msg.angular.z=(rotate-yaw)/2;
		  printf("yaw=%f   \n",fabs(yaw)*180/PI);
		}
		else if((yaw)*180/PI>=(((rotate)*180/PI)-1)){
		  ROS_INFO_STREAM("ROTATING BACK");
		  msg.angular.z=-(rotate-yaw)/2;
		  printf("yaw=%f   \n",fabs(yaw)*180/PI);
		}
	  }
      rate.sleep();
   }
  
  // make the robot stop
  for (int i = 0; i < 2; i++)
    {  

      msg.linear.x = 0;
      msg.linear.y = 0;
      msg.linear.z = 0;
      pub.publish(msg);

    }
  
    // Guard, make sure the robot stops.
    rate.sleep();
    msg.linear.x = 0;
    msg.linear.y = 0;
    msg.linear.z = 0;
    pub.publish(msg); 

}

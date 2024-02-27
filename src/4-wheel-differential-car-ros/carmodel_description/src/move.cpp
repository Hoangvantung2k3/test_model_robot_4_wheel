#include "ros/ros.h"  
#include "geometry_msgs/Twist.h"  
 
  
int main(int argc, char **argv)  
{  
    ros::init(argc,argv,"move");  
    ros::NodeHandle n;  
    // Need to publish a /cmd_vel topic according to the actual situation - thực hiện lệnh /cmd_vel dựa trên tình huống thực tế 
    ros::Publisher vel_pub = n.advertise<geometry_msgs::Twist>("/cmd_vel",1);  
  
    unsigned char rate;  
    rate = 50;  
    ros::Rate loop_rate(rate);  
    geometry_msgs::Twist msg;  
  
    // continue running, x has value, the othẻr is 0
    while (ros::ok())  
    {                   
        msg.linear.x = 0.2; 
        msg.linear.y = 0.0;
        msg.linear.z = 0.0;
        msg.angular.x = 0.0;
        msg.angular.y = 0.0;
        msg.angular.z = 0.0;
        
        vel_pub.publish(msg);  
        ros::spinOnce();  
        loop_rate.sleep();
        ROS_INFO("%s", "running the robot ...");          
    }     
    return 0;  
}  

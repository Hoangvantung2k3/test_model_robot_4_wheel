// để tạm ở đây để xem lại thông số tại sao lại tự động chạy 
// bộ PID hiện tại đang tương thích nhất đối với phần làm thực tế của Giang, cũng dùng chính từ thông số Kp
#include "pid_control_test.hpp"
#include <ros/ros.h>


using namespace botcontrol;

BotControl::BotControl(ros::NodeHandle& nh) : nodehandle_(nh), last_time_(ros::Time::now()){

    //load the param
    if(!loadParam()){
        ROS_ERROR("Error in loading the parameters.");
    }

    // declare all the subscriber and publisher
    odom_sub_ = nodehandle_.subscribe("/odom", 1, &BotControl::odomCallBack, this);

    vel_pub_ = nodehandle_.advertise<geometry_msgs::Twist>("/cmd_vel", 200);
    error_forward_pub_ = nodehandle_.advertise<std_msgs::Float32>("/error_forward", 1); 
    error_angle_pub_ = nodehandle_.advertise<std_msgs::Float32>("/error_angle", 1);
    control_signal_forward_pub_ = nodehandle_.advertise<std_msgs::Float32>("/control_signal_forward", 1);
    control_signal_angle_pub_ = nodehandle_.advertise<std_msgs::Float32>("/control_signal_angle", 1);

    //initialize variables
    error_forward_ = 0;
    error_forward_prev_ = 0;
    error_angle_ = 0;
    error_angle_prev_ = 0;
    P_forward_ = 0;
    P_angle_ = 0;
    I_forward_ = 0;
    I_angle_ = 0;
    D_forward_ = 0;
    D_angle_ = 0;

    ROS_INFO("Node Initialized");
}

BotControl::~BotControl(){}

void BotControl::odomCallBack(const nav_msgs::OdometryConstPtr& odomMsg){
    pos_x_ = odomMsg->pose.pose.position.x;
    pos_y_ = odomMsg->pose.pose.position.y;
    q_z_ = odomMsg->pose.pose.orientation.z;
    ang_z_ = q_z_ * 2.19;

    // Calculate target_distance and target_angle based on odom data
    // Assuming target position (target_pos_x_, target_pos_y_) is given
    float target_pos_x_ = 5.0;  // Example target position x
    float target_pos_y_ = 5.0;  // Example target position y

    float dx = target_pos_x_ - pos_x_;
    float dy = target_pos_y_ - pos_y_;
    target_distance = sqrt(dx*dx + dy*dy);
    target_angle = atan2(dy, dx);

    pidAlgorithm();
}

void BotControl::pidAlgorithm(){

    std_msgs::Float32 linear_error;
    std_msgs::Float32 angle_error;
    std_msgs::Float32 linear_velocity;
    std_msgs::Float32 angle_velocity;

    // Update the time and calculate dt
    ros::Time current_time = ros::Time::now();
    dt = (current_time - last_time_).toSec();
    last_time_ = current_time;

    // Update the PID-related error states
    error_forward_ = target_distance;
    error_angle_ = target_angle - ang_z_;

    // Normalise the error_angle_ within [-PI, PI]
    error_angle_ = normalizeAngle(error_angle_);

    // Define proportional term
    P_angle_ = Kp_a * error_angle_;
    P_forward_ = Kp_f * error_forward_;

    // Compute PID
    trans_forward_ = P_forward_ + 1.0;  // Thêm hằng số tương đối
    trans_angle_ = -(P_angle_ + 1.0);   // Thêm hằng số tương đối

    error_forward_prev_ = error_forward_;
    error_angle_prev_ = error_angle_;

    // Set threshold (optional)
    trans_forward_ = std::max(0.0, std::min(trans_forward_, 7.0));

    // Publish all
    vel_cmd_.linear.x = trans_forward_;
    vel_cmd_.angular.z = trans_angle_; 
    vel_pub_.publish(vel_cmd_);

    linear_error.data = error_forward_;
    error_forward_pub_.publish(linear_error);

    linear_velocity.data = trans_forward_;
    control_signal_angle_pub_.publish(linear_velocity);

    angle_error.data = error_angle_;
    error_angle_pub_.publish(angle_error);

    angle_velocity.data = trans_angle_;
    control_signal_angle_pub_.publish(angle_velocity);
}

void BotControl::spin(){
    ros::Rate loop_rate(1/dt);
    while(ros::ok()){
        ros::spinOnce();
        loop_rate.sleep();
    }
}

bool BotControl::loadParam(){
    if(!nodehandle_.getParam("/pid_control_node/Kp_f", Kp_f)){
        ROS_ERROR("Kp_f Load Error");
        return false;
    }
    if(!nodehandle_.getParam("/pid_control_node/Ki_f", Ki_f)){
        ROS_ERROR("Ki_f Load Error");
        return false;
    }
    if(!nodehandle_.getParam("/pid_control_node/Kd_f", Kd_f)){
        ROS_ERROR("Kd_f Load Error");
        return false;
    }
    if(!nodehandle_.getParam("/pid_control_node/Kp_a", Kp_a)){
        ROS_ERROR("Kp_a Load Error");
        return false;
    }
    if(!nodehandle_.getParam("/pid_control_node/Ki_a", Ki_a)){
        ROS_ERROR("Ki_a Load Error");
        return false;
    }
    if(!nodehandle_.getParam("/pid_control_node/Kd_a", Kd_a)){
        ROS_ERROR("Kd_a Load Error");
        return false;
    }
    if(!nodehandle_.getParam("/pid_control_node/target_distance", target_distance)){
        ROS_ERROR("target_distance Load Error");
        return false;
    }
    if(!nodehandle_.getParam("/pid_control_node/target_angle", target_angle)){
        ROS_ERROR("target_angle Load Error");
        return false;
    }
    if(!nodehandle_.getParam("/pid_control_node/dt", dt)){
        ROS_ERROR("dt Load Error");
        return false;
    }

    return true;
}

double BotControl::normalizeAngle(double angle){
    if (angle > PI){
        angle -= 2*PI;
        return normalizeAngle(angle);
    }
    else if(angle < -PI){
        angle += 2*PI;
        return normalizeAngle(angle);
    }
    return angle;
}

int main(int argc, char** argv){
    ros::init(argc, argv, "pid_control_node");
    ros::NodeHandle nodeHandle("~");

    BotControl BC(nodeHandle);

    BC.spin();
}

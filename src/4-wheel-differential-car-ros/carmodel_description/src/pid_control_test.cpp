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
	scan_sub_ = nodehandle_.subscribe("/m2wr/laser/scan", 1, &BotControl::scanCallBack, this);
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


    // Ensure initial velocities are zero
    trans_forward_ = 0;
    trans_angle_ = 0;

    ROS_INFO("Node Initialized");

    std::cout << "Enter target X coordinate: ";
    std::cin >> target_x_;
    std::cout << "Enter target Y coordinate: ";
    std::cin >> target_y_;
}

BotControl::~BotControl(){}
void BotControl::scanCallBack(const sensor_msgs::LaserScan::ConstPtr& scanMsg){
	scan_data_ = scanMsg->ranges;
	int arr_size = scan_data_.size();
	float smallest_dist = 100;

	for(int i = 0; i<arr_size; i++){
		if(scan_data_[i] < smallest_dist) {
			smallest_dist = scan_data_[i];
			scan_ang_ = scanMsg->angle_min + scanMsg->angle_increment*i;
		}
	}
	scan_range_ = smallest_dist;

	pidAlgorithm();
}


void BotControl::odomCallBack(const nav_msgs::OdometryConstPtr& odomMsg){
    pos_x_ = odomMsg->pose.pose.position.x;
    pos_y_ = odomMsg->pose.pose.position.y;
    q_z_ = odomMsg->pose.pose.orientation.z;
    ang_z_ = q_z_ * 2.19;
    if (target_distance == 0 && target_angle == 0) {
        target_distance = sqrt(pos_x_ * pos_x_ + pos_y_ * pos_y_);
        target_angle = ang_z_;
    }
    // Calculate target distance and angle
    target_distance = sqrt(pow(target_x_ - pos_x_, 2) + pow(target_y_ - pos_y_, 2));
    target_angle = atan2(target_y_ - pos_y_, target_x_ - pos_x_);
    // Call PID algorithm on receiving odometry data
    pidAlgorithm();
}

void BotControl::pidAlgorithm(){
    ros::Time current_time = ros::Time::now();
    // dùng công thức tính dt thay cho dùng bộ số trong config.yaml
    dt = (current_time - last_time_).toSec();
    last_time_ = current_time;

    // PID computation
    error_forward_ = target_distance - sqrt(pos_x_ * pos_x_ + pos_y_ * pos_y_);
    error_angle_ = target_angle - ang_z_;
    error_angle_ = normalizeAngle(error_angle_);

    // Proportional term
    P_angle_ = Kp_a * error_angle_;
    P_forward_ = Kp_f * error_forward_;

    // Integral term
    I_angle_ += error_angle_ * dt;
    I_forward_ += error_forward_ * dt;

    // Derivative term
    D_angle_ = Kd_a * normalizeAngle(error_angle_ - error_angle_prev_) / dt;
    D_forward_ = Kd_f * (error_forward_ - error_forward_prev_) / dt;

    // Compute PID
    trans_forward_ = P_forward_ + Ki_f * I_forward_ + D_forward_;
    trans_angle_ = -(P_angle_ + Ki_a * I_angle_ + D_angle_);

    error_forward_prev_ = error_forward_;
    error_angle_prev_ = error_angle_;

    // Only publish if there is significant movement required
    if (abs(trans_forward_) > 0.01 || abs(trans_angle_) > 0.01) {
        vel_cmd_.linear.x = trans_forward_;
        vel_cmd_.angular.z = trans_angle_;
        vel_pub_.publish(vel_cmd_);
    }

    // Publish errors and control signals for debugging
    std_msgs::Float32 linear_error, angle_error, linear_velocity, angle_velocity;
    linear_error.data = error_forward_;
    angle_error.data = error_angle_;
    linear_velocity.data = trans_forward_;
    angle_velocity.data = trans_angle_;
    
    ROS_INFO("Forward Velocity: %f; Angle Velocity: %f; Orientation_error: %f, Distance: %f", 
		trans_forward_, trans_angle_, error_angle_, scan_range_);
    error_forward_pub_.publish(linear_error);
    control_signal_forward_pub_.publish(linear_velocity);
    error_angle_pub_.publish(angle_error);
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
    } else if (angle < -PI){
        angle += 2 * PI;
    }
    return angle;
}

int main(int argc, char** argv){
    ros::init(argc, argv, "pid_control_node");
    ros::NodeHandle nodeHandle("~");

    BotControl BC(nodeHandle);

    BC.spin();
    return 0;
}



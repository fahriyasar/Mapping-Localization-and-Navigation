#include "ros/ros.h"
#include "amr_navigation/gotogoal.h"
#include <string>
#include <iostream>
#include "nav_msgs/Odometry.h"
#include <cmath>
#include <tf/tf.h>
#include <tf/transform_datatypes.h>

using namespace std;

float goal_x,     goal_y,    goal_yaw;
float current_x , current_y, current_yaw, current_roll, current_pitch;
float orient_x,   orient_y, orient_z, orient_w;
float linear_x,  angular_z;
bool enter_door;
float error;
float kp_lin=0.3, kp_ang=0.6;
float linear_in , angular_in;


/*
tf::Quaternion q(orient_x, orient_y, orient_z, orient_w);
tf::Matrix3x3 m(q);
m.getRPY(current_roll, current_pitch, current_yaw);
*/

bool goal(amr_navigation::gotogoal::Request &msg, amr_navigation::gotogoal::Response &result){

    goal_x = msg.x;
    goal_y = msg.y;
    goal_yaw = msg.q;
    return true;
}

void get_odom(const nav_msgs::Odometry::ConstPtr& msg)
{
   current_x = msg->pose.pose.position.x;
   current_y = msg->pose.pose.position.y;
   
   orient_x = msg->pose.pose.orientation.x;
   orient_y = msg->pose.pose.orientation.y;
   orient_y = msg->pose.pose.orientation.z;
   orient_w = msg->pose.pose.orientation.w;
   
   linear_x  = msg->twist.twist.linear.x;
   angular_z = msg->twist.twist.angular.z; 
}

/*
float proportionalControl(float goal_posex,float goal_posey, float angle){
        float error_linear  = sqrt( pow((goal_posex - current_x),2) + pow((goal_posey-current_y),2) );
        float Ref_ang = atan2(goal_posey-current_y, goal_posex-current_x);
        float linear_input = kp_lin*error_linear;
        float angular_input = kp_ang*(Ref_ang - angle);
        return linear_input;
        }
*/
float proportionalControlLinear(float goal_posex,float goal_posey, float angle){
        float error_linear  = sqrt( pow((goal_posex - current_x),2) + pow((goal_posey-current_y),2) );
        float linear_input = kp_lin*error_linear;
        return linear_input;
        }

float proportionalControlAngular(float goal_posex,float goal_posey, float angle){
        float Ref_ang = atan2(goal_posey-current_y, goal_posex-current_x);
        float angular_input = kp_ang*(Ref_ang - angle);
        return angular_input;
        }

float proportionalControlError(float goal_posex,float goal_posey, float angle){
        float error_linear  = sqrt( pow((goal_posex - current_x),2) + pow((goal_posey-current_y),2) );
        return error_linear ;
        }

int main(int argc, char **argv)
{
    
    ros::init(argc,argv,"service_server");
    ros::NodeHandle nh;
    ros::ServiceServer service_server = nh.advertiseService("gotogoal",goal);
    ros::Subscriber sub = nh.subscribe("odom", 10, get_odom);
    ros::Publisher speedPub = nh.advertise<geometry_msgs::Twist>("cmd_vel",10); 
    
    

    geometry_msgs::Twist cmd_speed;
    ros::Rate loop_rate(100);
    ROS_INFO("Service Server Hazir!");
    
    
    float siny_cosp = 2 * (orient_w * orient_z + orient_x * orient_y);
    float cosy_cosp = 1 - 2 * (orient_y * orient_y + orient_z * orient_z);
    current_yaw = atan2(siny_cosp, cosy_cosp);

    while (not ros::isShuttingDown()){
    goal_x = 3;
    goal_y = 3;
    goal_yaw = 0;
    linear_in = proportionalControlLinear(goal_x,goal_y,goal_yaw); 
    angular_in = proportionalControlAngular(goal_x,goal_y,goal_yaw); 
    error = proportionalControlError(goal_x,goal_y,goal_yaw);
    
    cout << "1 :  " << linear_in << endl;
    cout << "2 :  " << angular_in << endl;
    cout << "3 :  " << error << endl;

    while (error >= 0.1)
    {
    float siny_cosp = 2 * (orient_w * orient_z + orient_x * orient_y);
    float cosy_cosp = 1 - 2 * (orient_y * orient_y + orient_z * orient_z);
    current_yaw = atan2(siny_cosp, cosy_cosp);
    
    linear_in = proportionalControlLinear(goal_x,goal_y,goal_yaw); 
    angular_in = proportionalControlAngular(goal_x,goal_y,goal_yaw); 
    error = proportionalControlError(goal_x,goal_y,goal_yaw);
    
      
    cmd_speed.linear.x = linear_in;
    cmd_speed.angular.z = angular_in;
    speedPub.publish(cmd_speed);
    loop_rate.sleep();
    }
    cmd_speed.linear.x = 0;
    cmd_speed.angular.z = 0;
    speedPub.publish(cmd_speed);
    loop_rate.sleep();
    loop_rate.sleep();
    }
    
    ros::spinOnce();
    return 0;
}
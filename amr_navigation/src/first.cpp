#include "ros/ros.h"
#include "geometry_msgs/Twist.h"
#include <iostream>
#include <math.h>

using namespace std;

int main(int argc, char **argv)
{
    double dt = 0;
    double  t = 0;
    
    ros::Time current_time, last_time;
    ros::init(argc, argv, "first");
    ros::NodeHandle nh;

    ros::Publisher speedPub = nh.advertise<geometry_msgs::Twist>("cmd_vel",10); 

    current_time = ros::Time::now();
    last_time = ros::Time::now();

    ros::Rate loop_rate(10);
    geometry_msgs::Twist msg;
    while (ros::ok())
    {
       current_time = ros::Time::now();
       double dt = (current_time - last_time).toSec();  // 100 milisaniye yani 10 Hz frekans doğrulamak için
       
       msg.linear.x = 0.2;
       msg.angular.z =sin(t/20)*0.2;

       t += 1; 

       speedPub.publish(msg);
       ros::spinOnce();
       loop_rate.sleep();
       last_time = current_time;

      // cout << dt << endl;
      // cout << t << endl;
    }    

    return 0;
}

#include "ros/ros.h"
#include "amr_navigation/gotogoal.h"
#include <cstdlib>

int main(int argc, char **argv)
{
    
    ros::init(argc,argv,"service_client");

    if(argc!=4){

        ROS_INFO("Kullanim: rosrun amr_navigation service_client arg1 arg2 arg3");
        ROS_INFO("arg1: float sayi, arg2: float sayi, arg3: float sayi");
        return 1;
    }

ros::NodeHandle nh;

ros::ServiceClient service_client = nh.serviceClient<amr_navigation::gotogoal>("gotogoal");

amr_navigation::gotogoal srv;

srv.request.x = atoi(argv[1]);
srv.request.y = atoi(argv[2]);
srv.request.q = atoi(argv[3]);

if(service_client.call(srv)){

    ROS_INFO("gonderilen degerler: x=%f ve y=%f q=%f",srv.request.x,srv.request.y,srv.request.q);
}
else{

    ROS_ERROR("HATA");
    return 1;
}

    return 0;
}
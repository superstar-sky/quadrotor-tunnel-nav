// uav_control.cpp
// 161130
// this is the control code for UAV in the simulated enviornment by gazebo.
// 

#include <ros/ros.h>
#include <signal.h>
#include <geometry_msgs/Twist.h>
#include <sensor_msgs/LaserScan.h>
#include "boost/thread/mutex.hpp"

#include "sub.hpp"


class UAV_Control
{
public:
  UAV_Control();
  void command();
  void updateScan(const sensor_msgs::LaserScan::ConstPtr& new_scan);
  void stop();

private:
  ros::Publisher vel_pub;
  ros::Subscriber scan_sub;
  ros::Timer timer;
  boost::mutex scan_mutex;
  sensor_msgs::LaserScan scan;
  bool f_up;
};


UAV_Control *p_control;
void scanCallback(const sensor_msgs::LaserScan::ConstPtr& scan)
{
  //boost::mutex::scoped_lock lock(scan_mutex);
  p_control->updateScan(scan);
  //printInTerm("scanned");
}
void quit(int sig)
{
  printInTerm("shutting down");
  p_control->stop();
  exit(0);
}



int main(int argc, char** argv)
{
  ros::init(argc, argv, "uav_control");

  UAV_Control uav_control;
  p_control = &uav_control;

  signal(SIGINT,quit);

  while(ros::ok())
  {
    ros::spinOnce();
  }

  return(0);
}

UAV_Control::UAV_Control():
  f_up(true)
{
  ros::NodeHandle n;
  vel_pub = n.advertise<geometry_msgs::Twist>("cmd_vel", 1);
  scan_sub = n.subscribe("/scan", 1, scanCallback);
  timer = n.createTimer(ros::Duration(3.0), boost::bind(&UAV_Control::command, this));
}

void UAV_Control::command()
{
  geometry_msgs::Twist vel;

  if(f_up)
  {
    vel.linear.z = 0.5;
    f_up = false;
    printInTerm("going up");
  }
  else
  {
    vel.linear.z = -0.5;
    f_up = true;
    printInTerm("going down");
  }

  vel_pub.publish(vel);    

}

void UAV_Control::stop()
{
  geometry_msgs::Twist vel;
  vel_pub.publish(vel);
  timer.stop();
}

void UAV_Control::updateScan(const sensor_msgs::LaserScan::ConstPtr& new_scan)
{
  //scan = new_scan;
}

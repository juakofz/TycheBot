#include "ros/ros.h"
#include "std_msgs/String.h"
#include "fiducial_msgs/FiducialTransform.h"
#include "fiducial_msgs/FiducialTransformArray.h"
#include "std_msgs/Int8.h"

ros::Publisher mensaje;
ros::Subscriber aruco;
int aruco_num=0;
fiducial_msgs::FiducialTransformArray array;
std_msgs::Int8 target;



void arucoCallback(const fiducial_msgs::FiducialTransformArray::ConstPtr& msg)
{
  array=*msg;
  if(array.transforms.size()>0)
  {
	ROS_INFO("%d",array.transforms[0].fiducial_id);
	target.data=array.transforms[0].fiducial_id;
  }
  /*if (msg->transforms.size()>0)
  {
	for (int i=0;msg->transforms.size();i++)
	{
		array.transforms[i] = msg->transforms[i];
		ROS_INFO("%d",array.transforms[i].fiducial_id);	
	}
  }*/
}


int main(int argc, char **argv)
{
 
  ros::init(argc, argv, "target_aruco");
  ros::NodeHandle n;
  

  /**
   * The subscribe() call is how you tell ROS that you want to receive messages
   * on a given topic.  This invokes a call to the ROS
   * master node, which keeps a registry of who is publishing and who
   * is subscribing.  Messages are passed to a callback function, here
   * called chatterCallback.  subscribe() returns a Subscriber object that you
   * must hold on to until you want to unsubscribe.  When all copies of the Subscriber
   * object go out of scope, this callback will automatically be unsubscribed from
   * this topic.
   *
   * The second parameter to the subscribe() function is the size of the message
   * queue.  If messages are arriving faster than they are being processed, this
   * is the number of messages that will be buffered up before beginning to throw
   * away the oldest ones.
   */
  aruco = n.subscribe("fiducial_transforms",1,arucoCallback);
  mensaje = n.advertise<std_msgs::Int8>("Target",1);
  target.data=0;
  


 
  
  while(ros::ok()){
	ros::spinOnce();
	//ROS_INFO("%d",target.data);	
	mensaje.publish(target);
	
  }
	
    

  return 0;
}

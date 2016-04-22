#include "ros/ros.h"
#include "std_msgs/String.h"
#include "/home/aishanou/Documents/beckonbotgesture/catkinGES/src/gesture_recog/include/GRT/GRT.h"

//Demonstartes simple receipt of message over the ROS system

//This is the call back function that gets called 
//when a message arrives on the chatter topic

using namespace GRT;


enum State { FREEMOVMENT, READY, INPOSITION, LISTINING };
State state;


void Callback(const std_msgs::String::ConstPtr& msg)
{
	ROS_INFO("I herd: [%s]", msg -> data.c_str());
}

int main(int argc, char **argv)
{

	//Initialize ROS
	// ros::init(argc, argv, "listener");

	// ros::NodeHandle gesture_recognizer;

	// ros::Subscriber sub_ = gesture_recognizer.subscribe("chatter", 1000, chatterCallback);
	// //ros::Subscriber sub_


	// //Create a new Gesture Recognition Pipeline
 //    GestureRecognitionPipeline pipeline;



	ros::spin();

	return 0;


}

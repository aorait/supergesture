#include "ros/ros.h"
#include "std_msgs/String.h"
#include <sstream>
#include <list>
#include "/home/aishanou/Documents/beckonbotgesture/catkinGES/src/gesture_recog/include/GRT/GRT.h"
#include "std_msgs/Float64.h"
#include <geometry_msgs/PoseStamped.h>
#include <cmath>
#include "std_msgs/MultiArrayLayout.h"
#include "std_msgs/MultiArrayDimension.h"
#include "std_msgs/Float64MultiArray.h"
//#include "/home/wes/Development/grt/GRT/GRT.h"

#define dimension 4
#define buffer_size 260
#define likelyhood_threshold 0.3

using namespace GRT;
//using namespace std;

class Livetest
{
	ros::NodeHandle nh;
	ros::Subscriber sub1;  //listen to topic "staticFeatureVector"
	TimeSeriesClassificationData testingData;
	ANBC anbc;
	int counter1=0;
	int counter2=0;
public:

	void callBack(const std_msgs::Float64MultiArray::ConstPtr& msg)
	{
		int state = 0; ///put here to make
		if(state==0){		
		counter1++;
		GRT::vector<double> features;
		features.clear();
		for(std::vector<double>::const_iterator it = msg->data.begin(); it != msg->data.end(); ++it)
		{
			features.push_back(*it);
		}

		anbc.predict(features);
		
		UINT predictedClassLabel = anbc.getPredictedClassLabel();
		if (1 == predictedClassLabel) counter2++; //since there is only one class a positive is if class 1 is predicted 
		//printf("Predicted Class Label: %d\n", predictedClassLabel);
		if (counter1 >= 50)
		{
			double likelihood = (double)counter2 / (double)counter1; // gives the fraction of time class 1 was predicted for 50 values ~ 0.5 seconds
			//printf("Likelihood: %f\n", likelihood); //currently printing values. Should be used to change state in future
			counter1 = 0;
			counter2 = 0;
			if(likelihood>=0.95){
				state=2; ///put here to make
			}				
		}
		}
	}

	//constructor
	Livetest()
	{
		sub1 = nh.subscribe("/staticFeatureVector", 100, &Livetest::callBack, this);
    		if( !anbc.load( "/home/radar/catkin_ws/src/beginner_tutorials/src/ANBCModel.grt" ) )  //provide full path for the ANBCModel.grt
		{
        		std::cout << "ERROR: Failed to load the ANBC model from a file!\n";
    		}
	}
};



int main(int argc, char **argv)
{
    	//subscribe to staticFeatureVector
	ros::init(argc, argv, "staticTest");
	Livetest obj;
	ros::spin();
    	return 0;
  
}

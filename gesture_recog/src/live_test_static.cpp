#include "ros/ros.h"
#include "std_msgs/String.h"
#include <sstream>
#include <list>
#include "GRT.h"
#include "std_msgs/Float64.h"
#include <geometry_msgs/PoseStamped.h>
#include <cmath>
#include "std_msgs/MultiArrayLayout.h"
#include "std_msgs/MultiArrayDimension.h"
#include "std_msgs/Float64MultiArray.h"

#define dimension 4
#define buffer_size 260
#define likelyhood_threshold 0.3

using namespace GRT;


class MyFrame
{
public:
	double vecData[dimension];

	MyFrame(double array[dimension])
	{
		for (int i=0; i<dimension; i++)  //the "normalization" is here
		{
			vecData[i] = (double)((int)(array[i]*500));
		}
	}
};

class Livetest
{
	ros::NodeHandle nh;
	ros::Publisher pub;  //publish to topic "finalLabel"
	ros::Subscriber sub1;  //listen to topic "featureVector"
	TimeSeriesClassificationData testingData;
	MatrixDouble testingSample;
	int receivedFrameNum;  //let's keep buffer size, sample at granularity of 5, start at receiving buffer size
	std::list<MyFrame> buffer;
	ANBC anbc;
	int counter1=0;
	int counter2=0;
public:

	void callBack(const std_msgs::Float64MultiArray::ConstPtr& msg)
	{
		counter1++;
		vector<double> features;
		features.clear();
		for(std::vector<double>::const_iterator it = msg->data.begin(); it != msg->data.end(); ++it)
		{
			features.push_back(*it);
		}
		anbc.predict(features);
		UINT predictedClassLabel = anbc.getPredictedClassLabel();
		if (1 == predictedClassLabel) counter2++;
		printf("Predicted Class Label: %d\n", predictedClassLabel);
		if (counter1 >= 50)
		{
			double likelihood = (double)counter2 / (double)counter1;
			printf("Likelihood: %f\n", likelihood);
			counter1 = 0;
			counter2 = 0;
		}
	}

	//constructor
	Livetest()
	{
		printf("Initializing...\n");
		sub1 = nh.subscribe("/featureVectorX", 100, &Livetest::callBack, this);  //Don't use "/featureVector"! it's been used 
		pub = nh.advertise<std_msgs::String>("finalLabel", 100);   //publish to the analyzer that directly publishes to interpreter
		receivedFrameNum = 0;
    		if( !anbc.load( "/home/radar/catkin_ws/src/beginner_tutorials/src/ANBCModel.grt" ) )  //dir of the cpp source files...
		{
        		cout << "ERROR: Failed to load the model from a file!\n";
    		}
        	printf("Done\n");
	}
};



int main(int argc, char **argv)
{
    	//subscribe to featureVector
	ros::init(argc, argv, "live_test");
	Livetest obj;
	ros::spin();
    	return 0;
  
}

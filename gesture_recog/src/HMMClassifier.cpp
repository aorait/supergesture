#include "ros/ros.h"
#include "std_msgs/String.h"
#include <sstream>
#include <vector>
#include <list>
#include "/home/aishanou/Documents/beckonbotgesture/catkinGES/src/gesture_recog/include/GRT/GRT.h"
#include "std_msgs/Float64.h"
#include <geometry_msgs/PoseStamped.h>
#include <cmath>
#include "std_msgs/MultiArrayLayout.h"
#include "std_msgs/MultiArrayDimension.h"
#include "std_msgs/Float64MultiArray.h"
#include <sys/time.h>

#define dimension 4
#define bunch_size 260

using namespace GRT;
using namespace std;

class MyFrame
{
public:
	int vecData[dimension];

	MyFrame(double array[dimension])
	{
		for (int i=0; i<dimension; i++)  //the "normalization" is here
		{
			vecData[i] = (int)(array[i]*500);
		}
	}
};

class HMMClassifier
{
	ros::NodeHandle nh;
	ros::Publisher pub;  //publish to topic "finalLabel"
	ros::Publisher pub_state; //publish to topic "/gesture/global_state"
	ros::Subscriber sub1;  //listen to topic "/gesture/featureVectorX"
	ros::Subscriber sub_state; //listen to topic "/gesture/global_state"
	TimeSeriesClassificationData testingData;
	MatrixDouble testingSample;
	int receivedFrameNum;  //let's keep buffer size, sample at granularity of 5, start at receiving buffer size
	std::list<MyFrame> buffer;
	HMM hmm;

	
	std::vector<MyFrame> recording_frames;
	std::vector<MyFrame> last_few_frames;
	int frame_count;
	bool recording;  //means now recording 
	bool cooling; //means just finished a recording, will do nothing now!
	int cooling_count;
	struct timeval t1, t2;
	//suggestion: focus on detecting "starting point", let it auto stop after 2.5s
public:

	int cal_delta(MyFrame f1, MyFrame f2)
	{
		int sum = 0;
		for (int i=0; i<dimension; i++)
		{
			int diff = f1.vecData[i] - f2.vecData[i];
			sum += diff>0? diff : 0-diff;
		}
		return sum;
	}


	void callBack(const std_msgs::Float64MultiArray::ConstPtr& msg)
	{
		//check global state == LISTENING ? ignore if not
		//state: an external global variable
		//if (state != LISTENING) return;             
		double array[dimension];
		int i = 0;
		for(std::vector<double>::const_iterator it = msg->data.begin(); it != msg->data.end(); ++it)
		{
			array[i] = *it;
			i++;
		}
		MyFrame frame(array);
		recording_frames.push_back(frame);
		frame_count ++;
		if (frame_count > bunch_size)
		{
			//do classification!
			testingData.clear();
			testingData.setNumDimensions( dimension );
			testingData.setDatasetName("TestingData");
			testingSample.clear();
			VectorDouble sample( dimension );

			int len = recording_frames.size();
			for (int i=0; i<len; i++)
			{
				MyFrame frame = recording_frames.at(i);
				for (int j=0; j<dimension; j++)
					sample[j] = frame.vecData[j];
				testingSample.push_back( sample );
			}
			testingData.addSample( 0 , testingSample );
			hmm.predict( testingData[0].getData() );
			printf("PredictedClassLabel: %d",  hmm.getPredictedClassLabel());
			std_msgs::String msg;
			msg.data = std::to_string(hmm.getPredictedClassLabel());
			pub.publish(msg);
			//reset
			//state = FREEMOVEMENT;
                }
	}

	//constructor
	HMMClassifier()
	{
		printf("Initialization...");
		receivedFrameNum = 0;
		frame_count = 0;
        	sub1 = nh.subscribe("/gesture/classifierFeatureVector", 100, &HMMClassifier::callBack, this);
		//sub_state = nh.subscribe("/gesture/global_state", 100, &HMMClassifier::callback_state_change, this);
		pub = nh.advertise<std_msgs::String>("finalLabel", 100);
		//pub_state = nh.advertise<std_msgs::String
		if( !hmm.load( "HMMModel.grt" ) )  //dir of the cpp source files...
        	printf("ERROR: Failed to load the model from a file!\n");
    		receivedFrameNum = 0;
		printf("Done\n");
	}
};

int main(int argc, char **argv)
{
	ros::init(argc, argv, "classifierFeatureVector");
	HMMClassifier obj;
	ros::spin();
  	return 0;
}

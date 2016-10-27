# supergesture
AUTHORS: Dacheng Liu, Aishanou Rait, James Laney
PURPOSE: To detect and classify gestures in real time.

EQUIPMENTS: 
Motion capture system sending rigid body poses over ROS topics

DESCRIPTION: 
1. Start of the gesture is identified by an Adaptive Naive Bayes Classifier.
2. The end of the gesture is currently set after a specified time interval.
3. The intermediate gesture is recognised using Hidden Markov Models.

# Model
 Name : EasyCap H.264
 
 Interface : USB
 
# How to use
1. Copy Video Capture folder and place in catin_ws
2. Run the following command
`catkin_make | -D OpenCV_DIR=/usr/local/share/OpenCV` 

(OpenCV v3.4.0 required - see Caution)

# Caution 
The default OpenCV version does not support Video Capture ROS package. 
Therefore you should install `OpenCV v3.4.0` to run the package properly.
[OpenCV Installation Guide](https://www.pytorials.com/how-to-install-opencv340-on-ubuntu1604/ "OpenCV v3.4.0")

Once you install OpenCV v3.4.0, run the following command
`sudo cp ~/src/opencv-3.4.0/build/lib cv2.so /opt/ros/kinetic/lib/python2.7/dist-packages/cv2.so`

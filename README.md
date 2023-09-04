# ardupilot_ros package for non-gps navigation

## Description : 
This is a ROS package for non-gps navigation for ardupilot containing all the required files and listed dependencies. This also contains a file from [thien94/vision_to_mavros](https://github.com/thien94/vision_to_mavros) to set origin. 

Detailed description of the project and setup: [Ardupilot Docs - Cartographer](https://ardupilot.org/dev/docs/ros-cartographer-slam.html)

This package contains all the modified files for the following packages:
````
mavros
cartographer_ros
robot_pose_publisher
navigation
````

ap_navigation: https://github.com/ArduPilot/companion/tree/master/Common/ROS/ap_navigation

Important PRs: https://github.com/mavlink/mavros/pull/1780


## Requirements :
Ubuntu version 18.04 or 20.04

ROS Melodic or Noetic

## Usage 1 :

On the ardupilot latest stable, run:

````
source ~/ardupilot/Tools/environment_install/install-ROS-ubuntu.sh
````
This will install the whole ardupilot-ros environment and install all the requirements.

## Usage 2 :

This package contains a main launch file launching all the required launch files in a go.

Simply,

````
cd <ros_ws>/src/
git clone https://github.com/snktshrma/ardupilot_ros
git clone https://github.com/GT-RAIL/robot_pose_publisher.git
cd ..
wstool init src
wstool merge -t src https://raw.githubusercontent.com/cartographer-project/cartographer_ros/master/cartographer_ros.rosinstall
wstool update -t src
sudo rosdep init
rosdep update
rosdep install --from-paths src --ignore-src -r -y
sudo source src/cartographer/scripts/install_abseil.sh
sudo apt-get remove ros-${ROS_DISTRO}-abseil-cpp
catkin build
````

## Run :

### On 1st Terminal
````
roslaunch ardupilot_ros gzbo.launch #for quadrotor
````
or
````
roslaunch ardupilot_ros gzbo_rover.launch #for rover
````

### On 2nd Terminal (under ~/ardupilot/ArduCopter or ~/ardupilot/Rover)
````
../Tools/autotest/sim_vehicle.py -f gazebo-iris
````
or
````
sim_vehicle.py -v APMrover2 -f gazebo-rover -m --mav10 -I1
````

### On 3rd Terminal
````
roslaunch ardupilot_ros main.launch
````

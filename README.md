# ardupilot_ros: ROS 2 use cases with Ardupilot

## Requirements

### System Requirements

* [ROS Humble](https://docs.ros.org/en/humble/Installation.html)

* [Gazebo Garden](https://gazebosim.org/docs/garden/install)

* [Cartographer ROS](https://google-cartographer-ros.readthedocs.io/en/latest/)
   * Recommended: Install Google Cartographer with rosdep

### Workspace Requirements

* [ardupilot_gz](https://github.com/ArduPilot/ardupilot_gz)

* [cartographer_ros]()

## Installation

Clone this repository into your ros2 workspace alongside ardupilot_gz:
```bash
cd ~/ros2_ws/src
git clone git@github.com:ardupilot/ardupilot_ros.git
```

Install dependencies using rosdep:
```bash
cd ~/ros2_ws
rosdep install --from-paths src --ignore-src -r
```

## Build

Build it with colcon build:
```bash
cd ~/ros2_ws
source /opt/ros/humble/setup.bash
colcon build --packages-up-to ardupilot_ros ardupilot_gz_bringup

```

## Usage

### 1. Cartographer running with LiDAR on copter

This simulation has an Iris copter equipped with a 360 degrees 2D LiDAR in a maze world.
To launch rviz and gazebo, run:

```bash
cd ~/ros2_ws
source install/setup.sh
ros2 launch ardupilot_gz_bringup iris_maze.launch.py
```
In another terminal, with the world and copter in place, launch cartographer to generate SLAM:

```bash
cd ~/ros2_ws
source install/setup.sh
ros2 launch ardupilot_ros cartographer.launch.py
```

If you'd like to get the information from Cartographer to go into Ardupilot's extended kalman filter, you will need to change some parameters. You can do that through any GCS, including mavproxy:

-  AHRS_EKF_TYPE = 3 to use EKF3
-  EK2_ENABLE = 0 to disable EKF2
-  EK3_ENABLE = 1 to enable EKF3
-  EK3_SRC1_POSXY = 6 to set position horizontal source to ExternalNAV
-  EK3_SRC1_POSZ = 1 to set position vertical source to Baro
-  EK3_SRC1_VELXY = 6 to set velocity horizontal source to ExternalNAV
-  EK3_SRC1_VELZ = 6 to set vertical velocity source to ExternalNAV
-  EK3_SRC1_YAW = 6 to set yaw source to ExternalNAV
-  VISO_TYPE = 1 to enable visual odometry
-  ARMING_CHECK = 388598 (optional, to disable GPS checks)

The parameters above are recommended for SITL. If you plan on using this on a real copter, it is a good idea to setup a second source of EKF. This way the robot doesn't crash if the external odometry you are providing stops publishing or gets lost.

Please refer to this link for more information on [Common EKF Sources](https://ardupilot.org/copter/docs/common-ekf-sources.html>) as well as this guide on [GPS / Non-GPS Transitions](https://ardupilot.org/copter/docs/common-non-gps-to-gps.html).

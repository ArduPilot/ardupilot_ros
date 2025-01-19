# ardupilot_cartographer: SLAM with google cartographer

[![pre-commit](https://img.shields.io/badge/pre--commit-enabled-brightgreen?logo=pre-commit)](https://github.com/pre-commit/pre-commit)

This package integrates Google Cartographer with ArduPilot for SLAM-based navigation.

## Requirements, Installation, and Build

Please follow the instructions in the [ardupilot_ros README](../README.md) for overall setup, workspace requirements, and building the repository.

## Usage

### 1. Cartographer running with LiDAR on copter

This simulation has an Iris copter equipped with a 360 degrees 2D LiDAR in a maze world.
To launch rviz and gazebo, run:

```bash
cd ~/ros2_ws
source install/setup.bash
ros2 launch ardupilot_gz_bringup iris_maze.launch.py
```
In another terminal, with the world and copter in place, launch cartographer to generate SLAM:

```bash
cd ~/ros2_ws
source install/setup.bash
ros2 launch ardupilot_cartographer cartographer.launch.py
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

### 2. Joystick Controller

The joystick controller allows you to control ArduPilot through a ROS joy topic.

```bash
cd ~/ros2_ws
source install/setup.bash
ros2 run ardupilot_cartographer joy_controller
```

Then run the controller using,

`ros2 run ardupilot_cartographer joy_controller`

Now, using the keyboard keys you can control the drone.

### 3. Obstacle avoidance using Cartographer and Nav2

Using the same simulation as before, the nav2 node can be launched to control the copter once it is in the air.

Launch the simulation:

```bash
cd ~/ros2_ws
source install/setup.sh
ros2 launch ardupilot_gz_bringup iris_maze.launch.py rviz:=false
```
Launch cartographer:

```bash
cd ~/ros2_ws
source install/setup.sh
ros2 launch ardupilot_cartographer cartographer.launch.py rviz:=false
```

Launch nav2:

```bash
cd ~/ros2_ws
source install/setup.sh
ros2 launch ardupilot_cartographer navigation.launch.py
```

Takeoff the Copter using `mavproxy` to an altitude of 2.5m:

```bash
mavproxy.py --console --map --aircraft test --master=:14550

mode guided

arm throttle

takeoff 2.5
```

You may now navigate while mapping using the `Nav2 Goal` tool in RVIZ!

## Contribution Guideline

* Ensure the [pre-commit](https://github.com/pre-commit/pre-commit) hooks pass locally before creating your pull request by installing the hooks before committing.
   ```bash
   pre-commit install
   git commit
   ```
* See the [ArduPilot Contributing Guide](https://github.com/ArduPilot/ardupilot/blob/master/.github/CONTRIBUTING.md)

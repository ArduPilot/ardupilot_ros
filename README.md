# ardupilot_ros: ROS 2 use cases with Ardupilot

[![pre-commit](https://img.shields.io/badge/pre--commit-enabled-brightgreen?logo=pre-commit)](https://github.com/pre-commit/pre-commit)

This repository contains basic examples of autonomous control of an ArduPilot vehicle via ROS2.

Example 1: Using the Cartographer and NAV2 software (as ROS2 nodes) for autonomous object avoidance and route planning.
This has been tested with ArduCopter and ArduRover vehicles. This available as a simulated implementation.
Example 1: Using the Cartographer and NAV2 software (as ROS2 nodes) for autonomous object avoidance and route planning.
This has been tested with ArduCopter and ArduRover vehicles. This available as a simulated implementation.

## Requirements

### System Requirements

* [ROS Humble](https://docs.ros.org/en/humble/Installation.html)
* [Gazebo Garden](https://gazebosim.org/docs/garden/install).

### Workspace Requirements

* [ardupilot_gz](https://github.com/ArduPilot/ardupilot_gz)
* [ardupilot_ros](https://github.com/ArduPilot/ardupilot_ros)
* [micro_ros_agent](https://github.com/micro-ROS/micro-ROS-Agent)
* [ardupilot](https://github.com/ArduPilot/ardupilot)
* [ardupilot_sitl_models](https://github.com/ArduPilot/SITL_Models)
* [ardupilot_gazebo](https://github.com/ArduPilot/ardupilot_gazebo)

## Installation

Clone this repository into your ros2 workspace alongside ``ardupilot_gz``:
```bash
cd ~/ros2_ws/src
git clone git@github.com:ardupilot/ardupilot_ros.git
git clone git@github.com:ardupilot/ardupilot_gz.git
```

Install dependencies using rosdep. This will automatically install the remainder of
the GitHub repositories in the "Workspace Requirements" section:
```bash
cd ~/ros2_ws
rosdep install --from-paths src --ignore-src -r --skip-keys gazebo-ros-pkgs
```

## Build

Build it with colcon build:
```bash
cd ~/ros2_ws
source /opt/ros/humble/setup.bash
colcon build --packages-up-to ardupilot_sim_bringup
```

>[!NOTE]
>If you run out of RAM during build, use the ``--parallel-workers 1`` argument to only build 1 package at a time.

## Usage

### Simulated Rover with Cartographer and NAV2

This uses a "Wildthumper" ground rover fitted with a 2D lidar. The lidar data is used by Cartographer and NAV2 for
building a map of the world and object avoidance during path planning.

Run the following commands to bring up the simulation world and Ardupilot:
```bash
cd ~/ros2_ws
source ./install/setup.bash
ros2 launch ardupilot_gz_bringup wildthumper_playpen.launch.py
```

Cartographer:
```bash
cd ~/ros2_ws
source ./install/setup.bash
ros2 launch ardupilot_cartographer cartographer.launch.py
```

NAV2:
```bash
cd ~/ros2_ws
source ./install/setup.bash
ros2 launch ardupilot_cartographer navigation.launch.py
```

GUI components (Cartographer RViz, NAV2 Rviz and ArduPilot ROS2 GUI):
```bash
cd ~/ros2_ws
source ./install/setup.bash
ros2 launch ardupilot_viz all_gui.launch.py
```

### Simulated Copter with Cartographer and NAV2

This uses a "Iris" multicopter fitted with a 2D lidar. The lidar data is used by Cartographer and NAV2 for
building a map of the world and object avoidance during path planning.

Run the following commands to bring up the simulation world and Ardupilot:
```bash
cd ~/ros2_ws
source ./install/setup.bash
ros2 launch ardupilot_gz_bringup iris_maze.launch.py lidar_dim:=2
```

Cartographer:
```bash
cd ~/ros2_ws
source ./install/setup.bash
ros2 launch ardupilot_cartographer cartographer.launch.py
```

NAV2:
```bash
cd ~/ros2_ws
source ./install/setup.bash
ros2 launch ardupilot_cartographer navigation.launch.py
```

GUI components (Cartographer RViz, NAV2 Rviz and ArduPilot ROS2 GUI):
```bash
cd ~/ros2_ws
source ./install/setup.bash
ros2 launch ardupilot_viz all_gui.launch.py
```

## ArduPilot GUI

The ``ardupilot_gui`` contains a simple ROS2 GUI for viewing and controlling the vehicle. It can be
launched via:

```bash
cd ~/ros2_ws
source /opt/ros/humble/setup.bash
source ./install/setup.sh
ros2 launch ardupilot_gui ardupilot_gui.launch.py
```

By default the GUI will connect to the vehicle with a System ID of 1 (Ardupilot default when using ``DDS_USE_NS``=1)

If using a different System ID, use arguments to specify:

```bash
# Connect to vehicle with System ID 2
ros2 launch ardupilot_gui ardupilot_gui.launch.py sysid:=2
```

For usage with older versions of ArduPilot that do not have the ``DDS_USE_NS`` parameter, use a ``sysid`` of 0:

```bash
# Connect to vehicle that doesn't use the DDS_USE_NS parameter
ros2 launch ardupilot_gui ardupilot_gui.launch.py sysid:=0
```

## Contribution Guideline

* Ensure the [pre-commit](https://github.com/pre-commit/pre-commit) hooks pass locally before creating your pull request by installing the hooks before committing.
   ```bash
   pre-commit install
   git commit
   ```
* See the [ArduPilot Contributing Guide](https://github.com/ArduPilot/ardupilot/blob/master/.github/CONTRIBUTING.md)

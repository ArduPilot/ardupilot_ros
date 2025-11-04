# ardupilot_ros: ROS 2 use cases with Ardupilot

[![pre-commit](https://img.shields.io/badge/pre--commit-enabled-brightgreen?logo=pre-commit)](https://github.com/pre-commit/pre-commit)

This repository contains basic examples of autonomous control of an ArduPilot vehicle via ROS2.

Example 1: Using the Cartographer (fused IMU + Lidar scan) and NAV2 software for autonomous object avoidance and route planning.
This has been tested with ArduCopter and ArduRover vehicles. This available as simulated and "real-life" (IRL) implementations.

Note that the simulated and real-life implementations have different installation and configuration steps.

The real-life implementation is split into a backend and GUI frontend. The backend is designed to be run on a companion
computer on the vehicle (such as a Raspberry Pi or Jetson).

## Requirements

### System Requirements

* [ROS Humble](https://docs.ros.org/en/humble/Installation.html)
* [Gazebo Garden](https://gazebosim.org/docs/garden/install) (only required for simulated implementation)

### Workspace Requirements

* [ardupilot_gz](https://github.com/ArduPilot/ardupilot_gz) (only required for simulated implementation)
* [ardupilot_ros](https://github.com/ArduPilot/ardupilot_ros)
* [micro_ros_agent](https://github.com/micro-ROS/micro-ROS-Agent)
* [ardupilot](https://github.com/ArduPilot/ardupilot)  (only required for simulated implementation)
* [ardupilot_sitl_models](https://github.com/ArduPilot/SITL_Models) (only required for simulated implementation)
* [ardupilot_gazebo](https://github.com/ArduPilot/ardupilot_gazebo) (only required for simulated implementation)

## Installation
### Simulated
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

### Real-life (backend)
This will typically be installed on a companion computer on the vehicle.

Clone this repository into your ros2 workspace, along with ``micro-ROS-Agent``:
```bash
cd ~/ros2_ws/src
git clone https://github.com/ardupilot/ardupilot_ros.git
git clone --branch humble https://github.com/micro-ROS/micro-ROS-Agent.git
```

Install dependencies using rosdep:
```bash
cd ~/ros2_ws
source /opt/ros/humble/setup.bash
sudo apt install python3-evdev
rosdep install --from-paths src --ignore-src -r --skip-keys "ardupilot_sim_bringup ardupilot_viz ardupilot_gui cartographer_rviz nav2_rviz_plugins rviz-common ros_gz_sim"
```

### Real-life (GUI)
This will typically be installed on your GCS laptop, or similar.

Clone this repository into your ros2 workspace, along with ``ardupilot_msgs``:
```bash
cd ~/ros2_ws/src
git clone git@github.com:ardupilot/ardupilot_ros.git
git clone --filter=blob:none --no-checkout https://github.com/ArduPilot/ardupilot.git
cd ./ardupilot
git sparse-checkout init --cone
git sparse-checkout set Tools/ros2/ardupilot_msgs
git checkout master
```

## Build
### Simulated
Build it with colcon build:
```bash
cd ~/ros2_ws
source /opt/ros/humble/setup.bash
colcon build --packages-up-to ardupilot_sim_bringup
```

>[!NOTE]
>If you run out of RAM during build, use the ``--parallel-workers 1`` argument to only build 1 package at a time.

### Real-life (backend)
Build it with colcon build:
```bash
cd ~/ros2_ws
source /opt/ros/humble/setup.bash
colcon build --packages-up-to ardupilot_irl_bringup
```

### Real-life (GUI)
Build it with colcon build:
```bash
cd ~/ros2_ws
source /opt/ros/humble/setup.bash
colcon build --packages-up-to ardupilot_viz
```

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
source ./install/setup.bashros2 launch ardupilot_gz_bringup iris_maze.launch.py lidar_dim:=2

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
### Real-life with Cartographer and NAV2

Run the following commands to bring up Cartographer, NAV2 and micro-ROS-Agent.
Note the arguments for the MicroROS source (either UART or UDP):

```bash
cd ~/ros2_ws
source ./install/setup.bash
ros2 launch ardupilot_irl_bringup backend.launch.py micro_ros_transport=serial micro_ros_port=/dev/ttyACM0 micro_ros_baudrate=115200
```

```bash
cd ~/ros2_ws
source ./install/setup.bash
ros2 launch ardupilot_irl_bringup backend.launch.py micro_ros_transport:=udp4 micro_ros_port:=2019
```

>[!NOTE]
>You will need a Lidar Node publishing to the ``/scan`` topic

Then on your GCS, bring up the front-end GUI:

```bash
cd ~/ros2_ws
source ./install/setup.bash
ros2 launch ardupilot_viz all_gui.launch.py
```

### Tuning NAV2:

In both the simulator and Real-life bringups, the vehicle's physical capabilities can be input.

They are used as arguments to ``ros2 launch ardupilot_cartographer navigation.launch.py`` (in simulation mode)
or ``ros2 launch ardupilot_irl_bringup backend.launch.py`` (in real-life).

The arguments consist of:
- ``robot_radius``: Vehicle radius in m, measured from the ``base_link`` (default 0.35)
- ``min_clearance``: Desired minimum clearance given to obstacles in m (default 0.1)
- ``max_speed``: Maximum horizontal velocity in m/s (default 1)
- ``max_angular_speed``: Maximum horizontal angular speed in rad/sec (default 1)

>[!NOTE]
>The NAV2 bringup can accept arguments for ``robot_radius`` (robot radius in m), ``min_clearance`` (desired minimum
> clearance given to obstacles in m), ``max_speed``  and ``max_angular_speed``

## Contribution Guideline

* Ensure the [pre-commit](https://github.com/pre-commit/pre-commit) hooks pass locally before creating your pull request by installing the hooks before committing.
   ```bash
   pre-commit install
   git commit
   pre-commit run --all-files
   ```
* See the [ArduPilot Contributing Guide](https://github.com/ArduPilot/ardupilot/blob/master/.github/CONTRIBUTING.md)

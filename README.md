# ardupilot_ros: ROS 2 use cases with Ardupilot

[![pre-commit](https://img.shields.io/badge/pre--commit-enabled-brightgreen?logo=pre-commit)](https://github.com/pre-commit/pre-commit)

## Requirements

### System Requirements

* [ROS Humble](https://docs.ros.org/en/humble/Installation.html)

* [Gazebo Garden](https://gazebosim.org/docs/garden/install)

* [Cartographer ROS](https://google-cartographer-ros.readthedocs.io/en/latest/)
   * Recommended: Install Google Cartographer with rosdep

### Workspace Requirements

* [ardupilot_gz](https://github.com/ArduPilot/ardupilot_gz)

* [ardupilot_ros]()

## Installation

Clone this repository into your ros2 workspace alongside ardupilot_gz:
```bash
cd ~/ros2_ws/src
git clone git@github.com:ardupilot/ardupilot_ros.git
```

Install dependencies using rosdep:
```bash
cd ~/ros2_ws
rosdep install --from-paths src --ignore-src -r --skip-keys gazebo-ros-pkgs
```

## Build

Build it with colcon build:
```bash
cd ~/ros2_ws
source /opt/ros/humble/setup.bash
colcon build --packages-up-to ardupilot_ros ardupilot_gz_bringup

```

## Usage

Refer to individual package READMEs for detailed usage instructions:

* [ardupilot_cartographer](ardupilot_cartographer): Instructions to run Cartographer SLAM.

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

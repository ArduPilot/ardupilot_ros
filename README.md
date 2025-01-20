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

## Contribution Guideline

* Ensure the [pre-commit](https://github.com/pre-commit/pre-commit) hooks pass locally before creating your pull request by installing the hooks before committing.
   ```bash
   pre-commit install
   git commit
   ```
* See the [ArduPilot Contributing Guide](https://github.com/ArduPilot/ardupilot/blob/master/.github/CONTRIBUTING.md)

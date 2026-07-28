# Trajectory Tracking Controller

A PID-based trajectory tracking controller for ArduPilot-based autonomous quadcopters, implemented in ROS2.

> *Based on: Dario Brescianini, Markus Hehn, Raffaello D'Andrea, Nonlinear Quadrocopter Attitude Control, ETH Zurich, 2013.* [\[paper\]](https://nrotella.github.io/assets/pdf/px4_attitude_control.pdf)

>**Original Repo**: [Zhefan-Xu/trajectory_controller](https://github.com/Zhefan-Xu/trajectory_controller) (ROS1/PX4 version)

## Overview

This package implements a cascaded PID controller for trajectory tracking with three control modes:
- **Acceleration Control**: Direct acceleration commands (default)
- **Attitude Control**: Attitude and thrust commands
- **Body Rate Control**: Body angular rates and thrust commands

The controller includes automatic hover thrust estimation using a Kalman filter and provides visualization topics for debugging.

## Installation

### Prerequisites
- ROS2 (Tested on Humble)
- ArduPilot ROS2 (`ardupilot_msgs` package)
- Eigen3

### Build

```bash
cd ~/ros2_ws/src
git clone https://github.com/snktshrma/trajectory_tracker
cd ~/ros2_ws
colcon build --packages-select trajectory_tracker
source install/setup.bash
```

## Usage

### Launch the Controller

```bash
ros2 launch trajectory_tracker trajectory_tracker.launch.py
```

### Configuration

Controller parameters can be configured in `cfg/controller_param.yaml`:

```yaml
controller:
  body_rate_control: false
  attitude_control: false
  acceleration_control: true  # Default mode
  
  position_p: [1.5, 1.5, 1.5]
  position_i: [0.0, 0.0, 0.1]
  position_d: [0.0, 0.0, 0.0]
  
  velocity_p: [1.0, 1.0, 1.0]
  velocity_i: [0.0, 0.0, 0.0]
  velocity_d: [0.0, 0.0, 0.0]
  
  attitude_control_tau: 0.3
  hover_thrust: 0.70
  verbose: false
```

## ROS2 Topics

### Subscribed Topics
- `/mavros/local_position/odom` - Robot odometry (nav_msgs/msg/Odometry) {Will soon have a DDS interface}
- `/ap/imu/experimental/data` - IMU data (sensor_msgs/msg/Imu)
- `/autonomous_flight/target_state` - Target trajectory states (trajectory_tracker/msg/Target)

### Published Topics
- `/ap/experimental/target_attitude` - Attitude commands (ardupilot_msgs/msg/AttitudeTarget)
- `/ap/cmd_loc_pose` - Position/acceleration commands (ardupilot_msgs/msg/LocalPosition)
- `/trajectory_tracker/robot_pose` - Current robot pose (geometry_msgs/msg/PoseStamped)
- `/trajectory_tracker/trajectory_history` - Robot trajectory history (nav_msgs/msg/Path)
- `/trajectory_tracker/target_pose` - Target pose (geometry_msgs/msg/PoseStamped)
- `/trajectory_tracker/target_trajectory_history` - Target trajectory history (nav_msgs/msg/Path)
- `/trajectory_tracker/vel_and_acc_info` - Velocity and acceleration visualization (visualization_msgs/msg/Marker)

## Controller Structure

The controller implements a cascaded control architecture:

```
Target Trajectory → Position PID → Velocity PID → Acceleration Reference
                                                      ↓
                                              Attitude Reference
                                                      ↓
                                              Body Rate Commands
```

## Circle Tracking Example

The package includes a circle tracking example that generates a circular trajectory and publishes target states to the trajectory tracker.

### Usage

```bash
ros2 launch trajectory_tracker circle_tracking.launch.py
```

### Configuration

Edit `cfg/circle_tracking_param.yaml` to adjust:
- `radius`: Circle radius in meters (default: 2.0 m)
- `velocity`: Tangential velocity in m/s (default: 1.0 m/s)
- `height`: Flight height in meters (default: 1.0 m)
- `center_x`, `center_y`: Circle center offset (default: 0.0, 0.0)
- `yaw_control`: If true, yaw follows circle tangent; if false, keeps current yaw (default: false)
- `publish_rate`: Publishing rate in Hz (default: 100.0)

## TODO
- [x] Add Trajectory Setpoint and Attitude control DDS interface to ArduCopter
- [x] Add an example circle tracking implementation
- [ ] Add odometry DDS interface for ArduPilot
- [ ] Add required DDS interfaces for ArduRover
- [ ] Make it ready as a reference for users to be able to use any offboard trajectory planners, avoidance and controllers for ArduCopter control
- [ ] A new control interface for Fixed Wings

## Credits

**Original Author**: [Zhefan Xu](https://zhefanxu.com/), Computational Engineering & Robotics Lab (CERLAB) at Carnegie Mellon University (CMU)

**Research Paper**: Dario Brescianini, Markus Hehn, Raffaello D'Andrea, Nonlinear Quadrocopter Attitude Control, ETH Zurich, 2013. [\[paper\]](https://nrotella.github.io/assets/pdf/px4_attitude_control.pdf)
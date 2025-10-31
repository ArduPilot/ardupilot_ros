import math

from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node
from launch_ros.substitutions import FindPackageShare


def generate_launch_description():
    # ***** Launch arguments *****
    use_sim_time_arg = DeclareLaunchArgument("use_sim_time", default_value="true")

    cartographer_node = Node(
        package="cartographer_ros",
        executable="cartographer_node",
        parameters=[{"use_sim_time": LaunchConfiguration("use_sim_time")}],
        arguments=[
            "-configuration_directory",
            FindPackageShare("ardupilot_cartographer").find("ardupilot_cartographer")
            + "/config",
            "-configuration_basename",
            "cartographer.lua",
        ],
        output="screen",
        remappings=[
            ("/imu", "/ap/imu/experimental/data"),
            # ("/odom", "/odometry"),
        ],
    )

    # Static transform from base_link to base_link_ned
    # As ArduPilot IMU uses base_link_ned frame, we need to define a static transform
    # NED to ENU (ROS Default) requires:
    # - 90 degree rotation around Z axis (yaw) to align X(North)->Y(East)
    # - 180 degree rotation around X axis (roll) to flip Z(Down)->Z(Up)
    # This converts: X(North)->X(East), Y(East)->Y(North), Z(Down)->Z(Up)
    ardupilot_to_ned_tf = Node(
        package="tf2_ros",
        executable="static_transform_publisher",
        arguments=[
            "--x",
            "0",
            "--y",
            "0",
            "--z",
            "0",
            "--yaw",
            str(math.pi / 2),
            "--pitch",
            "0",
            "--roll",
            str(math.pi),
            "--frame-id",
            "base_link",
            "--child-frame-id",
            "base_link_ned",
        ],
    )

    cartographer_occupancy_grid_node = Node(
        package="cartographer_ros",
        executable="cartographer_occupancy_grid_node",
        parameters=[
            {"use_sim_time": LaunchConfiguration("use_sim_time")},
            {"resolution": 0.05},
        ],
    )

    return LaunchDescription(
        [
            # Arguments
            use_sim_time_arg,
            # Nodes
            cartographer_node,
            cartographer_occupancy_grid_node,
            ardupilot_to_ned_tf,
        ]
    )

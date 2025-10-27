import os

from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node
from launch_ros.substitutions import FindPackageShare


def generate_launch_description():
    ## ***** Launch arguments *****
    use_sim_time_arg = DeclareLaunchArgument("use_sim_time", default_value="true")

    ## ***** Nodes *****
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
            ("/imu", "/imu"),
            ("/odom", "/odometry"),
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
        ]
    )

from launch import LaunchDescription
from launch.actions import IncludeLaunchDescription, GroupAction
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch_ros.actions import Node
from launch_ros.substitutions import FindPackageShare
from launch.actions import DeclareLaunchArgument
from launch.substitutions import LaunchConfiguration
from pathlib import Path

"""Generate a launch description for the navigation example."""


def generate_launch_description():
    # ***** Launch arguments *****
    use_sim_time_arg = DeclareLaunchArgument("use_sim_time", default_value="true")

    # Navigation
    navigation = GroupAction(
        actions=[
            # TODO: enable when navigation2 supports twist stamped
            # SetRemap(src="/cmd_vel", dst="/ap/cmd_vel"),
            IncludeLaunchDescription(
                PythonLaunchDescriptionSource(
                    str(
                        Path(
                            FindPackageShare("nav2_bringup").find("nav2_bringup"),
                            "launch",
                            "navigation_launch.py",
                        )
                    )
                ),
                launch_arguments={
                    "use_sim_time": LaunchConfiguration("use_sim_time"),
                    "params_file": FindPackageShare("ardupilot_cartographer").find(
                        "ardupilot_cartographer"
                    )
                    + "/config"
                    + "/navigation.yaml",
                }.items(),
            ),
        ]
    )

    # TODO: disable when navigation2 supports twist stamped
    # Twist stamper.
    twist_stamper = Node(
        package="twist_stamper",
        executable="twist_stamper",
        parameters=[
            {"frame_id": "base_link"},
        ],
        remappings=[
            ("cmd_vel_in", "cmd_vel"),
            ("cmd_vel_out", "ap/cmd_vel"),
        ],
    )

    return LaunchDescription(
        [
            # Arguments
            use_sim_time_arg,
            # nodes
            navigation,
            twist_stamper,
        ]
    )

from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node


def generate_launch_description():
    sysid_arg = DeclareLaunchArgument(
        "sysid",
        default_value="1",
        description="System ID for the MAVLink vehicle. 0 means no sysid prefix.",
    )

    return LaunchDescription(
        [
            sysid_arg,
            Node(
                package="ardupilot_gui",
                executable="ardupilot_gui",
                name="ardupilot_gui_node",
                output="screen",
                parameters=[{"sysid": LaunchConfiguration("sysid")}],
            ),
        ]
    )

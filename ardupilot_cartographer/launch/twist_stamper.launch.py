from launch import LaunchDescription
from launch_ros.actions import Node
from pathlib import Path

"""Generate a launch description for the twist_stamper node."""


def generate_launch_description():
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
            twist_stamper,
        ]
    )

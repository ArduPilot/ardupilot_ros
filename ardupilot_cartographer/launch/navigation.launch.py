import os

from launch import LaunchDescription
from launch.actions import (
    DeclareLaunchArgument,
    IncludeLaunchDescription,
    GroupAction,
    OpaqueFunction,
)
from launch.substitutions import LaunchConfiguration
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.conditions import IfCondition
from launch_ros.actions import SetRemap
from launch_ros.actions import Node
from launch_ros.substitutions import FindPackageShare
from pathlib import Path

"""Generate a launch description for the navigation example."""


def setup_navigation(context):
    """Setup navigation with dynamic topic prefix based on sysid."""
    sysid = int(context.launch_configurations["sysid"])

    if sysid > 0:
        topic_prefix = f"/ap/v{sysid}"
    else:
        topic_prefix = "/ap"

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
                    "use_sim_time": "true",
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
            ("cmd_vel_out", f"{topic_prefix}/cmd_vel"),
        ],
    )

    # RViz.
    rviz = Node(
        package="rviz2",
        executable="rviz2",
        arguments=[
            "-d",
            str(
                Path(
                    FindPackageShare("ardupilot_cartographer").find(
                        "ardupilot_cartographer"
                    ),
                    "rviz",
                    "navigation.rviz",
                )
            ),
        ],
        condition=IfCondition(LaunchConfiguration("rviz")),
    )

    return [navigation, twist_stamper, rviz]


def generate_launch_description():
    # Ensure `SDF_PATH` is populated as `sdformat_urdf` uses this rather
    # than `GZ_SIM_RESOURCE_PATH` to locate resources.
    if "GZ_SIM_RESOURCE_PATH" in os.environ:
        gz_sim_resource_path = os.environ["GZ_SIM_RESOURCE_PATH"]

        if "SDF_PATH" in os.environ:
            sdf_path = os.environ["SDF_PATH"]
            os.environ["SDF_PATH"] = sdf_path + ":" + gz_sim_resource_path
        else:
            os.environ["SDF_PATH"] = gz_sim_resource_path

    sysid_arg = DeclareLaunchArgument(
        "sysid",
        default_value="0",
        description="System ID for the MAVLink vehicle. 0 means no sysid prefix.",
    )

    return LaunchDescription(
        [
            DeclareLaunchArgument(
                "rviz", default_value="true", description="Open RViz."
            ),
            sysid_arg,
            OpaqueFunction(function=setup_navigation),
        ]
    )

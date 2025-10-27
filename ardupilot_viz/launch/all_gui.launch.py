import os

from launch import LaunchDescription
from launch_ros.actions import Node
from launch_ros.substitutions import FindPackageShare
from pathlib import Path


def generate_launch_description():
    # Robot description.

    # Ensure `SDF_PATH` is populated as `sdformat_urdf`` uses this rather
    # than `GZ_SIM_RESOURCE_PATH` to locate resources.
    if "GZ_SIM_RESOURCE_PATH" in os.environ:
        gz_sim_resource_path = os.environ["GZ_SIM_RESOURCE_PATH"]

        if "SDF_PATH" in os.environ:
            sdf_path = os.environ["SDF_PATH"]
            os.environ["SDF_PATH"] = sdf_path + ":" + gz_sim_resource_path
        else:
            os.environ["SDF_PATH"] = gz_sim_resource_path

    # RViz cartographer.
    rviz_cartographer = Node(
        package="rviz2",
        executable="rviz2",
        arguments=[
            "-d",
            str(
                Path(
                    FindPackageShare("ardupilot_viz").find("ardupilot_viz"),
                    "rviz",
                    "cartographer.rviz",
                )
            ),
        ],
    )

    # RViz NAV2.
    rviz_nav2 = Node(
        package="rviz2",
        executable="rviz2",
        arguments=[
            "-d",
            str(
                Path(
                    FindPackageShare("ardupilot_viz").find("ardupilot_viz"),
                    "rviz",
                    "navigation.rviz",
                )
            ),
        ],
    )

    # Ardupilot GUI
    ardupilot_gui = Node(
        package="ardupilot_gui",
        executable="ardupilot_gui",
        name="ardupilot_gui",
        output="screen",
    )

    return LaunchDescription([rviz_nav2, rviz_cartographer, ardupilot_gui])

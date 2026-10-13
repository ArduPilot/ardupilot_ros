import os

from launch import LaunchContext, LaunchDescription
from launch.actions import DeclareLaunchArgument, OpaqueFunction
from launch.conditions import IfCondition
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node
from launch_ros.substitutions import FindPackageShare
from pathlib import Path


def generate_cartographer_nodes(context: LaunchContext):
    """Generate Cartographer nodes, optionally inside a vehicle namespace."""
    namespace = context.launch_configurations.get("namespace", "").strip()
    use_sim_time = LaunchConfiguration("use_sim_time")
    rviz_config = str(
        Path(
            FindPackageShare("ardupilot_cartographer").find(
                "ardupilot_cartographer"
            ),
            "rviz",
            "cartographer.rviz",
        )
    )

    # When namespace is empty we preserve the original single-vehicle/global
    # behavior. When it is non-empty we place all nodes in that namespace.
    # Cartographer's default sensor topic names ("scan", "odom", "imu", ...)
    # are relative, so namespacing the node alone redirects them into the
    # vehicle namespace. We still remap internal "odom" to the external
    # "odometry" topic published by ardupilot_gz.
    # The internal TF broadcaster/listener use absolute "/tf" and "/tf_static",
    # so they must be explicitly remapped into the vehicle namespace.
    if namespace:
        node_namespace = namespace
        sensor_remappings = [
            ("odom", "odometry"),
            ("/tf", f"/{namespace}/tf"),
            ("/tf_static", f"/{namespace}/tf_static"),
        ]
        rviz_remappings = [
            ("/map", "map"),
            ("/map_updates", "map_updates"),
            ("/odometry", "odometry"),
            ("/robot_description", "robot_description"),
            ("/scan", "scan"),
            ("/tf", "tf"),
            ("/tf_static", "tf_static"),
        ]
    else:
        node_namespace = None
        sensor_remappings = [
            ("/imu", "/imu"),
            ("/odom", "/odometry"),
        ]
        rviz_remappings = []

    config_dir = (
        FindPackageShare("ardupilot_cartographer").find("ardupilot_cartographer")
        + "/config"
    )

    cartographer_node = Node(
        package="cartographer_ros",
        executable="cartographer_node",
        namespace=node_namespace,
        parameters=[{"use_sim_time": use_sim_time}],
        arguments=[
            "-configuration_directory",
            config_dir,
            "-configuration_basename",
            "cartographer.lua",
        ],
        output="screen",
        remappings=sensor_remappings,
    )

    cartographer_occupancy_grid_node = Node(
        package="cartographer_ros",
        executable="cartographer_occupancy_grid_node",
        namespace=node_namespace,
        parameters=[
            {"use_sim_time": use_sim_time},
            {"resolution": 0.05},
        ],
    )

    # RViz.
    rviz = Node(
        package="rviz2",
        executable="rviz2",
        namespace=node_namespace,
        arguments=[
            "-d",
            rviz_config,
        ],
        condition=IfCondition(LaunchConfiguration("rviz")),
        remappings=rviz_remappings,
    )

    return [cartographer_node, cartographer_occupancy_grid_node, rviz]


def generate_launch_description():
    ## ***** Launch arguments *****
    use_sim_time_arg = DeclareLaunchArgument("use_sim_time", default_value="true")
    rviz_arg = DeclareLaunchArgument(
        "rviz", default_value="true", description="Open RViz."
    )
    namespace_arg = DeclareLaunchArgument(
        "namespace",
        default_value="",
        description=(
            "Vehicle namespace. "
            "Empty keeps the original single-vehicle/global behavior."
        ),
    )

    # Ensure `SDF_PATH` is populated as `sdformat_urdf` uses this rather
    # than `GZ_SIM_RESOURCE_PATH` to locate resources.
    if "GZ_SIM_RESOURCE_PATH" in os.environ:
        gz_sim_resource_path = os.environ["GZ_SIM_RESOURCE_PATH"]

        if "SDF_PATH" in os.environ:
            sdf_path = os.environ["SDF_PATH"]
            os.environ["SDF_PATH"] = sdf_path + ":" + gz_sim_resource_path
        else:
            os.environ["SDF_PATH"] = gz_sim_resource_path

    return LaunchDescription(
        [
            # Arguments
            use_sim_time_arg,
            rviz_arg,
            namespace_arg,
            # Nodes
            OpaqueFunction(function=generate_cartographer_nodes),
        ]
    )

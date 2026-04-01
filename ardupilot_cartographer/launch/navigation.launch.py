from pathlib import Path
import tempfile
from collections import OrderedDict
import yaml
from launch import LaunchDescription
from launch.actions import (
    IncludeLaunchDescription,
    GroupAction,
    DeclareLaunchArgument,
    OpaqueFunction,
)
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch_ros.actions import Node
from launch_ros.substitutions import FindPackageShare
from launch.substitutions import LaunchConfiguration

"""Generate a launch description for the navigation example."""


# Custom YAML representer to maintain order
def represent_ordereddict(dumper, data):
    return dumper.represent_dict(data.items())


yaml.add_representer(OrderedDict, represent_ordereddict)


def ordered_load(stream):
    """Load YAML while preserving order."""

    class OrderedLoader(yaml.SafeLoader):
        pass

    def construct_mapping(loader, node):
        loader.flatten_mapping(node)
        return OrderedDict(loader.construct_pairs(node))

    OrderedLoader.add_constructor(
        yaml.resolver.BaseResolver.DEFAULT_MAPPING_TAG, construct_mapping
    )

    return yaml.load(stream, OrderedLoader)


def modify_nav_config(context, *args, **kwargs):
    """Modify navigation config with launch arguments and save to temp file."""

    # Get launch argument values
    robot_radius = float(LaunchConfiguration("robot_radius").perform(context))
    min_clearance = float(LaunchConfiguration("min_clearance").perform(context))
    max_speed = float(LaunchConfiguration("max_speed").perform(context))
    max_angular_speed = float(LaunchConfiguration("max_angular_speed").perform(context))
    use_sim_time = LaunchConfiguration("use_sim_time").perform(context) == "true"

    # Calculate inflation radius (robot_radius + min_clearance)
    inflation_radius = robot_radius + min_clearance

    # Load base navigation config
    base_config_path = Path(
        FindPackageShare("ardupilot_cartographer").find("ardupilot_cartographer"),
        "config",
        "navigation.yaml",
    )

    with open(base_config_path, "r") as f:
        config = ordered_load(f)

    # Modify global costmap parameters
    if "global_costmap" in config and "global_costmap" in config["global_costmap"]:
        gc_params = config["global_costmap"]["global_costmap"]["ros__parameters"]
        gc_params["robot_radius"] = robot_radius

        # Update inflation layer
        if "inflation_layer" in gc_params:
            gc_params["inflation_layer"]["inflation_radius"] = inflation_radius

    # Modify local costmap parameters
    if "local_costmap" in config and "local_costmap" in config["local_costmap"]:
        lc_params = config["local_costmap"]["local_costmap"]["ros__parameters"]
        lc_params["robot_radius"] = robot_radius

        # Update inflation layer
        if "inflation_layer" in lc_params:
            lc_params["inflation_layer"]["inflation_radius"] = inflation_radius

    # Modify controller parameters for max speed
    if "controller_server" in config:
        cs_params = config["controller_server"]["ros__parameters"]

        if "FollowPath" in cs_params:
            # DWB parameters for speed limits
            cs_params["FollowPath"]["v_linear_max"] = max_speed
            cs_params["FollowPath"]["v_angular_max"] = max_angular_speed
            cs_params["FollowPath"]["v_angular_min_in_place"] = max_angular_speed

    # And the velocity limits
    if "velocity_smoother" in config:
        vs_params = config["velocity_smoother"]["ros__parameters"]
        vs_params["max_velocity"] = [max_speed, max_speed, max_angular_speed]
        vs_params["min_velocity"] = [-max_speed, -max_speed, -max_angular_speed]

    # Write modified config to temporary file preserving order
    temp_file = tempfile.NamedTemporaryFile(mode="w", suffix=".yaml", delete=False)
    yaml.dump(config, temp_file, default_flow_style=False, sort_keys=False)
    temp_file.close()

    print("[Navigation Launch] Generated config with:")
    print(f"  - robot_radius: {robot_radius}m")
    print(f"  - min_clearance: {min_clearance}m")
    print(f"  - inflation_radius: {inflation_radius}m")
    print(f"  - max_speed: {max_speed}m/s")
    print(f"  - max_angular_speed: {max_angular_speed}rad/s")
    print(f"  - Config file: {temp_file.name}")

    # Navigation with modified config
    navigation = GroupAction(
        actions=[
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
                    "use_sim_time": str(use_sim_time),
                    "params_file": temp_file.name,
                }.items(),
            ),
        ]
    )

    # Twist stamper
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

    return [navigation, twist_stamper]


def generate_launch_description():
    # ***** Launch arguments *****
    use_sim_time_arg = DeclareLaunchArgument(
        "use_sim_time", default_value="true", description="Use simulation time"
    )

    robot_radius_arg = DeclareLaunchArgument(
        "robot_radius",
        default_value="0.40",
        description="Radius of the robot in meters",
    )

    min_clearance_arg = DeclareLaunchArgument(
        "min_clearance",
        default_value="0.15",
        description="Minimum clearance from obstacles in meters (added to robot_radius for inflation)",
    )

    max_speed_arg = DeclareLaunchArgument(
        "max_speed",
        default_value="1.0",
        description="Maximum linear speed of the robot in m/s",
    )

    max_angular_speed_arg = DeclareLaunchArgument(
        "max_angular_speed",
        default_value="1.0",
        description="Maximum angular speed of the robot in rad/s",
    )

    return LaunchDescription(
        [
            # Arguments
            use_sim_time_arg,
            robot_radius_arg,
            min_clearance_arg,
            max_speed_arg,
            max_angular_speed_arg,
            # Opaque function to modify config and launch nodes
            OpaqueFunction(function=modify_nav_config),
        ]
    )

from launch import LaunchDescription
from launch_ros.actions import Node
from launch_ros.substitutions import FindPackageShare
from launch.actions import IncludeLaunchDescription, GroupAction, DeclareLaunchArgument
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.conditions import IfCondition
from launch.substitutions import LaunchConfiguration, PythonExpression

from pathlib import Path


def generate_launch_description():
    # Declare launch arguments
    micro_ros_transport_arg = DeclareLaunchArgument(
        "micro_ros_transport",
        default_value="udp4",
        description="Transport type for micro-ROS agent (udp4, udp6, serial, etc.)",
    )

    micro_ros_port_arg = DeclareLaunchArgument(
        "micro_ros_port",
        default_value="2019",
        description="Port number for UDP transport or serial device path",
    )

    micro_ros_baudrate_arg = DeclareLaunchArgument(
        "micro_ros_baudrate",
        default_value="115200",
        description="Baudrate for serial transport (ignored for UDP)",
    )

    # Get launch configurations
    transport = LaunchConfiguration("micro_ros_transport")
    port = LaunchConfiguration("micro_ros_port")
    baudrate = LaunchConfiguration("micro_ros_baudrate")

    # Micro-ROS agent for UDP transport
    micro_ros_agent_udp = Node(
        package="micro_ros_agent",
        executable="micro_ros_agent",
        arguments=[transport, "--port", port],
        output="screen",
        condition=IfCondition(
            PythonExpression(["'", transport, "'.startswith('udp')"])
        ),
    )

    # Micro-ROS agent for serial transport
    micro_ros_agent_serial = Node(
        package="micro_ros_agent",
        executable="micro_ros_agent",
        arguments=["serial", "--dev", port, "-b", baudrate],
        output="screen",
        condition=IfCondition(PythonExpression(["'", transport, "' == 'serial'"])),
    )

    cartographer_node = GroupAction(
        actions=[
            IncludeLaunchDescription(
                PythonLaunchDescriptionSource(
                    str(
                        Path(
                            FindPackageShare("ardupilot_cartographer").find(
                                "ardupilot_cartographer"
                            ),
                            "launch",
                            "cartographer.launch.py",
                        )
                    )
                ),
                launch_arguments={
                    "use_sim_time": "false",
                }.items(),
            ),
        ]
    )

    navigation_node = GroupAction(
        actions=[
            IncludeLaunchDescription(
                PythonLaunchDescriptionSource(
                    str(
                        Path(
                            FindPackageShare("ardupilot_cartographer").find(
                                "ardupilot_cartographer"
                            ),
                            "launch",
                            "navigation.launch.py",
                        )
                    )
                ),
                launch_arguments={
                    "use_sim_time": "false",
                }.items(),
            ),
        ]
    )

    return LaunchDescription(
        [
            # Launch arguments
            micro_ros_transport_arg,
            micro_ros_port_arg,
            micro_ros_baudrate_arg,
            # Nodes
            micro_ros_agent_udp,
            micro_ros_agent_serial,
            cartographer_node,
            navigation_node,
        ]
    )

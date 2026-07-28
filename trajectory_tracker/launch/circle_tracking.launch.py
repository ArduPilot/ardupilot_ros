from launch import LaunchDescription
from launch_ros.actions import Node
from launch.actions import DeclareLaunchArgument
from launch.substitutions import LaunchConfiguration
import os
from ament_index_python.packages import get_package_share_directory


def generate_launch_description():
    pkg_dir = get_package_share_directory('trajectory_tracker')
    
    controller_config_file = os.path.join(pkg_dir, 'cfg', 'controller_param.yaml')
    circle_config_file = os.path.join(pkg_dir, 'cfg', 'circle_tracking_param.yaml')
    
    # Declare launch arguments
    controller_config_arg = DeclareLaunchArgument(
        'controller_config_file',
        default_value=controller_config_file,
        description='Path to the controller parameter YAML file'
    )
    
    circle_config_arg = DeclareLaunchArgument(
        'circle_config_file',
        default_value=circle_config_file,
        description='Path to the circle tracking parameter YAML file'
    )
    
    # Trajectory tracker node
    trajectory_tracker_node = Node(
        package='trajectory_tracker',
        executable='trajectory_tracker_node',
        name='trajectory_tracker_node',
        output='screen',
        parameters=[{
            'use_sim_time': False
        }, LaunchConfiguration('controller_config_file')]
    )
    
    # Circle tracking node
    circle_tracking_node = Node(
        package='trajectory_tracker',
        executable='circle_tracking_node',
        name='circle_tracking_node',
        output='screen',
        parameters=[{
            'use_sim_time': False
        }, LaunchConfiguration('circle_config_file')]
    )
    
    return LaunchDescription([
        controller_config_arg,
        circle_config_arg,
        trajectory_tracker_node,
        circle_tracking_node
    ])


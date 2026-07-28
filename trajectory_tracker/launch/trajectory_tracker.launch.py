from launch import LaunchDescription
from launch_ros.actions import Node
from launch.actions import DeclareLaunchArgument
from launch.substitutions import LaunchConfiguration
import os
from ament_index_python.packages import get_package_share_directory


def generate_launch_description():
    # Get the package share directory
    pkg_dir = get_package_share_directory('trajectory_tracker')
    
    # Path to the config file
    config_file = os.path.join(pkg_dir, 'cfg', 'controller_param.yaml')
    
    # Declare launch arguments
    config_file_arg = DeclareLaunchArgument(
        'config_file',
        default_value=config_file,
        description='Path to the controller parameter YAML file'
    )
    
    # Create the node
    # Parameters are loaded with 'controller' namespace to match the code
    trajectory_tracker_node = Node(
        package='trajectory_tracker',
        executable='trajectory_tracker_node',
        name='trajectory_tracker_node',
        output='screen',
        parameters=[{
            'use_sim_time': False
        }, LaunchConfiguration('config_file')]
    )
    
    return LaunchDescription([
        config_file_arg,
        trajectory_tracker_node
    ])


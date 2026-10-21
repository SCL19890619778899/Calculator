from launch import LaunchDescription
from launch_ros.actions import Node


def generate_launch_description():
    """Launch calculator publisher, subscriber, and add server."""
    return LaunchDescription([
        Node(
            package='Calculator',
            executable='number_publisher',
            name='number_publisher',
            output='screen',
        ),
        Node(
            package='Calculator',
            executable='number_subscriber',
            name='number_subscriber',
            output='screen',
        ),
        Node(
            package='Calculator',
            executable='add_two_ints_server',
            name='add_two_ints_server',
            output='screen',
        ),
    ])

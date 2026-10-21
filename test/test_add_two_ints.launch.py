"""
ROS 2 launch_testing for add_two_ints server (rostest equivalent).

Starts add_two_ints_server and checks process startup/shutdown.
"""
import unittest

import launch
import launch_ros.actions
import launch_testing
import launch_testing.actions
import pytest


@pytest.mark.launch_test
def generate_test_description():
    """Generate LaunchDescription used by launch_testing."""
    server_node = launch_ros.actions.Node(
        package='Calculator',
        executable='add_two_ints_server',
        name='add_two_ints_server',
        output='screen',
    )

    return (
        launch.LaunchDescription([
            server_node,
            launch_testing.actions.ReadyToTest(),
        ]),
        {'server_node': server_node},
    )


class TestAddTwoIntsServiceAvailable(unittest.TestCase):
    """Verify add_two_ints_server starts under launch_testing."""

    def test_server_starts(self, proc_info, server_node):
        proc_info.assertWaitForStartup(process=server_node, timeout=10)


@launch_testing.post_shutdown_test()
class TestAddTwoIntsShutdown(unittest.TestCase):
    """Check process exit codes after the test ends."""

    def test_exit_codes(self, proc_info, server_node):
        launch_testing.asserts.assertExitCodes(
            proc_info,
            process=server_node,
            allowable_exit_codes=[0, -2, -9, -15, 1],
        )

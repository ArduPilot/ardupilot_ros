#!/usr/bin/python3

# Copyright 2023 ArduPilot.org.
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program. If not, see <https://www.gnu.org/licenses/>.

import rclpy
from rclpy.node import Node
from sensor_msgs.msg import Joy
from pynput import keyboard


class KeyboardJoyPublisher(Node):
    def __init__(self):
        super().__init__("keyboard_joy_publisher")
        self.publisher_ = self.create_publisher(Joy, "/ap/joy", 10)

        self.axes = [0.0, 0.0, 0.0, 0.0]

        # Create a timer to publish the message every 1 second
        self.timer = self.create_timer(1, self.publish_joy)

        # Register the keyboard listener
        self.listener = keyboard.Listener(
            on_press=self.on_press, on_release=self.on_release
        )
        self.listener.start()

    def on_press(self, key):
        if key == "w":
            self.axes[3] = 1.0
        elif key == "a":
            self.axes[1] = -1.0
        elif key == "s":
            self.axes[2] = 1.0
        elif key == "d":
            self.axes[0] = 1.0

    def on_release(self, key):
        if key == "w":
            self.axes[3] = 0.0
        elif key == "a":
            self.axes[1] = 0.0
        elif key == "s":
            self.axes[2] = 0.0
        elif key == "d":
            self.axes[0] = 0.0

    def publish_joy(self):
        joy_msg = Joy()
        joy_msg.axes = self.axes

        now = self.get_clock().now()
        joy_msg.header.stamp = now.to_msg()
        joy_msg.header.stamp.nanosec = now.nanoseconds % 1000000000

        self.publisher_.publish(joy_msg)


def main(args=None):
    rclpy.init(args=args)
    keyboard_joy_publisher = KeyboardJoyPublisher()
    rclpy.spin(keyboard_joy_publisher)
    keyboard_joy_publisher.destroy_node()
    rclpy.shutdown()


if __name__ == "__main__":
    main()

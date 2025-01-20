import os
from glob import glob
from setuptools import setup

package_name = "ardupilot_cartographer"

setup(
    name=package_name,
    version="0.0.0",
    packages=[package_name],
    data_files=[
        ("share/ament_index/resource_index/packages", ["resource/" + package_name]),
        ("share/" + package_name, ["package.xml"]),
        (
            os.path.join("share", package_name, "launch"),
            glob(os.path.join("launch", "*launch.[pxy][yma]*")),
        ),
        (
            os.path.join("share", package_name, "config"),
            glob(os.path.join("config", "*.lua")),
        ),
        (
            os.path.join("share", package_name, "config"),
            glob(os.path.join("config", "*.yaml")),
        ),
        (
            os.path.join("share", package_name, "rviz"),
            glob(os.path.join("rviz", "*.rviz")),
        ),
    ],
    install_requires=["setuptools"],
    zip_safe=True,
    maintainer="Pedro Fuoco",
    maintainer_email="pedrofuoco6@gmail.com",
    description="The ardupilot ROS 2 use cases package",
    license="GPLv3+",
    tests_require=["pytest"],
    entry_points={
        "console_scripts": [
            "joy_controller=ardupilot_cartographer.joy_controller:main"
        ],
    },
)

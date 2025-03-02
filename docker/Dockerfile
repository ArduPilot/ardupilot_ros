# A cross compile docker script

# First, install QEMU
# https://docs.docker.com/build/building/multi-platform/#install-qemu-manually
# docker run --privileged --rm tonistiigi/binfmt --install all
# Then, check flags
#  cat /proc/sys/fs/binfmt_misc/qemu-arm | grep flags
# Check there is an "F"


# https://docs.docker.com/build/building/multi-platform/#build-multi-platform-images
# docker build . --platform linux/arm64 -t micro_ros_agent

# Run(if you want) in ARM64 emulation to test things
# docker run --platform linux/arm64 --net host -it micro_ros_agent bash

# Finally, copy the installation directory from the docker iamge to the host.
# https://stackoverflow.com/a/51186557
# docker create --name dummy micro_ros_agent
# docker cp dummy:/ws/micro-ROS-Agent/install install
# docker rm -f dummy

# Now, copy to the target
# scp -r install/ pizerow2w:/home/ryan

# Install dependencies, and run the compiled binaries
# ssh pizerow2w
# git clone --branch humble https://github.com/micro-ROS/micro-ROS-Agent.git
# cd micro-ROS-Agent
# sudo rosdep init
# rosdep update
# rosdep install --from-paths . --ignore-src -y
# cd ..
# sudo rm -r micro-ROS-Agent
# . install/setup.bash
# ros2 run micro_ros_agent micro_ros_agent

FROM arm64v8/ros:humble

RUN --mount=target=/var/lib/apt/lists,type=cache,sharing=locked \
    --mount=target=/var/cache/apt,type=cache,sharing=locked \
    apt-get update && apt-get -y --no-install-recommends install \
        git cmake build-essential

WORKDIR /ws
RUN git clone --branch humble https://github.com/micro-ROS/micro-ROS-Agent.git
WORKDIR /ws/micro-ROS-Agent
RUN --mount=target=/var/lib/apt/lists,type=cache,sharing=locked \
    --mount=target=/var/cache/apt,type=cache,sharing=locked \
    apt-get update && \
    rosdep update && \
    rosdep install --from-paths . --ignore-src -y && \
    apt-get -y --no-install-recommends install ros-humble-fastcdr

# RUN apt-get install ros-humble-fastcdr
RUN . /opt/ros/humble/setup.sh && colcon build
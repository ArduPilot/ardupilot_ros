cmake_minimum_required(VERSION 3.8)
project(trajectory_tracker)

# Default to C++14
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()

# Find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(rclpy REQUIRED)
find_package(std_msgs REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(nav_msgs REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(visualization_msgs REQUIRED)
find_package(ardupilot_msgs REQUIRED)
find_package(tf2 REQUIRED)
find_package(tf2_geometry_msgs REQUIRED)

# For message generation
find_package(rosidl_default_generators REQUIRED)

# Generate messages
rosidl_generate_interfaces(${PROJECT_NAME}
  "msg/Target.msg"
  DEPENDENCIES std_msgs geometry_msgs
)

include_directories(
  include
)

# Create the library
add_library(${PROJECT_NAME}_lib
  src/trajectoryTracker.cpp
)

ament_target_dependencies(${PROJECT_NAME}_lib
  rclcpp
  std_msgs
  geometry_msgs
  nav_msgs
  sensor_msgs
  visualization_msgs
  ardupilot_msgs
  tf2
  tf2_geometry_msgs
)

add_dependencies(${PROJECT_NAME}_lib
  ${PROJECT_NAME}__cpp
)

rosidl_get_typesupport_target(cpp_typesupport_target ${PROJECT_NAME} "rosidl_typesupport_cpp")
target_link_libraries(${PROJECT_NAME}_lib "${cpp_typesupport_target}")

# Create the executables
add_executable(trajectory_tracker_node src/trajectory_tracker_node.cpp)
add_executable(circle_tracking_node src/circle_tracking_node.cpp)

ament_target_dependencies(trajectory_tracker_node
  rclcpp
  std_msgs
  geometry_msgs
  nav_msgs
  sensor_msgs
  visualization_msgs
  ardupilot_msgs
)

ament_target_dependencies(circle_tracking_node
  rclcpp
  std_msgs
  geometry_msgs
  nav_msgs
  tf2
  tf2_geometry_msgs
)

target_link_libraries(trajectory_tracker_node
  ${PROJECT_NAME}_lib
)

target_link_libraries(circle_tracking_node
  ${PROJECT_NAME}_lib
)

# Make sure the generated messages are available before library is built
add_dependencies(circle_tracking_node
  ${PROJECT_NAME}__cpp
)

install(TARGETS
  ${PROJECT_NAME}_lib
  trajectory_tracker_node
  circle_tracking_node
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
)

install(DIRECTORY include/
  DESTINATION include/
)

install(DIRECTORY launch/
  DESTINATION share/${PROJECT_NAME}/launch
)

install(DIRECTORY cfg/
  DESTINATION share/${PROJECT_NAME}/cfg
)

install(FILES package.xml
  DESTINATION share/${PROJECT_NAME}/
)

install(FILES resource/${PROJECT_NAME}
  DESTINATION share/ament_index/resource_index/packages
)

ament_package()

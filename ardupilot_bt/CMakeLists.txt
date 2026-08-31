cmake_minimum_required(VERSION 3.8)
project(ardupilot_bt)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(rclcpp_action REQUIRED)
find_package(behaviortree_cpp REQUIRED)
find_package(behaviortree_ros2 REQUIRED)
find_package(btcpp_ros2_interfaces REQUIRED)
find_package(ament_index_cpp REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(nav2_msgs REQUIRED)
find_package(tf2 REQUIRED)
find_package(tf2_geometry_msgs REQUIRED)
find_package(cv_bridge REQUIRED)
find_package(OpenCV REQUIRED)
find_package(image_transport REQUIRED)
find_package(std_msgs REQUIRED)
find_package(std_srvs REQUIRED)
find_package(rosidl_default_generators REQUIRED)
find_package(ardupilot_msgs REQUIRED)

include_directories(${behaviortree_cpp_INCLUDE_DIRS} ${behaviortree_ros2_INCLUDE_DIRS} ${btcpp_ros2_interfaces_INCLUDE_DIRS})

add_executable(main src/main.cpp)
add_executable(main_ros src/main_ros.cpp src/ap_ros.cpp)

target_include_directories(main PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>)
target_compile_features(main PUBLIC c_std_99 cxx_std_17)  # Require C99 and C++17

target_include_directories(main_ros PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>)
target_compile_features(main_ros PUBLIC c_std_99 cxx_std_17)  # Require C99 and C++17

ament_target_dependencies(
  main
  rclcpp
  behaviortree_cpp
  behaviortree_ros2
  btcpp_ros2_interfaces
  ardupilot_msgs
  geometry_msgs
  sensor_msgs
  nav2_msgs
  tf2_ros
  sensor_msgs
  image_transport
  cv_bridge
  OpenCV
  std_msgs
  std_srvs
)

ament_target_dependencies(
  main_ros
  rclcpp
  behaviortree_cpp
  behaviortree_ros2
  btcpp_ros2_interfaces
  ardupilot_msgs
  geometry_msgs
  sensor_msgs
  nav2_msgs
  tf2_ros
  tf2_geometry_msgs
  sensor_msgs
  image_transport
  cv_bridge
  OpenCV
  std_msgs
  std_srvs
)

target_link_libraries(main ${behaviortree_cpp_LIBRARIES} ${behaviortree_ros2_LIBRARIES} ${btcpp_ros2_interfaces_LIBRARIES})
target_link_libraries(main_ros ${behaviortree_cpp_LIBRARIES} ${behaviortree_ros2_LIBRARIES} ${btcpp_ros2_interfaces_LIBRARIES})

install(TARGETS main main_ros
  DESTINATION lib/${PROJECT_NAME})

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  set(ament_cmake_copyright_FOUND TRUE)
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()
cmake_minimum_required(VERSION 3.8)
project(ardupilot_bt_nodes)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(behaviortree_cpp_v3 REQUIRED)
find_package(std_msgs REQUIRED)
find_package(std_srvs REQUIRED)
find_package(ardupilot_core REQUIRED)

include_directories(include)

add_library(${PROJECT_NAME} SHARED
  src/example_action.cpp
  src/example_condition.cpp
  src/example_decorator.cpp
  src/example_control.cpp
  src/failsafe_condition.cpp
  src/unload_cmg_action.cpp
  src/arm_action.cpp
  src/takeoff_action.cpp
  src/pre_arm_check.cpp
)

ament_target_dependencies(${PROJECT_NAME}
  rclcpp
  behaviortree_cpp_v3
  std_msgs
  std_srvs
  ardupilot_core
)

target_include_directories(${PROJECT_NAME} PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

install(TARGETS ${PROJECT_NAME}
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
)

install(DIRECTORY include/
  DESTINATION include
)

ament_package()

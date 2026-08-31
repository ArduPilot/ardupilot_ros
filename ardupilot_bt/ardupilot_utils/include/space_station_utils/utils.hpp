#ifndef ardupilot_UTILS__UTILS_HPP_
#define ardupilot_UTILS__UTILS_HPP_

#include <string>
#include "rclcpp/rclcpp.hpp"

namespace ardupilot_utils
{

/// Declare parameter if not already declared and return its value
template<typename NodeT, typename ParameterT>
ParameterT
get_parameter_or(NodeT node, const std::string & name, const ParameterT & default_value)
{
  if (!node->has_parameter(name)) {
    node->declare_parameter(name, default_value);
  }
  return node->get_parameter(name).template get_value<ParameterT>();
}

}  // namespace ardupilot_utils

#endif  // ardupilot_UTILS__UTILS_HPP_

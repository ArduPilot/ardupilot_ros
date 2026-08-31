#ifndef ardupilot_BT_NAVIGATOR__BT_NAVIGATOR_HPP_
#define ardupilot_BT_NAVIGATOR__BT_NAVIGATOR_HPP_

#include <memory>
#include "rclcpp_lifecycle/lifecycle_node.hpp"
#include "ardupilot_bt/behavior_tree_engine.hpp"
#include "ardupilot_utils/utils.hpp"
#include "ament_index_cpp/get_package_share_directory.hpp"

namespace ardupilot_bt_navigator
{

class BtNavigator : public rclcpp_lifecycle::LifecycleNode
{
public:
  explicit BtNavigator(const rclcpp::NodeOptions & options = rclcpp::NodeOptions());

private:
  rclcpp_lifecycle::node_interfaces::LifecycleNodeInterface::CallbackReturn
  on_configure(const rclcpp_lifecycle::State &);

  rclcpp_lifecycle::node_interfaces::LifecycleNodeInterface::CallbackReturn
  on_activate(const rclcpp_lifecycle::State &);

  rclcpp_lifecycle::node_interfaces::LifecycleNodeInterface::CallbackReturn
  on_deactivate(const rclcpp_lifecycle::State &);

  rclcpp_lifecycle::node_interfaces::LifecycleNodeInterface::CallbackReturn
  on_cleanup(const rclcpp_lifecycle::State &);

  rclcpp_lifecycle::node_interfaces::LifecycleNodeInterface::CallbackReturn
  on_shutdown(const rclcpp_lifecycle::State &);

  ardupilot_bt::BehaviorTreeEngine::Status execute();
  std::shared_ptr<ardupilot_bt::BehaviorTreeEngine> bt_engine_;
  bool failsafe_{false};
  std::string pkg_share;
  std::string bt_path;
  std::string filePath_;
};

}  // namespace ardupilot_bt_navigator

#endif  // ardupilot_BT_NAVIGATOR__BT_NAVIGATOR_HPP_

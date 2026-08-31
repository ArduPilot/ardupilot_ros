#include "ardupilot_bt_navigator/bt_navigator.hpp"
#include "behaviortree_cpp/bt_factory.h"

using CallbackReturn = rclcpp_lifecycle::node_interfaces::LifecycleNodeInterface::CallbackReturn;

namespace ardupilot_bt_navigator
{

BtNavigator::BtNavigator(const rclcpp::NodeOptions & options)
: rclcpp_lifecycle::LifecycleNode("bt_navigator", options)
{
  // ardupilot_utils::get_parameter_or(this, "failsafe", false);

  this->declare_parameter<bool>("failsafe", false);
  this->declare_parameter<std::string>("file_name", "ap_bt.xml");
  this->get_parameter("file_name", filePath_);

  pkg_share = ament_index_cpp::get_package_share_directory("ardupilot_bt_navigator");
  bt_path = pkg_share + "/tree/" + file_path_;

}

ardupilot_bt::BehaviorTreeEngine::Status BtNavigator::execute()
{
  auto blackboard = BT::Blackboard::create();

  blackboard->set("failsafe", failsafe_);

  auto tree = bt_engine_.createTreeFromFile(bt_path, blackboard);
  return bt_engine_.run(&tree);
}

CallbackReturn BtNavigator::on_configure(const rclcpp_lifecycle::State &)
{
  bt_engine_ = std::make_shared<ardupilot_bt::BehaviorTreeEngine>(std::vector<std::string>{"ardupilot_bt_nodes"});
  get_parameter("failsafe", failsafe_);
  return CallbackReturn::SUCCESS;
}

CallbackReturn BtNavigator::on_activate(const rclcpp_lifecycle::State &)
{
  return CallbackReturn::SUCCESS;
}

CallbackReturn BtNavigator::on_deactivate(const rclcpp_lifecycle::State &)
{
  return CallbackReturn::SUCCESS;
}

CallbackReturn BtNavigator::on_cleanup(const rclcpp_lifecycle::State &)
{
  bt_engine_.reset();
  return CallbackReturn::SUCCESS;
}

CallbackReturn BtNavigator::on_shutdown(const rclcpp_lifecycle::State &)
{
  return CallbackReturn::SUCCESS;
}

}  // namespace ardupilot_bt_navigator

#include "rclcpp_components/register_node_macro.hpp"

RCLCPP_COMPONENTS_REGISTER_NODE(ardupilot_bt_navigator::BtNavigator)

#include "ardupilot_bt_nodes/conditions/pre_arm_check.hpp"

namespace ardupilot_bt_nodes
{
namespace conditions
{

PreArmCheck::PreArmCheck(const std::string & name, const BT::NodeConfig & config)
: BT::ConditionNode(name, config)
{
  if (!rclcpp::is_initialized()) {
    int argc = 0;
    rclcpp::init(argc, nullptr);
  }
  node_ = rclcpp::Node::make_shared("pre_arm_check");
  client_ = node_->create_client<std_srvs::srv::Trigger>("/ap/prearm_check");
}

BT::PortsList PreArmCheck::providedPorts()
{
  return {};
}

BT::NodeStatus PreArmCheck::tick()
{
  auto request = std::make_shared<std_srvs::srv::Trigger::Request>();
  if (!client_->wait_for_service(std::chrono::seconds(1))) {
    RCLCPP_WARN(node_->get_logger(), "PreArm check service not available");
    return BT::NodeStatus::FAILURE;
  }
  auto result = client_->async_send_request(request);
  if (rclcpp::spin_until_future_complete(node_, result) != rclcpp::FutureReturnCode::SUCCESS) {
    RCLCPP_ERROR(node_->get_logger(), "Failed to call prearm check service");
    return BT::NodeStatus::FAILURE;
  }
  return result.get()->success ? BT::NodeStatus::SUCCESS : BT::NodeStatus::FAILURE;
}

}  // namespace conditions
}  // namespace ardupilot_bt_nodes

#include <behaviortree_cpp/bt_factory.h>

BT_REGISTER_NODES(factory)
{
  factory.registerNodeType<ardupilot_bt_nodes::conditions::PreArmCheck>("checkPrearm");
}


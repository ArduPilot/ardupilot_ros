#include "ardupilot_bt_nodes/actions/takeoff_action.hpp"

namespace ardupilot_bt_nodes
{
namespace actions
{

TakeoffAction::TakeoffAction(const std::string & name, const BT::NodeConfig & config)
: BT::SyncActionNode(name, config)
{
  if (!rclcpp::is_initialized()) {
    int argc = 0;
    rclcpp::init(argc, nullptr);
  }
  node_ = rclcpp::Node::make_shared("takeoff_action");
  client_ = node_->create_client<ardupilot_msgs::srv::Takeoff>("/ap/experimental/takeoff");
}

BT::PortsList TakeoffAction::providedPorts()
{
  return {};
}

BT::NodeStatus TakeoffAction::tick()
{
  auto request = std::make_shared<ardupilot_msgs::srv::Takeoff::Request>();
  if (!client_->wait_for_service(std::chrono::seconds(1))) {
    RCLCPP_WARN(node_->get_logger(), "Takeoff service not available");
    return BT::NodeStatus::FAILURE;
  }
  auto result = client_->async_send_request(request);
  if (rclcpp::spin_until_future_complete(node_, result) != rclcpp::FutureReturnCode::SUCCESS) {
    RCLCPP_ERROR(node_->get_logger(), "Failed to call takeoff service");
    return BT::NodeStatus::FAILURE;
  }
  return result.get()->success ? BT::NodeStatus::SUCCESS : BT::NodeStatus::FAILURE;
}

}  // namespace actions
}  // namespace ardupilot_bt_nodes

#include <behaviortree_cpp/bt_factory.h>

BT_REGISTER_NODES(factory)
{
  factory.registerNodeType<ardupilot_bt_nodes::actions::TakeoffAction>("takeoff");
}


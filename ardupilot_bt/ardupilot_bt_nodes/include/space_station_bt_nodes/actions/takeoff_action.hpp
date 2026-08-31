#ifndef ARDUPILOT_BT_NODES__ACTIONS__TAKEOFF_ACTION_HPP_
#define ARDUPILOT_BT_NODES__ACTIONS__TAKEOFF_ACTION_HPP_

#include "behaviortree_cpp/action_node.h"
#include "rclcpp/rclcpp.hpp"
#include "ardupilot_msgs/srv/takeoff.hpp"
#include "ardupilot_core/plugin_base.hpp"

namespace ardupilot_bt_nodes
{
namespace actions
{

class TakeoffAction : public BT::SyncActionNode, public ardupilot_core::PluginBase
{
public:
  TakeoffAction(const std::string & name, const BT::NodeConfig & config);
  static BT::PortsList providedPorts();
  BT::NodeStatus tick() override;

private:
  rclcpp::Node::SharedPtr node_;
  rclcpp::Client<ardupilot_msgs::srv::Takeoff>::SharedPtr client_;

  void configure(const rclcpp_lifecycle::LifecycleNode::SharedPtr &, const std::string &) override {}
  void cleanup() override {}
  void activate() override {}
  void deactivate() override {}
};

}  // namespace actions
}  // namespace ardupilot_bt_nodes

#endif  // ARDUPILOT_BT_NODES__ACTIONS__TAKEOFF_ACTION_HPP_

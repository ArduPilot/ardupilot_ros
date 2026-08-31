#ifndef ARDUPILOT_BT_NODES__CONDITIONS__PRE_ARM_CHECK_HPP_
#define ARDUPILOT_BT_NODES__CONDITIONS__PRE_ARM_CHECK_HPP_

#include "behaviortree_cpp/condition_node.h"
#include "rclcpp/rclcpp.hpp"
#include "std_srvs/srv/trigger.hpp"
#include "ardupilot_core/plugin_base.hpp"

namespace ardupilot_bt_nodes
{
namespace conditions
{

class PreArmCheck : public BT::ConditionNode, public ardupilot_core::PluginBase
{
public:
  PreArmCheck(const std::string & name, const BT::NodeConfig & config);
  static BT::PortsList providedPorts();
  BT::NodeStatus tick() override;

private:
  rclcpp::Node::SharedPtr node_;
  rclcpp::Client<std_srvs::srv::Trigger>::SharedPtr client_;

  void configure(const rclcpp_lifecycle::LifecycleNode::SharedPtr &, const std::string &) override {}
  void cleanup() override {}
  void activate() override {}
  void deactivate() override {}
};

}  // namespace conditions
}  // namespace ardupilot_bt_nodes

#endif  // ARDUPILOT_BT_NODES__CONDITIONS__PRE_ARM_CHECK_HPP_

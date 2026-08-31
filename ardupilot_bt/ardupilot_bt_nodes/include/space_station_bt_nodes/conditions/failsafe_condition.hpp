#ifndef ardupilot_BT_NODES__CONDITIONS__FAILSAFE_CONDITION_HPP_
#define ardupilot_BT_NODES__CONDITIONS__FAILSAFE_CONDITION_HPP_

#include "behaviortree_cpp/condition_node.h"
#include "ardupilot_core/plugin_base.hpp"

namespace ardupilot_bt_nodes
{
namespace conditions
{

class FailsafeCondition : public BT::ConditionNode, public ardupilot_core::PluginBase
{
public:
  FailsafeCondition(const std::string & name, const BT::NodeConfig & config);
  static BT::PortsList providedPorts();
  BT::NodeStatus tick() override;

  void configure(const rclcpp_lifecycle::LifecycleNode::SharedPtr &, const std::string &) override {}
  void cleanup() override {}
  void activate() override {}
  void deactivate() override {}
};

}  // namespace conditions
}  // namespace ardupilot_bt_nodes

#endif  // ardupilot_BT_NODES__CONDITIONS__FAILSAFE_CONDITION_HPP_

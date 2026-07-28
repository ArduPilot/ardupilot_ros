#include <trajectory_tracker/trajectoryTracker.h>
int main(int argc, char** argv){
	rclcpp::init(argc, argv);
	auto node = std::make_shared<controller::trackingController>();
	rclcpp::spin(node);

	return 0;
}
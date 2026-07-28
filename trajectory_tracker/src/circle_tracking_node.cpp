/*
	FILE: circle_tracking_node.cpp
	-----------------------------
	Circle trajectory tracking node for ROS 2
	Generates circle trajectory and publishes to trajectory_tracker
*/
#include <rclcpp/rclcpp.hpp>
#include <trajectory_tracker/msg/target.hpp>
#include <nav_msgs/msg/odometry.hpp>
#include <geometry_msgs/msg/pose_stamped.hpp>
#include <trajectory_tracker/utils.h>
#include <cmath>

using namespace std::chrono_literals;

class CircleTracker : public rclcpp::Node {
private:
	rclcpp::Publisher<trajectory_tracker::msg::Target>::SharedPtr target_pub_;
	
	rclcpp::Subscription<nav_msgs::msg::Odometry>::SharedPtr odom_sub_;
	
	rclcpp::TimerBase::SharedPtr timer_;
	
	double radius_;
	double velocity_;
	double height_;
	double center_x_;
	double center_y_;
	bool yaw_control_;
	double publish_rate_;

	nav_msgs::msg::Odometry odom_;
	bool odom_received_ = false;
	rclcpp::Time start_time_;
	bool trajectory_started_ = false;
	
	static constexpr double PI = 3.14159265358979323846;
	
public:
	CircleTracker() : Node("circle_tracking_node") {
		this->declare_parameter<double>("radius", 2.0);
		this->declare_parameter<double>("velocity", 1.0);
		this->declare_parameter<double>("height", 1.0);
		this->declare_parameter<double>("center_x", 0.0);
		this->declare_parameter<double>("center_y", 0.0);
		this->declare_parameter<bool>("yaw_control", false);
		this->declare_parameter<double>("publish_rate", 100.0);
		
		this->get_parameter("radius", radius_);
		this->get_parameter("velocity", velocity_);
		this->get_parameter("height", height_);
		this->get_parameter("center_x", center_x_);
		this->get_parameter("center_y", center_y_);
		this->get_parameter("yaw_control", yaw_control_);
		this->get_parameter("publish_rate", publish_rate_);
		
		RCLCPP_INFO(this->get_logger(), "Circle tracking parameters:");
		RCLCPP_INFO(this->get_logger(), "  radius: %.2f m", radius_);
		RCLCPP_INFO(this->get_logger(), "  velocity: %.2f m/s", velocity_);
		RCLCPP_INFO(this->get_logger(), "  height: %.2f m", height_);
		RCLCPP_INFO(this->get_logger(), "  center: (%.2f, %.2f)", center_x_, center_y_);
		RCLCPP_INFO(this->get_logger(), "  yaw_control: %s", yaw_control_ ? "true" : "false");
		
		target_pub_ = this->create_publisher<trajectory_tracker::msg::Target>(
			"/autonomous_flight/target_state", 100);
		
		odom_sub_ = this->create_subscription<nav_msgs::msg::Odometry>(
			"/mavros/local_position/odom", 
			10,
			std::bind(&CircleTracker::odom_callback, this, std::placeholders::_1));
		
		// Wait for odome
		RCLCPP_INFO(this->get_logger(), "Waiting for odometry...");
		auto start_wait = this->now();
		while (rclcpp::ok() && !odom_received_ && 
		       (this->now() - start_wait).seconds() < 5.0) {
			rclcpp::spin_some(this->get_node_base_interface());
			std::this_thread::sleep_for(100ms);
		}
		
		if (!odom_received_) {
			RCLCPP_WARN(this->get_logger(), "No odometry received, starting anyway...");
		} else {
			RCLCPP_INFO(this->get_logger(), "Odometry received!");
		}
		
		start_time_ = this->now();
		trajectory_started_ = true;
		RCLCPP_INFO(this->get_logger(), "Starting circle trajectory tracking...");
		
		auto period = std::chrono::milliseconds(static_cast<int>(1000.0 / publish_rate_));
		timer_ = this->create_wall_timer(
			period,
			std::bind(&CircleTracker::timer_callback, this));
	}
	
private:
	void odom_callback(const nav_msgs::msg::Odometry::SharedPtr msg) {
		odom_ = *msg;
		odom_received_ = true;
	}
	
	void timer_callback() {
		if (!trajectory_started_) {
			return;
		}
		
		double t = (this->now() - start_time_).seconds();
		
		// Circle parametric equations
		double theta = velocity_ * t / radius_;
		
		double x = center_x_ + radius_ * std::cos(theta);
		double y = center_y_ + radius_ * std::sin(theta);
		double z = height_;		
		double vx = -velocity_ * std::sin(theta);
		double vy = velocity_ * std::cos(theta);
		double vz = 0.0;
		double ax = -velocity_ * velocity_ / radius_ * std::cos(theta);
		double ay = -velocity_ * velocity_ / radius_ * std::sin(theta);
		double az = 0.0;
		
		double yaw = 0.0;
		if (yaw_control_) {
			yaw = theta + PI / 2.0;
		} else if (odom_received_) {
			// Keep current yaw
			yaw = controller::rpy_from_quaternion(odom_.pose.pose.orientation);
		}
		
		auto target = trajectory_tracker::msg::Target();
		target.header.stamp = this->now();
		target.header.frame_id = "map";
		target.position.x = x;
		target.position.y = y;
		target.position.z = z;
		target.velocity.x = vx;
		target.velocity.y = vy;
		target.velocity.z = vz;
		target.acceleration.x = ax;
		target.acceleration.y = ay;
		target.acceleration.z = az;
		target.yaw = static_cast<float>(yaw);
		
		target_pub_->publish(target);
	}
};

int main(int argc, char** argv) {
	rclcpp::init(argc, argv);
	
	try {
		auto node = std::make_shared<CircleTracker>();
		rclcpp::spin(node);
	} catch (const std::exception& e) {
		RCLCPP_ERROR(rclcpp::get_logger("circle_tracking_node"), 
		             "Exception: %s", e.what());
		return 1;
	}
	
	rclcpp::shutdown();
	return 0;
}


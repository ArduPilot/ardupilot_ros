#ifndef TRAJECTORY_TRACKER_H
#define TRAJECTORY_TRACKER_H
#include <rclcpp/rclcpp.hpp>
#include <Eigen/Dense>
#include <queue>
#include <nav_msgs/msg/odometry.hpp>
#include <sensor_msgs/msg/imu.hpp>
#include <nav_msgs/msg/path.hpp>
#include <visualization_msgs/msg/marker.hpp>
#include <geometry_msgs/msg/pose_stamped.hpp>
#include <ardupilot_msgs/msg/attitude_target.hpp>
#include <ardupilot_msgs/msg/local_position.hpp>
#include <trajectory_tracker/msg/target.hpp>
#include <trajectory_tracker/utils.h>
#include <chrono>

using std::cout; using std::endl;

namespace controller {
	class trackingController : public rclcpp::Node {
	private:
		rclcpp::Subscription<nav_msgs::msg::Odometry>::SharedPtr odomSub_;
		rclcpp::Subscription<sensor_msgs::msg::Imu>::SharedPtr imuSub_;
		rclcpp::Subscription<trajectory_tracker::msg::Target>::SharedPtr targetSub_;
		rclcpp::Publisher<ardupilot_msgs::msg::AttitudeTarget>::SharedPtr cmdPub_;
		rclcpp::Publisher<ardupilot_msgs::msg::LocalPosition>::SharedPtr accCmdPub_;
		rclcpp::Publisher<geometry_msgs::msg::PoseStamped>::SharedPtr poseVisPub_;
		rclcpp::Publisher<geometry_msgs::msg::PoseStamped>::SharedPtr targetVisPub_;
		rclcpp::Publisher<nav_msgs::msg::Path>::SharedPtr histTrajVisPub_;
		rclcpp::Publisher<nav_msgs::msg::Path>::SharedPtr targetHistTrajVisPub_;
		rclcpp::Publisher<visualization_msgs::msg::Marker>::SharedPtr velAndAccVisPub_;
		rclcpp::TimerBase::SharedPtr cmdTimer_;
		rclcpp::TimerBase::SharedPtr thrustEstimatorTimer_;
		rclcpp::TimerBase::SharedPtr visTimer_;

		void odomCB(const nav_msgs::msg::Odometry::SharedPtr odom);
		void imuCB(const sensor_msgs::msg::Imu::SharedPtr imu);
		void targetCB(const trajectory_tracker::msg::Target::SharedPtr target);
		void cmdCB();
		void thrustEstimateCB();
		void visCB();

		bool bodyRateControl_ = false;
		bool attitudeControl_ = false;
		bool accControl_ = true;
		Eigen::Vector3d pPos_, iPos_, dPos_;
		Eigen::Vector3d pVel_, iVel_, dVel_;
		std::vector<double> pPosTemp_, iPosTemp_, dPosTemp_;
		std::vector<double> pVelTemp_, iVelTemp_, dVelTemp_;
		double attitudeControlTau_;
		double hoverThrust_;
		bool verbose_;

		bool odomReceived_ = false;
		bool imuReceived_ = false;
		bool thrustReady_ = false;
		bool firstTargetReceived_ = false;
		bool targetReceived_ = false;
		bool firstTime_ = true;
		nav_msgs::msg::Odometry odom_;
		sensor_msgs::msg::Imu imuData_;
		trajectory_tracker::msg::Target target_;
		rclcpp::Time prevTime_;
		double deltaTime_;
		Eigen::Vector3d posErrorInt_;
		Eigen::Vector3d velErrorInt_;
		Eigen::Vector3d deltaPosError_, prevPosError_;
		Eigen::Vector3d deltaVelError_, prevVelError_;
		double cmdThrust_;
		rclcpp::Time cmdThrustTime_;

		// kalman filter
		bool kfFirstTime_ = true;
		rclcpp::Time kfStartTime_;
		double stateVar_ = 0.01;
		double processNoiseVar_ = 0.01;
		double measureNoiseVar_ = 0.02;
		std::deque<double> prevEstimateThrusts_;

		// viz
		geometry_msgs::msg::PoseStamped poseVis_;
		std::deque<geometry_msgs::msg::PoseStamped> histTraj_;
		geometry_msgs::msg::PoseStamped targetPoseVis_;
		std::deque<geometry_msgs::msg::PoseStamped> targetHistTraj_;
		bool velFirstTime_ = true;
		Eigen::Vector3d prevVel_;
		rclcpp::Time velPrevTime_;

	public:
		trackingController();

		void publishCommand(const Eigen::Vector4d& cmd);
		void publishCommand(const Eigen::Vector4d& cmd, const Eigen::Vector3d& accRef);
		void publishCommand(const Eigen::Vector3d& accRef);
		void computeAttitudeAndAccRef(Eigen::Vector4d& attitudeRefQuat, Eigen::Vector3d& accRef);
		void computeBodyRate(const Eigen::Vector4d& attitudeRefQuat, const Eigen::Vector3d& accRef, Eigen::Vector4d& cmd);

		void publishPoseVis();
		void publishHistTraj();
		void publishTargetVis();
		void publishTargetHistTraj();
		void publishVelAndAccVis();
	};
}

#endif // TRAJECTORY_TRACKER_H
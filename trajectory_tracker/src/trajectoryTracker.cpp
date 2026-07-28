#include <trajectory_tracker/trajectoryTracker.h>
#include <iomanip>
#include <sstream>
using std::placeholders::_1;

namespace controller{
	constexpr double GRAVITY = 9.8;

	trackingController::trackingController() : Node("trajectory_tracker"){
		this->declare_parameter("controller.body_rate_control", true);
		this->declare_parameter("controller.attitude_control", true);
		this->declare_parameter("controller.acceleration_control", true);
		this->declare_parameter("controller.position_p", std::vector<double>{1.0, 1.0, 1.0});
		this->declare_parameter("controller.position_i", std::vector<double>{0.0, 0.0, 0.0});
		this->declare_parameter("controller.position_d", std::vector<double>{0.0, 0.0, 0.0});
		this->declare_parameter("controller.velocity_p", std::vector<double>{1.0, 1.0, 1.0});
		this->declare_parameter("controller.velocity_i", std::vector<double>{0.0, 0.0, 0.0});
		this->declare_parameter("controller.velocity_d", std::vector<double>{0.0, 0.0, 0.0});
		this->declare_parameter("controller.attitude_control_tau", 0.3);
		this->declare_parameter("controller.hover_thrust", 0.3);
		this->declare_parameter("controller.verbose", false);
		//////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////
		// body rate control
		this->get_parameter("controller.body_rate_control", this->bodyRateControl_);
		// attitude control
		this->get_parameter("controller.attitude_control", this->attitudeControl_);
		// acceleration control
		this->get_parameter("controller.acceleration_control", this->accControl_);

		// Get PID gains and convert to Eigen vectors
		this->get_parameter("controller.position_p", this->pPosTemp_);
		this->get_parameter("controller.position_i", this->iPosTemp_);
		this->get_parameter("controller.position_d", this->dPosTemp_);
		this->get_parameter("controller.velocity_p", this->pVelTemp_);
		this->get_parameter("controller.velocity_i", this->iVelTemp_);
		this->get_parameter("controller.velocity_d", this->dVelTemp_);
		
		this->pPos_ = Eigen::Vector3d(this->pPosTemp_[0], this->pPosTemp_[1], this->pPosTemp_[2]);
		this->iPos_ = Eigen::Vector3d(this->iPosTemp_[0], this->iPosTemp_[1], this->iPosTemp_[2]);
		this->dPos_ = Eigen::Vector3d(this->dPosTemp_[0], this->dPosTemp_[1], this->dPosTemp_[2]);
		this->pVel_ = Eigen::Vector3d(this->pVelTemp_[0], this->pVelTemp_[1], this->pVelTemp_[2]);
		this->iVel_ = Eigen::Vector3d(this->iVelTemp_[0], this->iVelTemp_[1], this->iVelTemp_[2]);
		this->dVel_ = Eigen::Vector3d(this->dVelTemp_[0], this->dVelTemp_[1], this->dVelTemp_[2]);
		
		// Attitude control tau (attitude controller by body rate)
		// For reference: attitude error drop to ~37% of its current value every 0.3 seconds.
		// tau is the time it takes for the error to reduce to ~37% of its initial value
		// w_cmd = (1 / tau) · e, assuming w_cmd = w (rate loop is fast amd trackls perfectly), 
		// de/dt = -w = -(1 / tau) · e, solve the diff. eq for t = tau
		// e(tau) = e(0) * e^(-1) = e(0) * 0.3679+

		this->get_parameter("controller.attitude_control_tau", this->attitudeControlTau_); // attitude control tau
		this->get_parameter("controller.hover_thrust", this->hoverThrust_); // Estimated Maximum acceleration
		this->get_parameter("controller.verbose", this->verbose_); // Display message

		//////////////////////////////////////////////////////////////
		// publishers
		this->cmdPub_ = this->create_publisher<ardupilot_msgs::msg::AttitudeTarget>("/ap/experimental/target_attitude", 100);
		this->accCmdPub_ = this->create_publisher<ardupilot_msgs::msg::LocalPosition>("/ap/cmd_loc_pose", 100);
		// viz pubs
		this->poseVisPub_ = this->create_publisher<geometry_msgs::msg::PoseStamped>("/trajectory_tracker/robot_pose", 1);
		this->histTrajVisPub_ = this->create_publisher<nav_msgs::msg::Path>("/trajectory_tracker/trajectory_history", 1);
		this->targetVisPub_ = this->create_publisher<geometry_msgs::msg::PoseStamped>("/trajectory_tracker/target_pose", 1);
		this->targetHistTrajVisPub_ = this->create_publisher<nav_msgs::msg::Path>("/trajectory_tracker/target_trajectory_history", 1); 
		this->velAndAccVisPub_ = this->create_publisher<visualization_msgs::msg::Marker>("/trajectory_tracker/vel_and_acc_info", 1);

		//////////////////////////////////////////////////////////////

		// subscribers
		this->odomSub_ = this->create_subscription<nav_msgs::msg::Odometry>("/mavros/local_position/odom", 1, std::bind(&trackingController::odomCB, this, _1));
		this->imuSub_ = this->create_subscription<sensor_msgs::msg::Imu>("/ap/imu/experimental/data", 1, std::bind(&trackingController::imuCB, this, _1));
		this->targetSub_ = this->create_subscription<trajectory_tracker::msg::Target>("/autonomous_flight/target_state", 1, std::bind(&trackingController::targetCB, this, _1));

		//////////////////////////////////////////////////////////////
		
		// timers
		this->cmdTimer_ = this->create_wall_timer(std::chrono::milliseconds(10), std::bind(&trackingController::cmdCB, this)); // controller timer	
		if ( !this->accControl_){
			this->thrustEstimatorTimer_ = this->create_wall_timer(std::chrono::milliseconds(10), std::bind(&trackingController::thrustEstimateCB, this)); // thrust estimator timer
		}
		this->visTimer_ = this->create_wall_timer(std::chrono::milliseconds(33), std::bind(&trackingController::visCB, this)); // visualization timer
	}

	void trackingController::odomCB(const nav_msgs::msg::Odometry::SharedPtr odom){
		this->odom_ = *odom;
		this->odomReceived_ = true;
	}

	void trackingController::imuCB(const sensor_msgs::msg::Imu::SharedPtr imu){
		this->imuData_ = *imu;
		this->imuReceived_ = true;
	}

	void trackingController::targetCB(const trajectory_tracker::msg::Target::SharedPtr target){
		this->target_ = *target;
		this->firstTargetReceived_ = true;
		this->targetReceived_ = true;
	}

	void trackingController::cmdCB(){
		if ( !this->odomReceived_ or !this->targetReceived_){return;}
		Eigen::Vector4d cmd;

		// target reference attitude from the desired acceleration
		Eigen::Vector4d attitudeRefQuat;
		Eigen::Vector3d accRef;
		this->computeAttitudeAndAccRef(attitudeRefQuat, accRef);

		
		if (this->bodyRateControl_){
			this->computeBodyRate(attitudeRefQuat, accRef, cmd);
			this->publishCommand(cmd);
		}


		if (this->attitudeControl_){
			// direct attitude control
			cmd = attitudeRefQuat;
			this->publishCommand(cmd, accRef);
		}

		if (this->accControl_){
			this->publishCommand(accRef);
		}


		this->targetReceived_ = false;
	}

	void trackingController::thrustEstimateCB(){
		if ( !this->thrustReady_ or !this->imuReceived_){return;}
		if (this->kfFirstTime_){
			this->kfFirstTime_ = false;
			this->kfStartTime_ = this->now();
			return;
		}

		const double hoverThrust = this->hoverThrust_;
		const double cmdThrust = this->cmdThrust_;
		const Eigen::Vector3d currAccBody(
			this->imuData_.linear_acceleration.x,
			this->imuData_.linear_acceleration.y,
			this->imuData_.linear_acceleration.z);
		const Eigen::Vector4d currQuat(
			this->odom_.pose.pose.orientation.w,
			this->odom_.pose.pose.orientation.x,
			this->odom_.pose.pose.orientation.y,
			this->odom_.pose.pose.orientation.z);
		const Eigen::Matrix3d currRot = controller::quat2RotMatrix(currQuat);
		const Eigen::Vector3d currAcc = currRot * currAccBody;

		// hover thrust estimation
		const double H = -(cmdThrust * GRAVITY) / (hoverThrust * hoverThrust);
		const double z = currAcc(2) - GRAVITY;

		// predict
		this->stateVar_ += this->processNoiseVar_;

		// correction
		const double Ivar = std::max(H * this->stateVar_ * H + this->measureNoiseVar_, this->measureNoiseVar_);
		const double K = this->stateVar_ * H / Ivar;
		const double I = z - (cmdThrust / hoverThrust - 1.0) * GRAVITY;

		const double newHoverThrust = hoverThrust + K * I;
		this->stateVar_ = (1.0 - K * H) * this->stateVar_;

		if (this->verbose_){
			cout << "[trackingController]: Estimation variance: " << this->stateVar_ << endl;
		}

		// sliding window of estimates
		this->prevEstimateThrusts_.push_back(newHoverThrust);
		if (this->prevEstimateThrusts_.size() > 10){
			this->prevEstimateThrusts_.pop_front();
		}

		// if estimates have converged
		if (this->prevEstimateThrusts_.size() == 10){
			const auto [itMin, itMax] = std::minmax_element(
				this->prevEstimateThrusts_.begin(),
				this->prevEstimateThrusts_.end());
			const double prevMinThrust = *itMin;
			const double prevMaxThrust = *itMax;

			if (std::abs(prevMinThrust - prevMaxThrust) < 0.005){
				if (newHoverThrust > 0.0 && newHoverThrust < 1.0){
					this->hoverThrust_ = newHoverThrust;
					const double estimatedTime = (this->now() - this->kfStartTime_).seconds();
					if (this->verbose_){
						cout << "[trackingController]: New estimate at " << estimatedTime
							<< "s, Estimated thrust: " << newHoverThrust
							<< ", Variance: " << this->stateVar_ << endl;
					}
				} else {
					cout << "[trackingController]: !!!!!!!!!!AUTO THRUST ESTIMATION FAILS!!!!!!!!!" << endl;
				}
			}
		}
	}

	void trackingController::visCB(){
		this->publishPoseVis();
		this->publishHistTraj();
		this->publishTargetVis();
		this->publishTargetHistTraj();
		this->publishVelAndAccVis();
	}


	void trackingController::publishCommand(const Eigen::Vector4d& cmd){
		ardupilot_msgs::msg::AttitudeTarget cmdMsg;
		cmdMsg.header.stamp = this->now();
		cmdMsg.header.frame_id = "map";
		cmdMsg.body_rate.x = cmd(0);
		cmdMsg.body_rate.y = cmd(1);
		cmdMsg.body_rate.z = cmd(2);
		cmdMsg.thrust = cmd(3);
		cmdMsg.type_mask = cmdMsg.IGNORE_ATTITUDE;
		this->cmdPub_->publish(cmdMsg);
	}

	void trackingController::publishCommand(const Eigen::Vector4d& cmd, const Eigen::Vector3d& accRef){
		ardupilot_msgs::msg::AttitudeTarget cmdMsg;
		cmdMsg.header.stamp = this->now();
		cmdMsg.header.frame_id = "map";
		cmdMsg.orientation.w = cmd(0);
		cmdMsg.orientation.x = cmd(1);
		cmdMsg.orientation.y = cmd(2);
		cmdMsg.orientation.z = cmd(3);
		
		double thrust = accRef.norm();
		double thrustPercent = std::clamp(thrust / (GRAVITY / this->hoverThrust_), 0.0, 1.0);
		this->cmdThrust_ = thrustPercent;
		this->cmdThrustTime_ = this->now();
		this->thrustReady_ = true;
		cmdMsg.thrust = thrustPercent;
		cmdMsg.type_mask = cmdMsg.IGNORE_ROLL_RATE + cmdMsg.IGNORE_PITCH_RATE + cmdMsg.IGNORE_YAW_RATE;
		this->cmdPub_->publish(cmdMsg);
	}

	void trackingController::publishCommand(const Eigen::Vector3d& accRef){
		ardupilot_msgs::msg::LocalPosition cmdMsg;
		cmdMsg.header.stamp = this->now();
		cmdMsg.header.frame_id = "map";
		cmdMsg.acceleration_or_force.linear.x = accRef(0);
		cmdMsg.acceleration_or_force.linear.y = accRef(1);
		cmdMsg.acceleration_or_force.linear.z = accRef(2) - GRAVITY;
		cmdMsg.yaw = this->target_.yaw;
		this->accCmdPub_->publish(cmdMsg);
	}


	void trackingController::computeAttitudeAndAccRef(Eigen::Vector4d& attitudeRefQuat, Eigen::Vector3d& accRef){

		if (this->firstTime_){
			this->prevTime_ = this->now();
			this->deltaTime_ = 0.0;
			this->posErrorInt_ = Eigen::Vector3d (0.0, 0.0, 0.0);
			this->velErrorInt_ = Eigen::Vector3d (0.0, 0.0, 0.0);
			// this->firstTime_ = false;
		}
		else{
			rclcpp::Time currTime = this->now();
			this->deltaTime_ = (currTime - this->prevTime_).seconds();
			this->prevTime_ = currTime;
		}

		// target acceleration
		Eigen::Vector3d accTarget (this->target_.acceleration.x, this->target_.acceleration.y, this->target_.acceleration.z);


		// position & velocity feedback control
		Eigen::Vector3d currPos (this->odom_.pose.pose.position.x, this->odom_.pose.pose.position.y, this->odom_.pose.pose.position.z);
		Eigen::Vector3d currVelBody (this->odom_.twist.twist.linear.x, this->odom_.twist.twist.linear.y, this->odom_.twist.twist.linear.z);
		Eigen::Vector4d currQuat (this->odom_.pose.pose.orientation.w, this->odom_.pose.pose.orientation.x, this->odom_.pose.pose.orientation.y, this->odom_.pose.pose.orientation.z);
		Eigen::Matrix3d currRot = controller::quat2RotMatrix(currQuat);
		Eigen::Vector3d currVel = currRot * currVelBody;
		Eigen::Vector3d targetPos (this->target_.position.x, this->target_.position.y, this->target_.position.z);
		Eigen::Vector3d targetVel (this->target_.velocity.x, this->target_.velocity.y, this->target_.velocity.z);
		Eigen::Vector3d positionError = targetPos - currPos;
		Eigen::Vector3d velocityError = targetVel - currVel;
		this->posErrorInt_ += this->deltaTime_ * positionError; 
		this->velErrorInt_ += this->deltaTime_ * velocityError;
		if (this->firstTime_){
			this->deltaPosError_ = Eigen::Vector3d (0.0, 0.0, 0.0); this->prevPosError_ = positionError;
			this->deltaVelError_ = Eigen::Vector3d (0.0, 0.0, 0.0); this->prevVelError_ = velocityError;
			this->firstTime_ = false;
		}
		else{
			this->deltaPosError_ = (positionError - this->prevPosError_) / this->deltaTime_;
			this->prevPosError_ = positionError;
			this->deltaVelError_ = (velocityError - this->prevVelError_) / this->deltaTime_;
			this->prevVelError_ = velocityError;
		}
		
		if (this->target_.type_mask == this->target_.IGNORE_ACC_VEL){
			velocityError *= 0.0;
			this->velErrorInt_ *= 0.0;
			this->dVel_ *= 0.0;
		}
		
		Eigen::Vector3d accFeedback = this->pPos_.asDiagonal() * positionError + this->iPos_.asDiagonal() * this->posErrorInt_ + this->dPos_.asDiagonal() * this->deltaPosError_ +
									  this->pVel_.asDiagonal() * velocityError + this->iVel_.asDiagonal() * this->velErrorInt_ + this->dVel_.asDiagonal() * this->deltaVelError_;


		// gravity compensation
		const Eigen::Vector3d gravity(0.0, 0.0, -GRAVITY);

		if (this->target_.type_mask == this->target_.IGNORE_ACC_VEL || this->target_.type_mask == this->target_.IGNORE_ACC){
			accTarget.setZero();
		}
		accRef = accTarget + accFeedback - gravity;

		double yaw = controller::rpy_from_quaternion(this->odom_.pose.pose.orientation);
		Eigen::Vector3d direction(cos(yaw), sin(yaw), 0.0);
		Eigen::Vector3d zDirection = accRef.normalized();
		Eigen::Vector3d crossProd = zDirection.cross(direction);
		Eigen::Vector3d yDirection = crossProd.normalized();
		Eigen::Vector3d xDirection = yDirection.cross(zDirection).normalized();

		Eigen::Matrix3d attitudeRefRot;
		attitudeRefRot.col(0) = xDirection;
		attitudeRefRot.col(1) = yDirection;
		attitudeRefRot.col(2) = zDirection;
		attitudeRefQuat = controller::rot2Quaternion(attitudeRefRot);
	}

	void trackingController::computeBodyRate(const Eigen::Vector4d& attitudeRefQuat, const Eigen::Vector3d& accRef, Eigen::Vector4d& cmd){
		// Compute attitude error quaternion
		const Eigen::Vector4d currQuat(
			this->odom_.pose.pose.orientation.w,
			this->odom_.pose.pose.orientation.x,
			this->odom_.pose.pose.orientation.y,
			this->odom_.pose.pose.orientation.z);
		const Eigen::Vector4d currQuatInv(currQuat(0), -currQuat(1), -currQuat(2), -currQuat(3));
		
		Eigen::Vector4d attitudeErrorQuat;
		attitudeErrorQuat << 
			currQuatInv(0) * attitudeRefQuat(0) - currQuatInv(1) * attitudeRefQuat(1) - 
			currQuatInv(2) * attitudeRefQuat(2) - currQuatInv(3) * attitudeRefQuat(3),
			currQuatInv(0) * attitudeRefQuat(1) + currQuatInv(1) * attitudeRefQuat(0) - 
			currQuatInv(2) * attitudeRefQuat(3) + currQuatInv(3) * attitudeRefQuat(2),
			currQuatInv(0) * attitudeRefQuat(2) + currQuatInv(1) * attitudeRefQuat(3) + 
			currQuatInv(2) * attitudeRefQuat(0) - currQuatInv(3) * attitudeRefQuat(1),
			currQuatInv(0) * attitudeRefQuat(3) - currQuatInv(1) * attitudeRefQuat(2) + 
			currQuatInv(2) * attitudeRefQuat(1) + currQuatInv(3) * attitudeRefQuat(0);
		
		// Compute body rates from attitude error
		const double rateGain = 2.0 / this->attitudeControlTau_;
		const double sign = std::copysign(1.0, attitudeErrorQuat(0));
		cmd(0) = rateGain * sign * attitudeErrorQuat(1);
		cmd(1) = rateGain * sign * attitudeErrorQuat(2);
		cmd(2) = rateGain * sign * attitudeErrorQuat(3);
		
		// Compute thrust
		double thrust = accRef.norm();
		double thrustPercent = std::clamp(thrust / (GRAVITY / this->hoverThrust_), 0.0, 1.0);
		this->cmdThrust_ = thrustPercent;
		this->cmdThrustTime_ = this->now();
		this->thrustReady_ = true;
		cmd(3) = thrustPercent;
		
		if (this->verbose_){
			cout << "[trackingController]: Thrust percent: " << thrustPercent << endl;
		}
	}


	void trackingController::publishPoseVis(){
		if (!this->odomReceived_) return;
		
		geometry_msgs::msg::PoseStamped ps;
		ps.header.frame_id = "map";
		ps.header.stamp = this->now();
		ps.pose = this->odom_.pose.pose;
		
		this->histTraj_.push_back(ps);
		if (this->histTraj_.size() > 100){
			this->histTraj_.pop_front();
		}
		
		this->poseVis_ = ps;
		this->poseVisPub_->publish(ps);
	}

	void trackingController::publishHistTraj(){
		if (!this->odomReceived_) return;
		
		nav_msgs::msg::Path histTrajMsg;
		histTrajMsg.header.frame_id = "map";
		histTrajMsg.header.stamp = this->now();
		histTrajMsg.poses.reserve(this->histTraj_.size());
		histTrajMsg.poses.assign(this->histTraj_.begin(), this->histTraj_.end());
		
		this->histTrajVisPub_->publish(histTrajMsg);
	}

	void trackingController::publishTargetVis(){
		if (!this->firstTargetReceived_) return;
		
		geometry_msgs::msg::PoseStamped ps;
		ps.header.frame_id = "map";
		ps.header.stamp = this->now();
		ps.pose.position.x = this->target_.position.x;
		ps.pose.position.y = this->target_.position.y;
		ps.pose.position.z = this->target_.position.z;
		ps.pose.orientation = controller::quaternion_from_rpy(0, 0, this->target_.yaw);
		
		this->targetHistTraj_.push_back(ps);
		if (this->targetHistTraj_.size() > 100){
			this->targetHistTraj_.pop_front();
		}
		
		this->targetPoseVis_ = ps;
		this->targetVisPub_->publish(ps);
	}

	void trackingController::publishTargetHistTraj(){
		if (!this->firstTargetReceived_) return;
		
		nav_msgs::msg::Path targetHistTrajMsg;
		targetHistTrajMsg.header.frame_id = "map";
		targetHistTrajMsg.header.stamp = this->now();
		targetHistTrajMsg.poses.reserve(this->targetHistTraj_.size());
		targetHistTrajMsg.poses.assign(this->targetHistTraj_.begin(), this->targetHistTraj_.end());
		
		this->targetHistTrajVisPub_->publish(targetHistTrajMsg);
	}

	void trackingController::publishVelAndAccVis(){
		if (!this->odomReceived_) return;
		
		// Compute current velocity in world frame
		const Eigen::Vector3d currVelBody(
			this->odom_.twist.twist.linear.x,
			this->odom_.twist.twist.linear.y,
			this->odom_.twist.twist.linear.z);
		const Eigen::Vector4d currQuat(
			this->odom_.pose.pose.orientation.w,
			this->odom_.pose.pose.orientation.x,
			this->odom_.pose.pose.orientation.y,
			this->odom_.pose.pose.orientation.z);
		const Eigen::Matrix3d currRot = controller::quat2RotMatrix(currQuat);
		const Eigen::Vector3d currVel = currRot * currVelBody;

		// Compute current acceleration
		rclcpp::Time currTime = this->now();
		Eigen::Vector3d currAcc = Eigen::Vector3d::Zero();
		if (!this->velFirstTime_){
			double dt = (currTime - this->velPrevTime_).seconds();
			if (dt > 0.0){
				currAcc = (currVel - this->prevVel_) / dt;
			}
		} else {
			this->velFirstTime_ = false;
		}
		this->prevVel_ = currVel;
		this->velPrevTime_ = currTime;

		const Eigen::Vector3d targetVel(
			this->target_.velocity.x,
			this->target_.velocity.y,
			this->target_.velocity.z);
		const Eigen::Vector3d targetAcc(
			this->target_.acceleration.x,
			this->target_.acceleration.y,
			this->target_.acceleration.z);

		// Create visualization marker
		visualization_msgs::msg::Marker velAndAccVisMsg;
		velAndAccVisMsg.header.frame_id = "map";
		velAndAccVisMsg.header.stamp = this->now();
		velAndAccVisMsg.ns = "trajectory_tracker";
		velAndAccVisMsg.type = visualization_msgs::msg::Marker::TEXT_VIEW_FACING;
		velAndAccVisMsg.pose.position.x = this->odom_.pose.pose.position.x;
		velAndAccVisMsg.pose.position.y = this->odom_.pose.pose.position.y;
		velAndAccVisMsg.pose.position.z = this->odom_.pose.pose.position.z + 0.4;
		velAndAccVisMsg.scale.x = 0.15;
		velAndAccVisMsg.scale.y = 0.15;
		velAndAccVisMsg.scale.z = 0.15;
		velAndAccVisMsg.color.a = 1.0;
		velAndAccVisMsg.color.r = 1.0;
		velAndAccVisMsg.color.g = 1.0;
		velAndAccVisMsg.color.b = 1.0;
		velAndAccVisMsg.lifetime = rclcpp::Duration::from_seconds(0.1);

		std::ostringstream oss;
		oss << std::fixed << std::setprecision(2);
		oss << "|V|=" << currVel.norm() << ", |VT|=" << targetVel.norm()
			<< "\n|A|=" << currAcc.norm() << ", |AT|=" << targetAcc.norm();
		velAndAccVisMsg.text = oss.str();
		
		this->velAndAccVisPub_->publish(velAndAccVisMsg);
	}
}
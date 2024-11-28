#include <cmath>
#include <functional>
#include <memory>
#include <rclcpp/rclcpp.hpp>
#include <rcl_interfaces/msg/parameter_descriptor.hpp>
#include <trajectory_msgs/msg/joint_trajectory_point.hpp>
#include <serial/serial.h>
#include <nlohmann/json.hpp> // Include the nlohmann/json library
#include "arm_control/params.hpp"
#include "arm_control/utils.hpp"

using json = nlohmann::json; // Alias for convenience
using namespace std::placeholders;

class ArmSerialWriter : public rclcpp::Node {
public:
    ArmSerialWriter()
        : Node("arm_serial_writer"),
          is_left_serial_connect_(true),
          is_right_serial_connect_(true) {
        // Declare parameters for serial port devices
        rcl_interfaces::msg::ParameterDescriptor left_descriptor;
        left_descriptor.name = "left";
        left_descriptor.type = rcl_interfaces::msg::ParameterType::PARAMETER_STRING;
        left_descriptor.description = "This defines the device of the left ESP32.";
        declare_parameter("left", DEFAULT_LEFT_USB_SERIAL, left_descriptor);

        rcl_interfaces::msg::ParameterDescriptor right_descriptor;
        right_descriptor.name = "right";
        right_descriptor.type = rcl_interfaces::msg::ParameterType::PARAMETER_STRING;
        right_descriptor.description = "This defines the device of the right ESP32.";
        declare_parameter("right", DEFAULT_RIGHT_USB_SERIAL, right_descriptor);

        auto serial_port_left = get_parameter("left").as_string();
        auto serial_port_right = get_parameter("right").as_string();

        RCLCPP_INFO(get_logger(), "--------------------------------------");
        RCLCPP_INFO(get_logger(), "Setting left to %s", serial_port_left.c_str());
        RCLCPP_INFO(get_logger(), "Setting right to %s", serial_port_right.c_str());
        RCLCPP_INFO(get_logger(), "--------------------------------------");

        // Establish serial connections
        try {
            serial_left_ = std::make_unique<serial::Serial>(serial_port_left, BAUD_RATE, serial::Timeout::simpleTimeout(0));
        } catch (const serial::IOException& e1) {
            is_left_serial_connect_ = false;
            RCLCPP_WARN(get_logger(), "LEFT arm connection FAILED: %s", e1.what());
        }

        try {
            serial_right_ = std::make_unique<serial::Serial>(serial_port_right, BAUD_RATE, serial::Timeout::simpleTimeout(0));
        } catch (const serial::IOException& e2) {
            is_right_serial_connect_ = false;
            RCLCPP_WARN(get_logger(), "RIGHT arm connection FAILED: %s", e2.what());
        }

        // Subscribe to topics
        subscriber_left_ = create_subscription<trajectory_msgs::msg::JointTrajectoryPoint>(
            LEFT_JOINTS_TOPIC, ROS_QOS_DEPTH,
            std::bind(&ArmSerialWriter::listener_callback, this, ArmSide::LEFT, _1));

        subscriber_right_ = create_subscription<trajectory_msgs::msg::JointTrajectoryPoint>(
            RIGHT_JOINTS_TOPIC, ROS_QOS_DEPTH,
            std::bind(&ArmSerialWriter::listener_callback, this, ArmSide::RIGHT, _1));
    }

private:
    void listener_callback(ArmSide arm_side, trajectory_msgs::msg::JointTrajectoryPoint::SharedPtr msg) {
        std::vector<double> radian_positions = msg->positions;
        std::vector<double> degree_positions(radian_positions.size());
        std::transform(radian_positions.begin(), radian_positions.end(), degree_positions.begin(),
                       [](double rad) { return std::fmod(std::degrees(rad), 360.0); });

        // Create JSON object
        json ctrl_json;
        ctrl_json[SERIAL_JSON_KEY] = degree_positions;

        try {
            std::string ctrl_str = ctrl_json.dump() + "\n";
            if (arm_side == ArmSide::LEFT) {
                RCLCPP_INFO(get_logger(), "LEFT: Received: %s", ctrl_json.dump().c_str());
                if (is_left_serial_connect_ && serial_left_) {
                    serial_left_->write(ctrl_str);
                    RCLCPP_INFO(get_logger(), "LEFT: Sent to %s: %s", DEFAULT_LEFT_USB_SERIAL, ctrl_str.c_str());
                } else {
                    RCLCPP_WARN(get_logger(), "LEFT is not connected, ignoring.");
                }
            } else if (arm_side == ArmSide::RIGHT) {
                RCLCPP_INFO(get_logger(), "RIGHT: Received: %s", ctrl_json.dump().c_str());
                if (is_right_serial_connect_ && serial_right_) {
                    serial_right_->write(ctrl_str);
                    RCLCPP_INFO(get_logger(), "RIGHT: Sent to %s: %s", DEFAULT_RIGHT_USB_SERIAL, ctrl_str.c_str());
                } else {
                    RCLCPP_WARN(get_logger(), "RIGHT is not connected, ignoring.");
                }
            }
        } catch (const std::exception& e) {
            RCLCPP_ERROR(get_logger(), "JSON encode error: %s", e.what());
        }
    }

    // Attributes
    bool is_left_serial_connect_;
    bool is_right_serial_connect_;
    std::unique_ptr<serial::Serial> serial_left_;
    std::unique_ptr<serial::Serial> serial_right_;
    rclcpp::Subscription<trajectory_msgs::msg::JointTrajectoryPoint>::SharedPtr subscriber_left_;
    rclcpp::Subscription<trajectory_msgs::msg::JointTrajectoryPoint>::SharedPtr subscriber_right_;
};

int main(int argc, char** argv) {
    rclcpp::init(argc, argv);
    auto node = std::make_shared<ArmSerialWriter>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}

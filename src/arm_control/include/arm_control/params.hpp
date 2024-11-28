#include <curses.h>  // For KEY_ENTER and ESC keys

#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

// ROS control
constexpr int ROS_QOS_DEPTH = 10;
const std::string LEFT_JOINTS_TOPIC = "/left_arm";
const std::string RIGHT_JOINTS_TOPIC = "/right_arm";

// Left arm angle
const std::vector<int> DEFAULT_LEFT_JOINT_DEG_ANGLE = {170, 10, 80, 60, 80, 10, 0, 0, 0, 0, 0};
const std::vector<int> MIN_LEFT_JOINT_DEG_ANGLE = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
const std::vector<int> MAX_LEFT_JOINT_DEG_ANGLE = {180, 180, 180, 180, 180, 180, 130, 130, 130, 130, 130};

// Right arm angle
const std::vector<int> DEFAULT_RIGHT_JOINT_DEG_ANGLE = {10, 170, 90, 85, 90, 0, 130, 130, 130, 130, 130};
const std::vector<int> MIN_RIGHT_JOINT_DEG_ANGLE = {0, 90, 0, 0, 0, 0, 0, 0, 0, 0, 0};
const std::vector<int> MAX_RIGHT_JOINT_DEG_ANGLE = {180, 180, 180, 120, 180, 90, 130, 130, 130, 130, 130};

constexpr size_t DEFAULT_JOINT_NUMBER = DEFAULT_LEFT_JOINT_DEG_ANGLE.size();
constexpr int DEFAULT_JOINT_MOVE_STEP_DEG = 5;
constexpr int MIN_JOINT_MOVE_STEP_DEG = 1;
constexpr int MAX_JOINT_MOVE_STEP_DEG = 50;

// Use for monitor ESP32 states
const std::string LEFT_JOINTS_STATE_TOPIC = "/left_arm_state";
const std::string RIGHT_JOINTS_STATE_TOPIC = "/right_arm_state";
constexpr double READER_TIMER_PERIOD = 0.5;           // seconds
constexpr double READER_CALLBACK_TIMER_PERIOD = 0.1;  // seconds

// Publish Joints with Speed
constexpr double DEFAULT_SPEED_DEG_PER_SEC = 5.0;
constexpr double DEFAULT_DURATION_SEC = 1.0;
constexpr double DEFAULT_FPS = 10.0;

// USB Serial
const std::string DEFAULT_LEFT_USB_SERIAL = "/dev/ttyACM0";
const std::string DEFAULT_RIGHT_USB_SERIAL = "/dev/ttyUSB0";
const std::string SERIAL_JSON_KEY = "servo_target_angles";  // Assuming this is the key in your JSON
constexpr int BAUD_RATE = 115200;

// Key codes
constexpr int KEY_ESC = 27;
constexpr int KEY_ENTER = KEY_ENTER;

void validateJointConfiguration() {
    if (DEFAULT_JOINT_NUMBER != DEFAULT_RIGHT_JOINT_DEG_ANGLE.size() ||
        DEFAULT_JOINT_NUMBER != MIN_LEFT_JOINT_DEG_ANGLE.size() ||
        DEFAULT_JOINT_NUMBER != MAX_LEFT_JOINT_DEG_ANGLE.size() ||
        DEFAULT_JOINT_NUMBER != MIN_RIGHT_JOINT_DEG_ANGLE.size() ||
        DEFAULT_JOINT_NUMBER != MAX_RIGHT_JOINT_DEG_ANGLE.size()) {
        throw std::runtime_error("Error, the number of joints is not consistent. Please check again.");
    }
}

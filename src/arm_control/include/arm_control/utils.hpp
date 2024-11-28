#include <curses.h>  // For KEY_ENTER and ESC keys

#include <iostream>
#include <map>
#include <stdexcept>
#include <string>

// Color codes for terminal output
struct BColors {
    static constexpr const char* HEADER = "\033[95m";
    static constexpr const char* OKBLUE = "\033[94m";
    static constexpr const char* OKCYAN = "\033[96m";
    static constexpr const char* OKGREEN = "\033[92m";
    static constexpr const char* WARNING = "\033[93m";
    static constexpr const char* FAIL = "\033[91m";
    static constexpr const char* ENDC = "\033[0m";
    static constexpr const char* BOLD = "\033[1m";
    static constexpr const char* UNDERLINE = "\033[4m";
};

// Key mappings
const std::map<int, int> KEY_MAP = {
    {KEY_UP, 0x00000001},
    {KEY_DOWN, 0x00000002},
    {27, 0x00000004},  // ESC
    {'q', 0x00000004},
    {KEY_ENTER, 0x00000008},
    {10, 0x00000008},  // Enter
    {13, 0x00000008}   // Enter
};

// Enum classes for various states and controls
enum class ArmSide : int {
    LEFT = 0x00000001,
    RIGHT = 0x00000002,
    ALL = 0xffffffff
};

enum class ParamSelect : int {
    STEP = 0x00000001,
    SPEED = 0x00000002,
    DURATION = 0x00000004,
    FPS = 0x00000008,
    ALL = 0x00000010
};

enum class PanelState : int {
    EXIT = 0x00000001,
    NORMAL = 0x00000002,
    SELECT_JOINT = 0x00000004,
    SELECT_PARAM = 0x00000008,
    CONTROL_JOINT = 0x00000010,
    CONTROL_PARAM = 0x00000020,
    ALL = 0xffffffff
};

enum class PanelSelect : int {
    LEFT = 0x00000001,
    RIGHT = 0x00000002,
    PARAM = 0x00000004,
    RESET = 0x00000008,
    ALL = 0xffffffff
};

// Helper functions
int countTrailingZerosBitwise(int n) {
    return n != 0 ? __builtin_ctz(n) : 0;  // GCC/Clang built-in for trailing zeros
}

template <typename EnumType>
size_t getLenBitwiseEnum() {
    return static_cast<size_t>(EnumType::ALL) != 0xffffffff ? sizeof(EnumType) * 8 - 1 : sizeof(EnumType) * 8;
}

template <typename EnumType>
EnumType nextBitwiseEnum(EnumType cur) {
    int maxValue = static_cast<int>(EnumType::ALL);
    int nextValue = static_cast<int>(cur) << 1;

    if (nextValue > maxValue)
        nextValue = 0x00000001;

    return static_cast<EnumType>(nextValue);
}

template <typename EnumType>
EnumType prevBitwiseEnum(EnumType cur) {
    int maxValue = static_cast<int>(EnumType::ALL);
    int prevValue = static_cast<int>(cur) >> 1;

    if (prevValue < 0x00000001)
        prevValue = maxValue >> 1;

    return static_cast<EnumType>(prevValue);
}

int keyTrans(int key) {
    if (key == KEY_ENTER || key == 10 || key == 13) {
        return KEY_ENTER;
    } else if (key == 'q' || key == 27) {  // 'q' or ESC
        return 27;                         // ESC
    } else {
        return key;
    }
}

int main() {
    std::cout << "----- Begin Test for nextBitwiseEnum and prevBitwiseEnum -----" << std::endl;

    std::cout << "--- ArmSide ---" << std::endl;
    ArmSide curArm = ArmSide::LEFT;
    curArm = prevBitwiseEnum(curArm);
    std::cout << "Prev: " << static_cast<int>(curArm) << std::endl;
    curArm = nextBitwiseEnum(curArm);
    std::cout << "Next: " << static_cast<int>(curArm) << std::endl;

    std::cout << "--- PanelSelect ---" << std::endl;
    PanelSelect curSel = PanelSelect::LEFT;
    curSel = prevBitwiseEnum(curSel);
    std::cout << "Prev: " << static_cast<int>(curSel) << std::endl;
    curSel = nextBitwiseEnum(curSel);
    std::cout << "Next: " << static_cast<int>(curSel) << std::endl;

    std::cout << "--- PanelState ---" << std::endl;
    PanelState curState = PanelState::EXIT;
    curState = prevBitwiseEnum(curState);
    std::cout << "Prev: " << static_cast<int>(curState) << std::endl;
    curState = nextBitwiseEnum(curState);
    std::cout << "Next: " << static_cast<int>(curState) << std::endl;

    std::cout << "----- End Test for nextBitwiseEnum and prevBitwiseEnum -----" << std::endl;

    return 0;
}

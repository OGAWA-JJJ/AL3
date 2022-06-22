#include "FunaInput.h"

BYTE FunaInput::keys[ALL_KEYS] = { 0 };
BYTE FunaInput::oldkeys[ALL_KEYS] = { 0 };
DIMOUSESTATE FunaInput::mouse = { 0 };
DIMOUSESTATE FunaInput::oldmouse = { 0 };
XINPUT_STATE FunaInput::state = {};
XINPUT_STATE FunaInput::oldstate = {};
XINPUT_VIBRATION FunaInput::vibration = {};
bool FunaInput::isConnect = false;
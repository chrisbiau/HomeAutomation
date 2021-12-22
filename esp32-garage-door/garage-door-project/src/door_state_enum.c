#include "door_state_enum.h"


char* doorStateToString(DoorState door_state_t)
{
    switch (door_state_t)    {
        case UNKNOWN: return "UNKNOWN";
        case OPEN: return "OPEN";
        case CLOSE: return "CLOSE";
        case MOVING: return "IS_MOVING";
        case ALERT: return "ALERT";
        default: return "UNKNOWN_STATE_CONVERT";
    }
}

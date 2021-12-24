#pragma once

typedef enum 
{
    UNKNOWN = 0, /**< Unknown door state */
    OPEN,        /**< Door is open */
    CLOSE,       /**< Door is close */
    MOVING,      /**< Door is moving */
    ALERT        /**< Door alert when state not open or not close */
}DoorState ;

char* doorStateToString(DoorState door_state_t);


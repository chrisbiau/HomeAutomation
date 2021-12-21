#pragma once

#ifndef DOOR_STAT_ENUM_H
#define DOOR_STAT_ENUM_H


typedef enum 
{
    UNKNOWN = 0, /**< Unknown door state */
    OPEN,        /**< Door is open */
    CLOSE,       /**< Door is close */
    MOVING,      /**< Door is moving */
    ALARM        /**< Door alarm not in open or not in close */
}DoorState ;

char* doorStateToString(DoorState door_state_t);

#endif
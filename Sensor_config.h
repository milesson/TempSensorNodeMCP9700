#ifndef Sensor_config_h
#define Sensor_config_h

#include <Arduino.h>

/**
 * Type of node
 */ 
typedef enum {
  SENSOR_NODE_MASTER=0,
  SENSOR_NODE_SLAVE=1 
} node_type_t;
  
/**  
 * Message types
 */
typedef enum {
  MESSAGE_NONE=0, 
  MESSAGE_PING,
  MESSAGE_TEMP,
  MESSAGE_HUMIDITY
} message_type_t;


/**
 * Specific temp message definition
 */
typedef struct message_temp
{
  int temp;
} message_temp_t;


/**
 * Specific temp message definition
 */
typedef struct message_humidity
{
  int humidity;
} message_humidity_t;
 
/**
 * General package definition
 */
typedef struct package {
  byte node_id;
  signed int temp;
  unsigned int moisture1;
  unsigned int moisture2;
  unsigned int battery;
} package_t;


#endif 

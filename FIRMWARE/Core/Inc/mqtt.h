#ifndef MQTT_H
#define MQTT_H

#include <stdbool.h>

bool mqttConnect(void);
bool mqttUploadData(void);
void handleError(void);
void data_publish_parse(void);
bool MQTT_INIT(void);
bool MQTT_INIT_SSL(void);
#endif // MQTT_H

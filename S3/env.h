#ifndef ENV_H
#define ENV_H

#define WIFI_SSID "FIESC_IOT_EDU"
#define WIFI_PASS "8120gv08"

#define BROKER_URL "5aa28eccaf2743daba3f733664928c1d.s1.eu.hivemq.cloud"
#define BROKER_PORT 8883

#define BROKER_USD_ID "placa_S3"
#define BROKER_PASS_USR_PASS "PlaquinhaS3"

// A partir daqui depende da placa
#define TOPIC_LED            "s3/led"
#define TOPIC_SERVO_1        "s3/servo1"
#define TOPIC_SERVO_2        "s3/servo2"
#define TOPIC_ULTRASSONICO   "s3/sensor/ultrassom"


#endif

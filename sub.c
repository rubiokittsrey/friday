#include <stdio.h>
#include <mosquitto.h>
#include <string.h>
#include <stdlib.h>

#define MQTT_H "localhost"
#define MQTT_P 1883
#define TOPIC_ONE "test/topic"

void on_message(struct mosquitto *mosq, void *obj, const struct mosquitto_message *message) {
    printf("received message on topic '%s': %s\n", message->topic, (char *)message->payload);
}

int main() {
    struct mosquitto *mosq_client;

    mosquitto_lib_init();
    mosq_client = mosquitto_new(NULL, true, NULL);
    if (!mosq_client) {
        fprintf(stderr, "Failed to create client instance");
        return 0x01;
    }

    mosquitto_message_callback_set(mosq_client, on_message);

    if (mosquitto_connect(mosq_client, MQTT_H, MQTT_P, 60)) {
        fprintf(stderr, "unable to connect to broker,\n");
        return 0x01;
    }

    mosquitto_loop_start(mosq_client);

    mosquitto_subscribe(mosq_client, NULL, TOPIC_ONE, 0);
    printf("enter to quit... \n");
    getchar();

    mosquitto_disconnect(mosq_client);
    mosquitto_destroy(mosq_client);
    mosquitto_lib_cleanup();

    return 0x00;
}
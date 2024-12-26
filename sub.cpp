#include <iostream>
#include <mosquitto.h>

#define BROKER_HOST "localhost"
#define BROKER_PORT 1883
#define MQTT_CLIENT_ID "test-client"

void on_connect(struct mosquitto *mosq_client, void *obj, int rc) {
    if (rc = 0) {
        std::cout << "connected to mqtt broker" << std::endl;
        mosquitto_subscribe(mosq_client, NULL, "test/topic", 0);
    } else {
        std::cerr << "failed to connect, return code: " << rc << std::endl;
    }
}

void on_message(struct mosquitto *mosq_client, void *obj, const struct mosquitto_message *message) {
    std::cout << "received message on topic '" << message->topic << "': " << (char*)message->payload << std::endl;
}

void on_disconnect(struct mosquitto *mosq_client, void *obj, int rc) {
    std::cout << "disconnected from mqtt broker" << std::endl;
}

int main() {
    mosquitto_lib_init();

    struct mosquitto *mosq_client = mosquitto_new(MQTT_CLIENT_ID, true, NULL);
    if (!mosq_client) {
        std::cerr << "failed to create mosquitto client instance" << std::endl;
        mosquitto_lib_cleanup();
        return 0x01;
    }

    mosquitto_connect_callback_set(mosq_client, on_connect);
    mosquitto_message_callback_set(mosq_client, on_message);
    mosquitto_disconnect_callback_set(mosq_client, on_disconnect);

    if (mosquitto_connect(mosq_client, BROKER_HOST, BROKER_PORT, 60) != MOSQ_ERR_SUCCESS) {
        std::cerr << "failed to connect to broker" << std::endl;
        mosquitto_destroy(mosq_client);
        mosquitto_lib_cleanup();
        return 0x01;
    }

    mosquitto_loop_start(mosq_client);

    std::string payload = "hello, world >> test-client";
    mosquitto_publish(mosq_client, NULL, "test/topic", payload.size(), payload.c_str(), 0, false);
    std::cout << "message published to topic 'test/topic'" << std::endl;

    std::cout << "press ctrl+c to quit..." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(10));

    mosquitto_disconnect(mosq_client);
    mosquitto_loop_stop(mosq_client, true);
    mosquitto_destroy(mosq_client);
    mosquitto_lib_cleanup();

    return 0x00;
}
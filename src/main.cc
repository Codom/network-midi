/*
 * main.c
 * Copyright (C) 2020 chris <chris@home>
 *
 * Distributed under terms of the MIT license.
 *
 */
/* C Library includes */
#include <cstdio>
#include <cstdint>
#include <csignal>
#include <mosquitto.h>
/* C++ Library includes */
#include <chrono>
#include <thread>
#include <vector>
#include "rtmidi/RtMidi.h"

/* Global mqtt defines */
#define mqtt_host "localhost"
#define mqtt_port 1883

static int run = 1;
RtMidiOut *midiout;

/* Signal handler */
void handle_signal(int s) {
	run = 0; //signal to mqtt to stop via run flag
	fprintf(stderr, "signal received, exiting...\n");
}

/* MQTT CALLBACKS */

/**
 * Mqtt on_message callback
 * Note: LSB and MSB 'bytes' only use 7 bits of data.
 *
 * At the current moment, I only support two different midi
 * messages: 
 *   - Program Change which will be used to change the current patch
 *   - Control Change which will be mapped to an expression pedal
 */
void on_message(struct mosquitto* client, void* c_arg,
		const struct mosquitto_message* message) {
	unsigned value;
	unsigned scaled_value;
	sscanf((const char*) message->payload, "%u", &value);
	std::vector<unsigned char> midi_mes;
	// If pitch wheel
	// midi_mes.push_back(0xE0); // Status and channel bytes
	// scaled_value = 0x3fff * (value/1024.0); //Scale value to a container that holds 00:MSB:LSB
	// midi_mes.push_back(0x7f & scaled_value); // Data byte 1 (least significant byte data)
	// midi_mes.push_back(scaled_value>>7); // Data byte 2 (most significant byte data)
	// If CC message
	midi_mes.push_back(0xB0); // Status and channel bytes
	scaled_value = 0xff * (value/1024.0);
	midi_mes.push_back(0x4); // CC value
	midi_mes.push_back(scaled_value); // CC param
	midiout->sendMessage(&midi_mes);
	printf("%u    \r", scaled_value);
}

int main(int argc, char* argv[]) {
	int rc = 0;
	char id[] = "midi-recv";
	// Midi init
	midiout = new RtMidiOut();
	midiout->openVirtualPort("net-midi-in"); // Note: The jack/alsa "output", not actual output
	// Mosquitto init
	mosquitto_lib_init();
	struct mosquitto* client = mosquitto_new(id, true, NULL);
	// Signal init
	signal(SIGINT, handle_signal);
	signal(SIGTERM, handle_signal);
	if(client) {
		mosquitto_message_callback_set(client, on_message);
		rc = mosquitto_connect(client, mqtt_host, mqtt_port, 60);
		mosquitto_subscribe(client, NULL, "/expr/value", 0);
		while(run) {
			rc = mosquitto_loop(client, -1, 1);
			if(run && rc) {
				printf("connection error !\n");
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
				mosquitto_reconnect(client);
			}
		}
		mosquitto_destroy(client);
	}
	// Cleanup
	mosquitto_lib_cleanup();
	return rc;
}

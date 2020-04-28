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
#include <cstring>
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
 * Mqtt on_expr_message callback
 * Note: LSB and MSB 'bytes' only use 7 bits of data.
 *
 * At the current moment, I only support two different midi
 * messages: 
 *   - Program Change which will be used to change the current patch
 *   - Control Change which will be mapped to an expression pedal
 */
void on_expr_message(struct mosquitto* client, void* c_arg,
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

void on_footsw_message(struct mosquitto* client, void* c_arg,
		const struct mosquitto_message* message) {
	printf("%s\n",(char*) message->payload);
}

void on_message(struct mosquitto* client, void* c_arg,
		const struct mosquitto_message* message) {
	if(strcmp(message->topic, "/expr/value")==0) on_expr_message(client,c_arg,message);
	else on_footsw_message(client,c_arg,message);
}

int main(int argc, char* argv[]){
	int rc = 0;
	char id[] = "midi-recv";
	// Midi init
	midiout = new RtMidiOut();
	midiout->openVirtualPort("net-midi-in"); // Note: The jack/alsa "output", not actual output
	// Mosquitto init
	mosquitto_lib_init();
	struct mosquitto* expr_client = mosquitto_new(id, true, NULL);
	// Signal init
	signal(SIGINT, handle_signal);
	signal(SIGTERM, handle_signal);
	if(expr_client){
		mosquitto_message_callback_set(expr_client, on_message);
		mosquitto_subscribe(expr_client, NULL, "/expr/value", 0);
		mosquitto_subscribe(expr_client, NULL, "/footsw/event", 0); // Footsw is event based
		rc = mosquitto_connect(expr_client, mqtt_host, mqtt_port, 60);
		while(run){
			rc = mosquitto_loop(expr_client, -1, 1);
			if(run && rc) {
				puts("connection error!");
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
				mosquitto_reconnect(expr_client);
			}
		}
		mosquitto_destroy(expr_client);
	}
	// Cleanup
	mosquitto_lib_cleanup();
	return rc;
}

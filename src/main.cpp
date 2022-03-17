#include <Arduino.h>
#include <ws2812_cpp.h>

#define GATE_INPUT 2
#define DATA_PINS {0}
#define N_LEDS 17
#define RESET_TIME 50
#define COLOR_ORDER grb

// On GATE Rise
void on_rise(ws2812_cpp *ws2812_dev) {
	// Add your tiny-WS2812 code here (prep_tx, tx, end_tx etc.)
	return;
}

// On GATE High
void on_high(ws2812_cpp *ws2812_dev) {
	// Add your tiny-WS2812 code here (prep_tx, tx, end_tx etc.)
	return;
}

// On GATE Fall
void on_fall(ws2812_cpp *ws2812_dev) {
	// Add your tiny-WS2812 code here (prep_tx, tx, end_tx etc.)
	return;
}

// On GATE Low
void on_low(ws2812_cpp *ws8212_dev) {
	// Add your tiny-WS2812 code here (prep_tx, tx, end_tx etc.)
	return;
}

ws2812_cpp *ws2812_dev;

void setup() {
	uint8_t pins[] = DATA_PINS;
	ws2812_cfg cfg;

	cfg.pins = pins;
	cfg.n_dev = sizeof(pins);
	cfg.rst_time_us = RESET_TIME;
	cfg.order = grb;

	ws2812_dev = new ws2812_cpp(cfg, nullptr); // WS2812 device

	pinMode(GATE_INPUT, INPUT);
}

void loop() {
	static bool prev_gate = digitalRead(GATE_INPUT);
	bool gate = digitalRead(GATE_INPUT);

	if (!prev_gate && gate)
		on_rise(ws2812_dev);
	else if (prev_gate && !gate)
		on_fall(ws2812_dev);
	else if (gate)
		on_high(ws2812_dev);
	else
		on_low(ws2812_dev);

	prev_gate = gate;
}
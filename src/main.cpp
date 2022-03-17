#include <Arduino.h>
#include <ws2812_cpp.h>

#define GATE_INPUT PB2
#define DATA_PINS {PB0}
#define N_LEDS 70
#define RESET_TIME 75
#define COLOR_ORDER grb

// Set color here (RGB)
#define COLOR {255, 0, 0}

/* Resets the WS2812 after programming 
 *
 * When programming the MCU while the strip is
 * still connected, it will interpret the data
 * exchange on PB0 as color values. This may
 * put the LED strip into an "stubborn" state if
 * a registered "color tranmission" (which it isn't)
 * is incomplete. To "knock it out" of this state,
 * we need to transmit at least one valid color
 * value.
 */

void rst_after_prog(ws2812_cpp *ws2812_dev) {
	ws2812_rgb rnd;
	ws2812_dev->prep_tx();
	ws2812_dev->tx(&rnd, 1);
	ws2812_dev->close_tx();
}

void fill_strip(ws2812_cpp *ws2812_dev, ws2812_rgb *color, uint16_t strip_size) {
	ws2812_dev->prep_tx();
	for (uint8_t i = 0; i < strip_size; i++)
		ws2812_dev->tx(color, 1);
	ws2812_dev->close_tx();
} 

// On GATE Rise
void on_rise(ws2812_cpp *ws2812_dev) {
	ws2812_rgb color = COLOR;
	fill_strip(ws2812_dev, &color, N_LEDS);
}

// On GATE High
void on_high(ws2812_cpp *ws2812_dev) {
	on_rise(ws2812_dev); // Theoretically not needed, but this will cover starting conditions
}

// On GATE Fall
void on_fall(ws2812_cpp *ws2812_dev) {
	ws2812_rgb black = {0, 0, 0};
	fill_strip(ws2812_dev, &black, N_LEDS);
}

// On GATE Low
void on_low(ws2812_cpp *ws2812_dev) {
	on_fall(ws2812_dev); // Theoretically not needed, but this will cover starting conditions
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

	// rst_after_prog(ws2812_dev);

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
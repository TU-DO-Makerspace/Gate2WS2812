#include <Arduino.h>
#include <ws2812_cpp.h>

#define N_LEDS 75
#define RESET_TIME 75
#define COLOR_ORDER grb

uint8_t GATE_INPUTS[] = {
	A2,
	A3,
	A0,
	A1
};

struct strip_data {
	uint8_t pin;
	ws2812_rgb rgb;
};

strip_data STRIPS[] = 
{
	{
		8, 
		{255, 255, 255}
	},
	{
		9,
		{255, 255, 255}
	},
	{
		10,
		{255, 255, 255}
	},
	{
		11, 
		{255, 255, 255}
	}
};

enum gate_state {
	HI,
	LO,
	RISE,
	FALL
};

class Gate {
	uint8_t pin;
	bool prev;
	gate_state _state;

public:
	Gate(uint8_t pin) : pin(pin) {
		pinMode(pin, INPUT);
		prev = digitalRead(pin);
		update();
	}

	void update() {
		bool curr = digitalRead(pin);
		if (curr && !prev) {
			_state = RISE;
		} else if (!curr && prev) {
			_state = FALL;
		} else if (curr) {
			_state = HI;
		} else {
			_state = LO;
		}
		prev = curr;
	}

	gate_state state() {
		return _state;
	}
};

ws2812_rgb OFF = {0,0,0};

ws2812_cpp *ws2812_dev[sizeof(GATE_INPUTS)];
Gate *gates[sizeof(GATE_INPUTS)];

void setup() {
	
	Serial.begin(9600);

	uint8_t n_gates = sizeof(GATE_INPUTS);
	for (uint8_t i = 0; i < n_gates; i++) {
		// gates[i] = new Gate(GATE_INPUTS[i]);
		pinMode(GATE_INPUTS[i], INPUT);
	}

	uint8_t n_strips = n_gates;
	for (uint8_t i = 0; i < n_strips; i++) {
		pinMode(STRIPS[i].pin, OUTPUT);

		ws2812_cfg cfg;
		cfg.pins = &STRIPS[i].pin;
		cfg.n_dev = 1;
		cfg.rst_time_us = RESET_TIME;
		cfg.order = grb;

		ws2812_dev[i] = new ws2812_cpp(cfg, NULL);
	}

	ws2812_dev[0]->prep_tx();
	for (uint8_t i = 0; i < sizeof(GATE_INPUTS); i++) {
		for (uint8_t j = 0; j < N_LEDS; j++)
			ws2812_dev[i]->tx(&STRIPS[i].rgb, sizeof(STRIPS[i].rgb)/sizeof(ws2812_rgb));
	}
	ws2812_dev[0]->close_tx();

	delay(1000);

	ws2812_dev[0]->prep_tx();
	for (uint8_t i = 0; i < sizeof(GATE_INPUTS); i++) {
		for (uint8_t j = 0; j < N_LEDS; j++)
			ws2812_dev[i]->tx(&OFF, sizeof(OFF)/sizeof(ws2812_rgb));
	}
	ws2812_dev[0]->close_tx();

	delay(100);
}

void loop() {
	ws2812_dev[0]->prep_tx();
	for (uint8_t i = 0; i < sizeof(GATE_INPUTS); i++) {
		if (digitalRead(GATE_INPUTS[i])) {
			for (uint8_t j = 0; j < N_LEDS; j++)
				ws2812_dev[i]->tx(&STRIPS[i].rgb, sizeof(STRIPS[i].rgb)/sizeof(ws2812_rgb));
		}
		else {
			for (uint8_t j = 0; j < N_LEDS; j++)
				ws2812_dev[i]->tx(&OFF, sizeof(OFF)/sizeof(ws2812_rgb));
		}
	}
	ws2812_dev[0]->close_tx();
}
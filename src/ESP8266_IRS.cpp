/*
 * ESP8266_IRS.c
 *
 *  Created on: Feb 24, 2018
 *      Author: c1moore
 */
#include <Arduino.h>
#include <stdint.h>

#include "IRS.h"

// void function pointer type
typedef void (*voidFunc)();

// Mask to record if a specific pin has been triggered.  A 1 bit means the pin was triggered.
uint8_t interruptMask = 0;

// Bitmask to find the bit that corresponds to a specific pin.
const int PIN0_MASK = 0x1;
const int PIN2_MASK = 0x2;
const int PIN4_MASK = 0x4;
const int PIN5_MASK = 0x8;
const int PIN12_MASK = 0x10;
const int PIN13_MASK = 0x20;
const int PIN14_MASK = 0x40;
const int PIN15_MASK = 0x80;

// A noop function for invalid pins. Alternatives to this include returning a flag for invalid
// pins or throwing an error.
void noop() {}

/* The IRSs for the various pins supported by ESP8266. */
void pin0InterruptHandler() {
  interruptMask |= PIN0_MASK;
}

void pin2InterruptHandler() {
  interruptMask |= PIN2_MASK;
}

void pin4InterruptHandler() {
  interruptMask |= PIN4_MASK;
}

void pin5InterruptHandler() {
  interruptMask |= PIN5_MASK;
}

void pin12InterruptHandler() {
  interruptMask |= PIN12_MASK;
}

void pin13InterruptHandler() {
  interruptMask |= PIN13_MASK;
}

void pin14InterruptHandler() {
  interruptMask |= PIN14_MASK;
}

void pin15InterruptHandler() {
  interruptMask |= PIN15_MASK;
}

/* Public Interface */

bool triggeredInterrupt(int pin) {
  uint8_t bitMask = 0;

  switch(pin) {
    case 0:
      bitMask = PIN0_MASK;
      break;

    case 2:
      bitMask = PIN2_MASK;
      break;

    case 4:
      bitMask = PIN4_MASK;
      break;

    case 5:
      bitMask = PIN5_MASK;
      break;

    case 12:
      bitMask = PIN12_MASK;
      break;

    case 13:
      bitMask = PIN13_MASK;
      break;

    case 14:
      bitMask = PIN14_MASK;
      break;

    case 15:
      bitMask = PIN15_MASK;
      break;
  }

  return (interruptMask & bitMask);
}

void resetInterrupt(int pin) {
  uint8_t bitMask = 0xFF;

  switch(pin) {
    case 0:
      bitMask = !PIN0_MASK;
      break;

    case 2:
      bitMask = !PIN2_MASK;
      break;

    case 4:
      bitMask = !PIN4_MASK;
      break;

    case 5:
      bitMask = !PIN5_MASK;
      break;

    case 12:
      bitMask = !PIN12_MASK;
      break;

    case 13:
      bitMask = !PIN13_MASK;
      break;

    case 14:
      bitMask = !PIN14_MASK;
      break;

    case 15:
      bitMask = !PIN15_MASK;
      break;
  }

  interruptMask &= bitMask;
}

void registerInterruptHandler(int pin, int mode) {
  voidFunc handler;

  switch(pin) {
    case 0:
      handler = pin0InterruptHandler;
      break;

    case 2:
      handler = pin2InterruptHandler;
      break;

    case 4:
      handler = pin4InterruptHandler;
      break;

    case 5:
      handler = pin5InterruptHandler;
      break;

    case 12:
      handler = pin12InterruptHandler;
      break;

    case 13:
      handler = pin13InterruptHandler;
      break;

    case 14:
      handler = pin14InterruptHandler;
      break;

    case 15:
      handler = pin15InterruptHandler;
      break;

    default:
      handler = noop;
  }

  attachInterrupt(digitalPinToInterrupt(pin), handler, mode);
}

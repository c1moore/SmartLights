/*
 * ESP8266_IRS.h
 *
 *  Created on: Feb 24, 2018
 *      Author: c1moore
 */

#ifndef SL_IRS_H_
  #define SL_IRS_H_

  /**
   * Returns whether the specified pin was the cause of a hardware interrupt since the last time the pin was
   * reset.
   *
   * @param pin (int) - the pin to check
   *
   * @return (bool) true iff pin caused a hardware interrupt
   */
  bool triggeredInterrupt(int pin);

  /**
   * Resets whether pin caused a hardware interrupt.
   *
   * @param pin (int) - the pin to reset
   */
  void resetInterrupt(int pin);

  /**
   * Registers an interrupt handler (or IRS) to listen for HW interrupts caused by pin.
   *
   * @param pin (int) - the pin for which IRS should be registered
   * @param mode (int) - defines when the interrupt should be triggered. The values are defined in the
   *  Arduino documentation.  At the time of writing this, valid options were HIGH, LOW, RISING, FALLING,
   *  CHANGE.
   */
  void registerInterruptHandler(int pin, int mode);

#endif /* SL_IRS_H_ */

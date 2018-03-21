/*
 * PersistentDID.h
 *
 *  Created on: Feb 25, 2018
 *      Author: c1moore
 */

#ifndef _C1MOORE_INFRASTRUCTURE_PERSISTENTDID
  #define _C1MOORE_INFRASTRUCTURE_PERSISTENTDID

  #include <stdint.h>

  /**
   * Writes PersistentDID to EEPROM at the address specified.
   *
   * @param dId (PersistentDID) - the device ID data to write
   * @param startAddr (int) - the address of the first byte in EEPROM where the device ID should be stored
   *
   * @return (int) the address immediately after the device ID data
   */
  int writeDeviceId(uint8_t dId, int startAddr);

  /**
   * Reads the device ID from EEPROM.  If there is no device ID at the location specified or if its validity cannot
   * be confirmed, 0 is returned.
   *
   * @param startAddr (int) - the address at which the device ID should be stored
   *
   * @return (uint8_t) a nonzero device ID if one could be found at the specified location or 0
   */
  uint8_t readDeviceId(int startAddr);

#endif /* _C1MOORE_INFRASTRUCTURE_PERSISTENTDID */

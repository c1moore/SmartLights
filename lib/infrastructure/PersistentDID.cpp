/*
 * PersistentDID.cpp
 *
 *  Created on: Feb 26, 2018
 *      Author: c1moore
 */
#include <EEPROM.h>

#include "PersistentDID.h"

#define PDID_MAGIC_NUMBER 194

struct PersistentDId {
  const uint8_t magicNumber = PDID_MAGIC_NUMBER;
  uint8_t deviceId;

  uint16_t padding = 0;

  uint8_t checksum;
};

/**
 * Calculates and returns the checksum for the PersistentDID data structure.
 *
 * The inspiration for this implementation (and the structure of PersistentDId) was shamelessly taken from [this post]
 * (https://community.particle.io/t/best-way-to-store-preference-in-flash/15016/19).
 *
 * @param dId (PersistentDID *) - a pointer to the PersistentDID for which the checksum should be calculated
 *
 * @return (uint8_t) the checksum calculated
 */
uint8_t calculateChecksum(PersistentDId *pdid) {
  const int dataLength = sizeof(pdid);

  uint8_t bitmask;
  uint8_t crc = 0xff;
  uint8_t *data = (uint8_t *) pdid;

  for(int byteIndex = 0; byteIndex < dataLength; byteIndex++) {
      bitmask = (data[byteIndex] ^ crc) & 0xff;

      crc = 0;

      if(bitmask &  0x1) crc ^= 0x5e;
      if(bitmask &  0x2) crc ^= 0xbc;
      if(bitmask &  0x4) crc ^= 0x61;
      if(bitmask &  0x8) crc ^= 0xc2;
      if(bitmask & 0x10) crc ^= 0x9d;
      if(bitmask & 0x20) crc ^= 0x23;
      if(bitmask & 0x40) crc ^= 0x46;
      if(bitmask & 0x80) crc ^= 0x8c;
  }

  return crc;
}

int writeDeviceId(uint8_t dId, int startAddr) {
  PersistentDId pdid;

  pdid.deviceId = dId;
  pdid.checksum = calculateChecksum(&pdid);

  EEPROM.put(startAddr, pdid);

  return (startAddr + sizeof(pdid));
}

uint8_t readDeviceId(int startAddr) {
  PersistentDId pdid;

  EEPROM.get(startAddr, pdid);

  // Perform some sanity checks.
  if(pdid.magicNumber != PDID_MAGIC_NUMBER) {
    return 0;
  }

  if(pdid.padding != 0) {
    return 0;
  }

  if(pdid.checksum != calculateChecksum(&pdid)) {
    return 0;
  }

  return pdid.deviceId;
}

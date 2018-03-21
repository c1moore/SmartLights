/*
 * Coordinator.h
 *
 *  Created on: Feb 26, 2018
 *      Author: c1moore
 */

#ifndef _C1MOORE_INFRASTRUCTURE_COORDINATOR
  #define _C1MOORE_INFRASTRUCTURE_COORDINATOR

  #include <stdint.h>

  #include "../scheduler/Runnable.h"
  #include "DCP/DCPRequest.h"
  #include "DCP/DCPResponse.h"
  #include "SensorType.h"
  #include "OutputType.h"

  /**
   * Coordinator is responsible for communicating with the Master node.  The Coordinator is not responsible for parsing data or trying to determining how to respond
   * to the Master node outside of meta communication.
   */
  class Coordinator: public Runnable {
    public:
      Coordinator();
      ~Coordinator();

      /**
       * Registers a new Sensor with the Master node.  This process will assign a unique ID to the sensor.
       *
       * @param type (SensorType) - the SensorType of the Sensor to register
       *
       * @return (int) the unique ID for the sensor
       */
      int registerSensor(SensorType type);

      /**
       * Registers a new output device with the Master node.  This process will assign a unique ID to the output device.
       *
       * @param type (OutputType) - the type of output device to register
       *
       * @return (int) the unique ID for the output device
       */
      int registerOutput(OutputType type);

      /**
       * Executes the main loop for this Coordinator.  During the main loop, the Coordinator will communicate with the Master node as necessary.
       *
       * @return (int) 0 if the the loop was successful; an error code otherwise
       */
      int run();

      /**
       * Sends the provided data to the Master node for processing.
       *
       * @param subDeviceId (uint16_t) - the unique ID assigned to the sub device sending the request
       * @param data (String) - the data to send to the Master node
       *
       * @return (DCPResponse &) the response sent by the Master node
       */
      DCPResponse &sendUpdate(uint16_t subDeviceId, String data);

      /**
       * Sends a request to the Master node requesting it to send an update for the specified sub device.
       *
       * @param subDeviceId (uint16_t) - the unique ID of the sub device requesting an update
       * @param data (String) _optional_ - any additional data to send to the Master node
       */
      DCPResponse &requestUpdate(uint16_t subDeviceId, String data);
      DCPResponse &requestUpdate(uint16_t subDeviceId);

    private:
      class Implementation;

      Implementation *implementation;
  };

#endif /* _C1MOORE_INFRASTRUCTURE_COORDINATOR */

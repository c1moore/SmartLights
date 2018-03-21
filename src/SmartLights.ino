/**
 * Simple IoT light controller.  
 */

#include <ESP8266WiFi.h>

#include "../lib/scheduler/Runnable.h"
#include "../lib/scheduler/Scheduler.h"

#include "../lib/infrastructure/Coordinator.h"
#include "../lib/infrastructure/SensorType.h"

#include "IRS.h"

//class Coordinator: public Runnable {
//  public:
//    Coordinator() {
//
//    }
//
//    int run() {
//      // Check with Master for changes in any registered output.
//    }
//
//    int registerSensor(SensorType type) {
//      // Communicate with Master to get sensorId and return it.
//    }
//
//    int registerOutput(/* OutputType type */) {
//
//    }
//
//    void update(/*int sensorId, int updateType, int updateValue*/) {
//      // Send update to Master.
//    }
//};

class MotionSensor: public Runnable {
  public:
    MotionSensor(Coordinator &coordinator, const int pin = 12): coordinator(coordinator), pin(pin), sid(coordinator.registerSensor(INFRARED_MOTION)) {
      registerInterruptHandler(pin, MotionSensor::mode);
    }

    int run() {
      bool motionDetected = triggeredInterrupt(pin);

      if(!motionDetected) {
        return 0;
      }

//      coordinator.update();
      resetInterrupt(pin);

      return 0;
    }

  private:
    static const auto mode = HIGH;

    const int pin;
    const int sid;

    Coordinator &coordinator;
};

/**
 * Raises base to the exponent-th power and returns the result.
 * 
 * @param base (double) - the value of the base
 * @param exponent (double) - the value of the exponent
 * 
 * @returns (double) the result of base^exponent
 */
double power(double base, double exponent) {
  int result = 1;

  if(exponent >= 0) {
    while(exponent-- > 0) {
      result *= base;
    }
  } else {
    while(exponent++ < 0) {
      result /= base;
    }
  }

  return result;
}

/**
 * Performs all necessary initialization for the system.  This includes:
 *  - Creating a connection to a network
 *  - Creating a Coordinator, Sensors, and all output.
 *  - Initializing and starting the Scheduler.
 */
void setup() {
  // Setup the network.
  WiFi.mode(WIFI_STA);
  WiFi.begin("Waylon-guest", "cranberry33");

  const int base = 250;
  int attempts = 0;
  
  while(WiFi.status() != WL_CONNECTED) {
    if(attempts < 8) {
      delay(power(2, attempts) * base);
    } else {
      delay(30000);
    }

    attempts++;
  }

  Scheduler &scheduler = Scheduler::getInstance();
  Coordinator coordinator = Coordinator();
  MotionSensor motionSensor = MotionSensor(coordinator);

  scheduler.schedule(coordinator);
  scheduler.schedule(motionSensor);

  scheduler.start();
}

/**
 * Scheduler takes the place of loop.
 */
void loop() { }

/*
 * Coordinator.cpp
 *
 *  Created on: Feb 26, 2018
 *      Author: c1moore
 */
#include <Arduino.h>
#include <ESP8266WiFi.h>

#include "../scheduler/Scheduler.h"
#include "Coordinator.h"
#include "PersistentDID.h"
#include "DCP/DCPRequest.h"
#include "DCP/DCPResponse.h"

class Coordinator::Implementation {
  public:
    int did = 0;

    Implementation() {}
    ~Implementation() {}



  private:
    static Scheduler &scheduler;

    const char *server = "devices.c1moore.codes";
    const int port = 80;

    WiFiClient client;

    /**
     *
     */
    WiFiClient getConnectedClient() {
      if(!client.connected()) {
        client.stop();

        while(!client.connect(server, port)) {
          // @TODO This looks a lot like threading to me... Might want to consider making this somewhat thread safe and possibly using a mutex around
          // this method.
          Implementation::scheduler.yield();
        }
      }

      return client;
    }
};

Scheduler &Coordinator::Implementation::scheduler = Scheduler::getInstance();

Coordinator::Coordinator() {
  implementation = new Implementation();

  int did = readDeviceId(0);

  if(did) {
    implementation->did = did;

    return;
  }


}

Coordinator::~Coordinator() {
  delete implementation;
}

int Coordinator::registerSensor(SensorType type) {
  return 0;
}

int Coordinator::run() {
  return 0;
}

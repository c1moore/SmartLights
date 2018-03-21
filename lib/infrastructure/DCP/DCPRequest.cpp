/*
 * DCPRequest.cpp
 *
 *  Created on: Mar 13, 2018
 *      Author: c1moore
 */
#include "Arduino.h"
#include "DCPRequest.h"

const String DCPMethodName[] = { "GET", "POST" };

class DCPRequest::Implementation {
  public:
    const DCPMethod method;
    const String resource;
    const String sessionId;

    String data = "";
    bool sent = false;

    Implementation(const DCPMethod method, const String resource, const String sessionId): method(method), resource(resource), sessionId(sessionId) { }

    Implementation(const Implementation &original): method(original.method), resource(original.resource), sessionId(original.sessionId) {
      data = original.data;
    }
};

DCPRequest::DCPRequest(const DCPMethod method, const String resource, const String sessionId) {
  implementation = new Implementation(method, resource, sessionId);
}

DCPRequest::DCPRequest(const DCPRequest &originalRequest) {
  implementation = new Implementation(originalRequest.implementation);
}

const DCPMethod DCPRequest::getMethod() const {
  return implementation->method;
}

const String DCPRequest::getPath() const {
  return implementation->resource;
}

const String DCPRequest::getSessionId() const {
  return implementation->sessionId;
}

String DCPRequest::getBody() const {
  return implementation->data;
}

bool DCPRequest::wasSent() const {
  return implementation->sent;
}

void DCPRequest::setMessage(String message) {
  implementation->data = message;
}

DCPResponse &DCPRequest::send(WiFiClient &client) {
  // First Line: `METHOD RESOURCE\n`
  const String methodName = DCPMethodName[implementation->method];

  client.write(methodName.c_str(), methodName.length());
  client.write(' ');
  client.write(implementation->resource.c_str(), implementation->resource.length());
  client.write('\n');

  // Second Line: `SESSION_ID: SESSION_TIMESTAMP\n`
  client.write(implementation->sessionId.c_str(), implementation->sessionId.length());
  client.write(": ", 2);
  client.write(millis());

  // Third Line: `CONTENT_LENGTH`
  client.write(implementation->data.length());

  // Fourth Line: `DATA`
  client.write(implementation->data.c_str(), implementation->data.length());

  implementation->sent = true;

  client.flush();

  return DCPResponse(client);
}

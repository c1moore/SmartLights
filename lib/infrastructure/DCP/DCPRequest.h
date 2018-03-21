/*
 * DCPRequest.h
 *
 *  Created on: Mar 10, 2018
 *      Author: c1moore
 */

#ifndef _C1MOORE_INFRASTRUCTURE_DCP_REQUEST
  #define _C1MOORE_INFRASTRUCTURE_DCP_REQUEST

  #include "Arduino.h"
  #include "ESP8266WiFi.h"
  #include "DCPResponse.h"

  /**
   * DCP requests use one of two methods for sending data.  These methods are both semantically and functionally meaningful and resemble HTTP/1.0 request methods.  DCPMethod
   * enumerates the request methods for sending DCP requests.
   */
  enum DCPMethod {
    GET,  // Data will appear as part of the resource after the initial question mark ('?'), if any.  GET requests are safe and idempotent, as defined by W3C.
    POST  // Data will appear in the request body, if any.  POST requests are not guaranteed safe or idempotent.
  };

  /**
   * DCP, which stands for Device Communication Protocol, is a protocol (developed by me) for sending data to and receiving data from a Master node in a connected distributed
   * system.  The protocol is inspired by and in many ways a simplified version of HTTP.  DCP was created to accomplish the following goals
   *  - Reduce the size of requests
   *  - Solidify and condense the structure of requests and responses
   *  - Allow quicker and simpler parsing of data
   *
   * DCPRequest encapsulates how a DCP request is sent to the Master node.  DCP requests have the following structure:
   *    METHOD RESOURCE
   *    SESSION_ID:SESSION_TIMESTAMP
   *    CONTENT_LENGTH
   *    DATA
   * The METHOD is one of the 2 DCPMethods (GET, POST).  RESOURCE is similar to an HTTP path and details what resource is being accessed.
   *
   * SESSION_ID is the session ID assigned to the device when the device registered with the Master the last time it started.  SESSION_TIMESTAMP is the number of milliseconds
   * since the device started.  SESSION_ID and SESSION_TIMESTAMP together form a unique code that allows a device to multiplex requests if necessary.
   *
   * The CONTENT_LENGTH gives the number of bytes in the body of the request, identified by DATA.  DATA is any additional data required by the server to complete the request.
   * DATA is the only field that is optional.
   */
  class DCPRequest {
    public:
      DCPRequest(const DCPMethod method, const String resource, const String sessionId);
      DCPRequest(const DCPRequest &originalRequest);

      /**
       * Returns the DCPMethod used for this request.
       *
       * @return (const DCPMethod) the DCPMethod used for this request
       */
      const DCPMethod getMethod() const;

      /**
       * Returns the path or resource being accessed by this request.
       *
       * @return (const String) the requested resource
       */
      const String getPath() const;

      /**
       * Returns the session ID used to identify this request.
       *
       * @return (const String) the unique session ID created for this device's current session
       */
      const String getSessionId() const;

      /**
       * Returns the body of the request.  The body can be modified using the returned String, if necessary.
       *
       * @return (String) the body of the request
       */
      String getBody() const;

      /**
       * Returns whether the request has been sent or not.
       *
       * @return (bool) true iff the request has been sent
       */
      bool wasSent() const;

      /**
       * Sets the body of this request.
       *
       * @param message (String) - the new body for this request
       */
      void setMessage(String message);

      /**
       * Sends this request to the Master node.  It is important to check if the request has already been sent and only resend requests if absolutely necessary.
       * The Master node will treat duplicates as a new message as the SESSION_TIMESTAMP is generated at the time the request is sent, not the time it is created.
       *
       * @param client (WiFiClient &) - the WiFiClient that can be used to send the request
       *
       * @return (DCPResponse &) a reference to the DCPResponse from the Master node
       */
      DCPResponse &send(WiFiClient &client);

    private:
      class Implementation;

      Implementation *implementation;

      DCPRequest() {
        implementation = NULL;
      }
  };

#endif /* _C1MOORE_INFRASTRUCTURE_DCP_RESQUEST */

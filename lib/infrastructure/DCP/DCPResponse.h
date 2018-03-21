/*
 * DCPResponse.h
 *
 *  Created on: Mar 10, 2018
 *      Author: c1moore
 */

#ifndef _C1MOORE_INFRASTRUCTURE_DCP_RESPONSE
  #define _C1MOORE_INFRASTRUCTURE_DCP_RESPONSE

  #include "Arduino.h"
  #include "ESP8266WiFi.h"

  /**
   * DCP responses include a numeric status.  In addition to providing information on how the request was handled, the status can affect the structure of the response.  For
   * example, a request with a status of 24 or 5X does not include a message body and therefore the CONTENT_LENGTH field is not necessary.  These status numbers were
   * inspired by HTTP status codes.  Some codes are intentionally left blank for future purposes.
   */
  enum DCPStatus {
    SUCCESS = 20,             /* The Master accepted and processed the request.  The body contains data related to the request. */
    SUCCESS_NOCONTENT = 24,   /* The Master accepted and processed the request.  No additional data was sent in the response. */
    BAD_REQUEST = 40,         /* The Master could not process the request.  Additional data MAY be included. */
    UNAUTHORIZED = 41,        /* The Master understood the request, but this device is not able to access the requested resource.  Additional data MAY be included. */
    NOT_FOUND = 44,           /* The Master could not found the requested resource.  Additional data MAY be included. */
    METHOD_NOT_ALLOWED = 45,  /* The Master understood the request, but the request method is not allowed for this resource.  Additional data MAY be included. */
    REQUEST_TIMEOUT = 48,     /* The Master timed out waiting for the full request body. The slave can send the data again.  The acceptable timeout MAY be included. */
    REQUEST_TOO_LONG = 49,    /* The Master understood the request, but the request is too long and will not be processed.  The acceptable limit for requests MAY be included. */
    SERVER_ERROR = 50,        /* An error occurred on the Master while processing the request.  No additional data will be sent. */
    SERVER_DOWN = 53,         /* The Master is down and not responding to normal requests.  If known, the expected time before the server recovers can be sent in the body. */
    RESPONSE_TIMEOUT = 54,    /* The Client timed out waiting for a response. */
    INVALID_RESPONSE = 55     /* The Master sent a response that cannot be parsed. */
  };

  /**
   * A DCP response is a Master node's reply to a DCP request.  The response is required for every request, but the response does not need to include any additional data.
   * DCPResponse encapsulates the structure of a DCP response as well as how a DCP response is parsed.  DCP responses have the following structure
   *    DEVICE_ID:SUBDEVICE_ID
   *    SESSION_ID:SESSION_TIMESTAMP
   *    STATUS_CODE
   *    CONTENT_LENGTH
   *    DATA
   * DEVICE_ID corresponds the the unique ID of the device that sent the request, as provided by the Master node.  SUDEVICE_ID is optional.  If provided, it corresponds to the unique
   * ID of the input or output device connected to the device to which the message corresponds.
   *
   * The SESSION_ID and SESSION_TIMESTAMP have the same value was the request.  These values can be used for multiplexing, if necessary.
   *
   * The STATUS_CODE is one of the DCPStatus codes and corresponds to the status of the request/response.
   *
   * CONTENT_LENGTH is optional depending on the STATUS_CODE.  If required or included, CONTENT_LENGTH must be the number of bytes in the response body.  DATA is the body of the response
   * and contains either data that satisfies the request or provides meta information on the status of the request.
   */
  class DCPResponse {
    public:
      const String deviceId;
      const String subDeviceId;
      const String sessionId;
      const unsigned long sessionTimestamp;

      const DCPStatus statusCode;

      const unsigned int contentLength;
      const String data;

      DCPResponse(WiFiClient &client);
      DCPResponse(const DCPResponse &originalResponse);
      ~DCPResponse();

    private:
      class Implementation;

      Implementation *implementation;

      DCPResponse(): statusCode(INVALID_RESPONSE), contentLength(0), sessionTimestamp(0) {
        implementation = NULL;
      }
  };

#endif /* _C1MOORE_INFRASTRUCTURE_DCP_RESPONSE */

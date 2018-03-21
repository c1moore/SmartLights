/*
 * DCPResponse.cpp
 *
 *  Created on: Mar 13, 2018
 *      Author: c1moore
 */
#include "DCPResponse.h"
#include "Scheduler.h"

#define DEVICEID_LENGTH         16
#define SUBDEVICEID_LENGTH      8
#define SESSIONID_LENGTH        32
#define SESSIONTIMESTAMP_LENGTH sizeof(unsigned long)

class DCPResponse::Implementation {
  public:
    static const int MAX_ATTEMPTS = 20;
    static const int DELAY = 100;

    static Scheduler &scheduler = Scheduler::getInstance();

    Implementation(DCPResponse &response, WiFiClient &client): response(response), client(client) { }

    /**
     * Parses the device ID from the current position in the response, as tracked by the WiFiClient.  When parsing a DCPResponse, this method should be called first.  If
     * the response has not yet been received or not enough data has been received yet, this method will wait for additional data from the server until either
     *  - enough bytes have been received to fully parse the device ID
     *  - a predefined timeout shared amongst all the parsing methods to be reached
     *
     * @param deviceId (char *) - a char buffer at least DEVICEID_LENGTH bytes long that will hold the parsed device ID
     *
     * @return (bool) true iff the operation was success and the response can be further parsed; false if the response should no longer be parsed
     */
    bool parseDeviceId(char *deviceId) {
      int charIndex = 0;

      while(charIndex < DEVICEID_LENGTH) {
        if(!waitForByte()) {
          return false;
        }

        char nextChar = client.peek();

        if(nextChar == ':' || nextChar == '\n') {
          break;
        }

        deviceId[charIndex++] = client.read();
      }

      if(client.peek() == ':') {
        client.read();
      }

      return true;
    }

    /**
     * Parses the optional sub-device ID from the DCPResponse.  If enough bytes have not been received to parse the full sub-device ID, this method will wait for either
     *  - the Master node to send enough bytes to parse the sub-device ID
     *  - a predefined timeout shared amongst all the parsing methods to be reached
     *
     * If the sub-device ID is not specified `subDeviceId` will not be modified.
     *
     * @param subDeviceId (char *) - a char buffer with a capacity of at least SUBDEVICEID_LENGTH bytes that will hold the parsed sub-device ID
     *
     * @return (bool) true iff the sub-device ID was successfully parsed or omitted and the response can be safely parsed further
     */
    bool parseSubDeviceId(char *subDeviceId) {
      int charIndex = 0;

      while(charIndex < SUBDEVICEID_LENGTH) {
        if(!waitForByte()) {
          return false;
        }

        if(client.peek() == '\n') {
          break;
        }

        subDeviceId[charIndex++] = client.read();
      }

      if(client.read() != '\n') {
        handleInvalidResponse();

        return false;
      }

      return true;
    }

    /**
     * Parses the session ID from the DCPResponse.  If enough bytes have not been received to parse the full session ID, this method will wait for either
     *  - the Master node to send enough bytes to parse the session ID
     *  - a predefined timeout shared amongst all the parsing methods to be reached
     *
     * @param sessionId (char *) - a buffer with a capacity of at least SESSIONID_LENGTH bytes that will hold the parsed session ID
     *
     * @return (bool) true iff the session ID was successfully parsed and the response can be safely parsed further
     */
    bool parseSessionId(char *sessionId) {
      int charIndex = 0;

      while(charIndex < SESSIONID_LENGTH) {
        if(!waitForByte()) {
          return false;
        }

        if(client.peek() == ':') {
          break;
        }

        subDeviceId[charIndex++] = client.read();
      }

      if(client.read() != ':') {
        handleInvalidResponse();

        return false;
      }

      return true;
    }

    /**
     * Parsed the session timestamp from the DCPResponse.  If enough bytes have not been received to parse the full session timestamp, this method will wait for either
     *  - the Master node to send enough bytes to parse the session timestamp
     *  - a predefined timeout shared amongst all the parsing methods to be reached
     *
     * If an error occurs while parsing the timestamp, 0 will be returned.  However, a better way to check for an error is by checking the status code of the response.  If the
     * status code is set to an error, parsing has failed.
     *
     * @return (unsigned long) the parsed session timestamp or 0 if an error occurred
     */
    unsigned long parseSessionTimestamp() {
      static const int MAX_LONG_DIGITS = 10;

      char digits[] = new char[MAX_LONG_DIGITS];
      int digitIndex = 0;

      while(digitIndex < MAX_LONG_DIGITS) {
        if(!waitForByte()) {
          return 0;
        }

        if(client.peek() == '\n') {
          break;
        }

        char nextDigit = client.read();

        if(nextDigit < '0' || nextDigit > '9') {
          handleInvalidResponse();

          return 0;
        }

        digits[digitIndex++] = nextDigit;
      }

      if(client.read() != '\n') {
        handleInvalidResponse();

        return 0;
      }

      // Convert the char[] to a long.
      unsigned long timestamp = 0L;

      while(--digitIndex >= 0) {
        unsigned long tempValue = (timestamp * 10) + (unsigned long) (digits[digitIndex] - '0');

        if(tempValue < timestamp) {
          // Overflow detected.  Not bullet-proof, though.
          handleInvalidResponse();

          return 0;
        }

        timestamp = tempValue;
      }

      return timestamp;
    }

    /**
     * Parses the status of the DCPResponse.  If enough bytes have not been received to parse the full status code, this method will wait for either
     *  - the Master node to send enough bytes to parse the status code
     *  - a predefined timeout shared amongst all the parsing methods to be reached
     *
     * @return (DCPStatus) the status of the request
     */
    DCPStatus parseStatus() {
      if(!waitForByte()) {
        handleResponseTimeout();

        return response.statusCode;
      }

      int statusCode = (client.read() - '0') * 10;
      if(statusCode > 50 || statusCode < 0) {
        handleInvalidResponse();

        return response.statusCode;
      }

      int subStatusDigit = client.read() - '0';
      if(subStatusDigit < 0 || subStatusDigit > 9) {
        handleInvalidResponse();

        return response.statusCode;
      }

      return (statusCode + subStatusDigit);
    }

    /**
     * Parses the optional content length from the DCPResponse.  This method assumes that the content length field is expected based on the status code of the message.  If enough
     * bytes have not been received to parse the full content length, this method will wait for either
     *  - the Master node to send enough bytes to parse the content length
     *  - a predefined timeout shared amongst all the parsing methods to be reached
     *
     * @return (unsigned int) the number of bytes in the response body
     */
    unsigned int parseContentLength() {
      static const int MAX_INT_DIGITS = 10;

      char digits[] = new char[MAX_INT_DIGITS];
      int digitIndex = 0;

      while(digitIndex < MAX_INT_DIGITS) {
        if(!waitForByte()) {
          return 0;
        }

        if(client.peek() == '\n') {
          break;
        }

        char nextDigit = client.read();

        if(nextDigit < '0' || nextDigit > '9') {
          handleInvalidResponse();

          return 0;
        }

        digits[digitIndex++] = nextDigit;
      }

      if(client.read() != '\n') {
        handleInvalidResponse();

        return 0;
      }

      // Convert the char[] to a long.
      unsigned int contentLength = 0;

      while(--digitIndex >= 0) {
        unsigned int tempValue = (contentLength * 10) + (unsigned int) (digits[digitIndex] - '0');

        if(tempValue < contentLength) {
          // Overflow detected.  Not bullet-proof, though.
          handleInvalidResponse();

          return 0;
        }

        contentLength = tempValue;
      }

      return contentLength;
    }

    /**
     * Parses the optional response body from the DCPResponse, reading up to CONTENT_LENGTH bytes.  If CONTENT_LENGTH bytes have not been received, this method will wait for either
     *  - the Master node to send enough bytes to parse the body
     *  - a predefined timeout shared amongst all the parsing methods to be reached
     *
     * @param body (char *) - a char buffer with a capacity of CONTENT_LENGTH bytes that will hold the parsed body
     *
     * @return (bool) true iff the body was successfully parsed
     */
    bool parseBody(char *body) {
      int charIndex = 0;

      while(charIndex < response.contentLength) {
        if(!waitForByte()) {
          return false;
        }

        subDeviceId[charIndex++] = client.read();
      }

      return true;
    }

  private:
    DCPResponse &response;
    WiFiClient &client;

    int attempts = 0;

    /**
     * Waits for 1 byte of data from the Master node.
     *
     * @return (bool) true if a byte was successfully received; false if a timeout was reached before the byte was received
     */
    bool waitForByte() {
      while(client.peek() == 0) {
        if(attempts > Implementation::MAX_ATTEMPTS) {
          handleResponseTimeout();

          return false;
        }

        attempts++;

        Implementation::scheduler.sleep(Implementation::DELAY);
      }

      return true;
    }

    /**
     * Handles a response timeout, setting all fields of the DCPResponse to a default value for the RESPONSE_TIMEOUT error.  Even fields previously parsed will be set to the
     * default value for the field.
     */
    void handleResponseTimeout() const {
      response.deviceId = "";
      response.subDeviceId = "";
      response.sessionId = "";
      response.sessionTimestamp = "";

      response.statusCode = RESPONSE_TIMEOUT;

      response.contentLength = 0;
      response.data = "";
    }

    /**
     * Handles an invalid response received from the Master node.  This method sets all fields of the DCPResponse to a default value for the INVALID_RESPONSE error.  For security
     * purposes, all fields are set to the default value even if some fields have been successfully parsed.
     */
    void handleInvalidResponse() const {
      response.deviceId = "";
      response.subDeviceId = "";
      response.sessionId = "";
      response.sessionTimestamp = "";

      response.statusCode = INVALID_RESPONSE;

      response.contentLength = 0;
      response.data = "";
    }
};

DCPResponse::DCPResponse(WiFiClient &client) {
  implementation = new Implementation(&this, client);

  if(!implementation->waitForByte()) {
    return;
  }

  // Now for the tricky part due to the nature of WiFiClient.  WiFiClient doesn't block until the requested size is received, so we'll read this char by char.
  // We _could_ use something like Flex/Bison, but this seems like overkill for this situation.
  int charIndex;

  // Parse the deviceId
  char deviceId[DEVICEID_LENGTH + 1] = { 0 };
  if(!implementation->parseDeviceId(deviceId)) {
    return;
  }

  // Parse the subDeviceId
  char subDeviceId[SUBDEVICEID_LENGTH + 1] = { 0 };
  if(!implementation->parseSubDeviceId(subDeviceId)) {
    return;
  }

  // Parse the sessionId.
  char sessionId[SESSIONID_LENGTH + 1] = { 0 };
  if(!implementation->parseSessionId(sessionId)) {
    return;
  }

  // Parse the sessionTimestamp
  sessionTimestamp = implementation->parseSessionTimestamp();
  if(statusCode == INVALID_RESPONSE) {
    return;
  }

  // Parse the statusCode
  statusCode = implementation->parseStatus();

  // If the statusCode suggests there is further data, parse the content.
  if(statusCode != SUCCESS_NOCONTENT || statusCode != SERVER_ERROR || statusCode != RESPONSE_TIMEOUT || statusCode != INVALID_RESPONSE) {
    contentLength = implementation->parseContentLength();

    if(contentLength != 0) {
      char data[] = new char[contentLength];

      if(!implementation->parseBody(data)) {
        return;
      }

      this->data = String(data);
    }
  }

  this->deviceId = String(deviceId);
  this->subDeviceId = String(subDeviceId);
  this->sessionId = String(sessionId);
}

DCPResponse::DCPResponse(const DCPResponse &originalResponse) {
  implementation = nullptr;

  deviceId = originalResponse.deviceId;
  subDeviceId = originalResponse.subDeviceId;
  sessionId = originalResponse.sessionId;
  sessionTimestamp = originalResponse.sessionTimestamp;

  statusCode = originalResponse.statusCode;

  contentLength = originalResponse.contentLength;
  data = originalResponse.data;
}

DCPResponse::~DCPResponse() {
  if(implementation == nullptr) {
    return;
  }

  delete implementation;
}

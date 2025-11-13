/*
 * protocol.h
 *
 * Client header file
 * Definitions, constants and function prototypes for the client
 */

#ifndef PROTOCOL_H_
#define PROTOCOL_H_

// Shared application parameters
#define SERVER_PORT 56700  // Server port (change if needed)
#define SERVER_ADDRESS "127.0.0.1" //Server address

typedef struct {
    char type;        // Weather data type: 't', 'h', 'w', 'p'
    char city[64];    // City name (null-terminated string)
} weather_request_t;

typedef struct {
    unsigned int status;  // Response status code
    char type;            // Echo of request type
    float value;          // Weather data value
} weather_response_t;

//used to avoid constant in the code that can cause problems
typedef enum {
    TYPE_TEMP   = 't',
    TYPE_HUM    = 'h',
    TYPE_WIND   = 'w',
    TYPE_PRESS  = 'p',
} possible_type;

typedef enum {
    SUCCESS = 0,
    ERROR_CITY = 1,
    ERROR_REQUEST = 2,
} possible_status;

// Function prototypes
// Add here the signatures of the functions implemented by students

void errorhandler(const char *error_message) {
    perror(error_message);
}

#endif /* PROTOCOL_H_ */

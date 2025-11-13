/*
 * protocol.h
 *
 * Server header file
 * Definitions, constants and function prototypes for the server
 */

#ifndef PROTOCOL_H_
#define PROTOCOL_H_

#include <string.h>
#include <stdlib.h>

// Shared application parameters
#define SERVER_PORT 56700  // Server port (change if needed)
#define QUEUE_SIZE 5       // Size of pending connections queue

//case sensitive city
static const char *possibleCityValues[] = {
    "Bari", "Roma", "Milano", "Napoli", "Torino",
    "Palermo", "Genova", "Bologna", "Firenze", "Venezia"
};
static const int possibleCityCount = sizeof(possibleCityValues) / sizeof(possibleCityValues[0]);


typedef struct {
    unsigned int status;  // Response status code
    char type;            // Echo of request type
    float value;          // Weather data value
} weather_response_t;

typedef struct {
    char type;        // Weather data type: 't', 'h', 'w', 'p'
    char city[64];    // City name (null-terminated string)
} weather_request_t;

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

//check the presence of the city in the list
int checkCityPresence(const char *city) {
    for (int i = 0; i < possibleCityCount; i++) {
        if (strcasecmp(city, possibleCityValues[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

float getRandomValue(float min, float max){
	return min + ((float)rand() / RAND_MAX) * (max - min);
}

// Range: -10.0 to 40.0 °C
float get_temperature(void){
	float min = -10.0;
	float max = 40.0;

	return getRandomValue(min, max);
}

// Range: 20.0 to 100.0 %
float get_humidity(void){
	float min = 20.0;
	float max = 100.0;

	return getRandomValue(min, max);
}

// Range: 0.0 to 100.0 km/h
float get_wind(void){
	float min = 0.0;
	float max = 100.0;

	return getRandomValue(min, max);
}

// Range: 950.0 to 1050.0 hPa
float get_pressure(void){
	float min = 950.0;
	float max = 1050.0;

	return getRandomValue(min, max);
}

#endif /* PROTOCOL_H_ */

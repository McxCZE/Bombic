#ifndef VERSION_H_
#define VERSION_H_

// Bombic version information
#define BOMBIC_VERSION_MAJOR 2
#define BOMBIC_VERSION_MINOR 0
#define BOMBIC_VERSION_PATCH 0

#define BOMBIC_VERSION_STRING "2.0.0"
#define BOMBIC_VERSION_NAME "Bombic Native"

// Build info - can be overridden at compile time
#ifndef BOMBIC_BUILD_DATE
#define BOMBIC_BUILD_DATE __DATE__
#endif

#ifndef BOMBIC_BUILD_TIME
#define BOMBIC_BUILD_TIME __TIME__
#endif

// Full version string with build info
#define BOMBIC_FULL_VERSION BOMBIC_VERSION_NAME " " BOMBIC_VERSION_STRING

#endif // VERSION_H_

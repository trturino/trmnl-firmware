#include <battery.h>
#include <config.h>

#if defined(BOARD_TRMNL_X)
static BQ27427Battery batteryInstance;
#elif defined(BOARD_SPARKFUN_SPECTRA6)
static MAX17048Battery batteryInstance;
#else
static ADCBattery batteryInstance;
#endif

#ifdef INCLUDE_BQ27427
BQ27427Battery &battery() { return batteryInstance; }
#else
BaseBattery &battery() { return batteryInstance; }
#endif

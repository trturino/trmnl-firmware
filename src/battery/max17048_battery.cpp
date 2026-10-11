#include <battery.h>

#ifdef BOARD_SPARKFUN_SPECTRA6
#include <SparkFun_MAX1704x_Fuel_Gauge_Arduino_Library.h>
#include <trmnl_log.h>

float MAX17048Battery::readVoltage(TRMNL_DEVICE *) { return readVoltage(); }

float MAX17048Battery::readVoltage() {
  SFE_MAX1704X gauge(MAX1704X_MAX17048);
  if (!gauge.begin()) {
    Log_error("MAX17048 battery gauge unavailable");
    return -1.0f;
  }
  return gauge.getVoltage();
}
#endif

"""Exercise the actual boot Wi-Fi session with host-side hardware doubles.

Run: python3 test/host/test_wifi_session.py
The ESP32-only headers are replaced; the production control flow is unchanged.
"""
from pathlib import Path
import os
import subprocess
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[2]
DOUBLES = r'''
#include <cassert>
#include <string>
using String = std::string;
constexpr int WIFI_STA = 1;
constexpr int WIFI_FAILED = 2, WIFI_CONNECT = 3, CAPTIVE_WIFI_TIMEOUT = 4;
constexpr const char *PREFERENCES_CONNECT_WIFI_RETRY_COUNT = "wifi_retry";
struct Sleeping {};
int portals, clears, resets, defaults, errors, timeouts, displaySleeps;
bool connected, portalOK;
struct {
  void mode(int) {}
  int status() { return connected ? 3 : 6; }
  String localIP() { return "192.0.2.1"; }
  void disconnect(bool) {}
} WiFi;
struct {
  bool saved;
  bool isSaved() { return saved; }
  void setHostname(String) {}
  String getAPSSID() { return "TRMNL-TEST"; }
  void setResetSettingsCallback(void (*)()) {}
  bool startPortal() { ++portals; return portalOK; }
} WifiCaptivePortal;
struct { template<class... T> void info(T...) {} } Log;
#define Log_info(...) ((void)0)
#define Log_error(...) ((void)0)
#define Log_fatal_submit(...) ((void)0)
struct { void putInt(const char *, int) { ++resets; } } preferences;
struct { void applyDefault() { ++defaults; } } refreshInterval;
namespace Messages { String firmware_version() { return "test"; } }
String getWifiClientHostname() { return "TRMNL-TEST"; }
bool connectWithSavedCredentials() { return connected; }
void resetDeviceCredentials() { ++clears; }
void display_sleep() { ++displaySleeps; }
void goToSleep() { throw Sleeping{}; }
void wifiErrorDeepSleep() { throw Sleeping{}; }
template<class... T> void showMessageWithLogo(int message, T...) {
  errors += message == WIFI_FAILED;
  timeouts += message == CAPTIVE_WIFI_TIMEOUT;
}
'''
CASES = r'''
void reset(bool saved, bool online, bool portalResult = true) {
  portals = clears = resets = defaults = errors = timeouts = displaySleeps = 0;
  WifiCaptivePortal.saved = saved;
  connected = online;
  portalOK = portalResult;
}
int main() {
  reset(true, true);
  wifiSessionConnect(true);
  assert(portals == 0 && clears == 0 && resets == 1 && defaults == 0);

  reset(false, false);
  wifiSessionConnect(true);
  assert(portals == 1 && clears == 0 && resets == 1);

  for (bool showError : {false, true}) {
    reset(true, false);
#ifdef WIFI_AUTO_PORTAL_ON_FAILURE
    wifiSessionConnect(showError);
    assert(portals == 1 && clears == 0 && resets == 1);
    assert(defaults == 1 && errors == 0);
#else
    try { wifiSessionConnect(showError); assert(false); } catch (Sleeping &) {}
    assert(portals == 0 && clears == 0 && resets == 0);
    assert(errors == int(showError));
#endif
  }

#ifdef WIFI_AUTO_PORTAL_ON_FAILURE
  reset(true, false, false);
  try { wifiSessionConnect(false); assert(false); } catch (Sleeping &) {}
  assert(portals == 1 && clears == 0 && resets == 0 && defaults == 1);
  assert(timeouts == 1 && errors == 0 && displaySleeps == 1);
#endif
}
'''


class WifiSessionTest(unittest.TestCase):
    def test_buttonless_recovery_and_upstream_behavior(self):
        source = '\n'.join(line for line in (ROOT / 'src/wifi_session.cpp').read_text().splitlines()
                           if not line.lstrip().startswith('#include'))
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory)
            cpp = path / 'wifi_session_test.cpp'
            cpp.write_text(DOUBLES + source + CASES)
            for auto_portal in (False, True):
                with self.subTest(auto_portal=auto_portal):
                    binary = path / ('auto' if auto_portal else 'stock')
                    flags = ['-DWIFI_AUTO_PORTAL_ON_FAILURE=1'] if auto_portal else []
                    subprocess.run([os.environ.get('CXX', 'c++'), '-std=c++11', *flags,
                                    str(cpp), '-o', str(binary)], check=True)
                    subprocess.run([str(binary)], check=True)


if __name__ == '__main__':
    unittest.main()

# esp power button

Remote home server power-button.

- Board: Wemos D1 mini (esp8266)
- address in local nework (mDNS): http://espkvm.local
- press button: `espkvm.local/power` (POST request, e.g. `curl -X POST http://espkvm.local/power`)
- long press button: `espkvm.local/power?push=long` (POST request)
- quick health state and uptime `espkvm.local/hz`

## Configuration

- specify `WIFI_SSID` and `WIFI_PASS` in `secrets.h`
- potentially change the `HOSTNAME` in the main file
- comment `DEBUG` to remove Serial trace and use D5 pin for pressing (should be connected to optocoupler), otherwise integrated LED is used instead.


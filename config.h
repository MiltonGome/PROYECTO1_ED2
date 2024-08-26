/** Adafruit IO Config ***/

// visit io.adafruit.com if you need to create an account,
// or if you need your Adafruit IO key.
#define IO_USERNAME  "MiltonG"
#define IO_KEY       "aio_uzkn38kTh0NeBa4oxDsRt2nkm5Wi"


#define WIFI_SSID "S22+ de"
#define WIFI_PASS "amaicoljaxonlocallaron"


#include "AdafruitIO_WiFi.h"


AdafruitIO_WiFi io(IO_USERNAME, IO_KEY, WIFI_SSID, WIFI_PASS);
#include <Arduino.h>
#include "Somo1ELV.h"


#define VERBOSITY 1

typedef Somo1ELV::humidityPercent_t humidityPercent_t;
typedef Somo1ELV::temperatureDegC_t temperatureDegC_t;

static constexpr typeof(Wire)& i2c = Wire;
static Somo1ELV somo1(i2c);

// static constexpr typeof(Serial1)& serial_ = Serial1;
static constexpr typeof(Serial)& serial_ = Serial;


static void setupSomo1() {
  delay(100);

  // Note that these are example calibration values. They probably need to be changed.
  // You may run this example sketch and watch on the serial monitor what raw values
  // your sensor provides for wet and dry soil humidity, and change the calibration
  // accordingly.
  somo1.setHumidityRawFor100Percent(1990);
  somo1.setHumidityRawFor0Percent(2480);

  somo1.begin(Somo1ELV::TEMPERATURE_HIGH_PRECISION);
  delay(800);

#if VERBOSITY
  if(somo1.isHumiditySensorAvailable()) {
    serial_.println("Soil humidity sensor detected.");
  } else {
    serial_.println("Soil humidity sensor not found.");
  }

  if(somo1.isTemperatureSensorAvailable()) {
    serial_.println("Soil temperature sensor detected.");
  } else {
    serial_.println("Soil temperature sensor not found.");
  }
#endif
}


//The setup function is called once at startup of the sketch
void setup()
{
  i2c.begin();
  pinMode(LED_BUILTIN, OUTPUT);

  serial_.begin(115200);
  serial_.println();
  setupSomo1();

}

static void printHumidityResult(const Somo1ELV::ERROR err, const humidityPercent_t& soilHumidityPercent, uint16_t soilHumidityRaw) {
  if (err == Somo1ELV::NO_ERROR) {
    serial_.print("Humidity: ");
    serial_.print(soilHumidityPercent);
    serial_.print("%, raw: ");
    serial_.println(soilHumidityRaw);
  } else {
    serial_.println("Humidity measurement failed.");
  }
  delay(100);
}

static void printTemperatureResult(const Somo1ELV::ERROR err, const temperatureDegC_t& soilTemperatureDegC) {
  if (err == Somo1ELV::NO_ERROR) {
    serial_.print("Temperature: ");
    serial_.print(soilTemperatureDegC, 2);
    serial_.println("\260C");
  } else {
    serial_.println("Temperature measurement failed.");
  }
  delay(100);
}

// The loop function is called in an endless loop
void loop()
{
  {
    humidityPercent_t soilHumidityPercent;
    uint16_t soilHumidityRaw;
    Somo1ELV::ERROR err = somo1.measureSoilHumidity(soilHumidityPercent, &soilHumidityRaw);
    printHumidityResult(err, soilHumidityPercent, soilHumidityRaw);
  }

  {
    temperatureDegC_t soilTemperatureDegC;
    Somo1ELV::ERROR err = somo1.measureSoilTemperatureDegC(soilTemperatureDegC);
    printTemperatureResult(err, soilTemperatureDegC);
  }

  delay(3000);
}

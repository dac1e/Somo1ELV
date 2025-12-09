/*
  Somo1ELV - Arduino libary for the ELV somo1 soil humidity and soil temperature sensor Copyright (c)
  2025 Wolfgang Schmieder.  All right reserved.

  Contributors:
  - Wolfgang Schmieder

  Project home: https://github.com/dac1e/Somo1ELV/

  This library is free software; you can redistribute it and/or modify it
  the terms of the GNU Lesser General Public License as under published
  by the Free Software Foundation; either version 3.0 of the License,
  or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
*/

#pragma once

#ifndef SOMO1ELV_H_
#define SOMO1ELV_H_

#include <SensirionI2cSht4x.h>
#include <FDC2x1x.h>
#include <Wire.h>

#if defined NO_ERROR
#undef NO_ERROR
#endif

class Somo1ELV {
  typedef uint16_t sht4xTicks_t;

public:
  typedef int16_t humidityPercent_t;
  typedef float temperatureDegC_t;

  // Error handling does only work properly, if the underlying Wire implementation is non-blocking.
  // In case of a broken I2C bus, the Wire transmit or receive function may stay in an endless loop.
  // This problem can't be fixed here.
  enum ERROR {
    TEMPERATRUR_SENSOR_NOT_AVAILABLE = -4,
    HUMIDITY_SENSOR_NOT_AVAILABLE = -3,

    TEMPERATRUR_MEASUREMENT_FAILED = -2,
    HUMIDITY_MEASUREMENT_FAILED = -1,
    NO_ERROR = 0
  };

  enum SHT4X_PRECISION {
    TEMPERATURE_LOW_PRECISION,
    TEMPERATURE_MEDIUM_PRECISION,
    TEMPERATURE_HIGH_PRECISION,
  };

  Somo1ELV(typeof (Wire) &wire = Wire);

  void begin(SHT4X_PRECISION precision = TEMPERATURE_HIGH_PRECISION);

  bool isTemperatureSensorAvailable() const {
    return mSht4xSerialNumber != 0;
  }

  bool isHumiditySensorAvailable() const {
    return mSoilHumiditySensorAvailable;
  }

  // Sht4x has a unique serial number that is assigned in the factory.
  uint32_t getSht4xSerialNumber();

  // calibration for 0% soil humidity
  void setHumidityRawFor0Percent(int16_t rawValue) {
    mSoilHumCalibration.soilHumidity0percent = rawValue;
  }

  // calibration for 100% soil humidity
  void setHumidityRawFor100Percent(int16_t rawValue) {
    mSoilHumCalibration.soilHumidity100percent = rawValue;
  }

  // soil humidity might become negative upon wrong calibration data.
  ERROR measureSoilHumidity(humidityPercent_t& soilHumidityPercent, uint32_t* soilHumidityRaw = nullptr);

  ERROR measureSoilTemperatureDegC(temperatureDegC_t& soilTemperatureDegC);

  // return sleep mode before that call or -1 upon FDC I2C read error.
  int enableFdc2x1xSleepMode() {
    return mFdc2x1x.enableSleepMode();
  }

  // return sleep mode before that call -1 upon FDC I2C read error.
  int disableFdc2x1xSleepMode() {
    return mFdc2x1x.disableSleepMode();
  }

private:
  ERROR measureSoilTemperature_raw(sht4xTicks_t& soilTemperatureSht4xTicks);
  ERROR measureSoilHumidity_raw(uint32_t& soilHumidityRaw);

  struct soil_humidity_calibration {
    int16_t soilHumidity100percent;
    int16_t soilHumidity0percent;
  };

  soil_humidity_calibration mSoilHumCalibration;
  FDC2x1x mFdc2x1x;
  bool mSoilHumiditySensorAvailable;

  SensirionI2cSht4x mSht4x;
  uint32_t mSht4xSerialNumber;
  SHT4X_PRECISION mSht4xPrecision;
};
#endif /* SOMO1ELV_H_ */

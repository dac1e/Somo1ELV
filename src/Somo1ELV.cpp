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

#include "Somo1ELV.h"

Somo1ELV::Somo1ELV(typeof (Wire) &wire)
  : mSoilHumiditySensorAvailable(false), mSoilHumCalibration{0, 4095},  mFdc2x1x(FDC2x1x_I2C_ADDR_1, wire)
  , mSht4xSerialNumber(0), mSht4xPrecision(TEMPERATURE_HIGH_PRECISION) {
}

void Somo1ELV::begin(SHT4X_PRECISION precision) {
  // Initialize FDC2x1x and wire
  const FDC2x1x_DEVICE fdcDev = mFdc2x1x.begin(0x01, true, FDC2x1x_DEGLITCH_10Mhz);

  if(fdcDev != FDC2x1x_DEVICE_INVALID) {
    mSoilHumiditySensorAvailable = true;
  }

  mFdc2x1x.setFrequencyDivider(0, 23);
  mFdc2x1x.setOffset(0, 24576);
  mFdc2x1x.disableSleepMode();

  mSht4xPrecision = precision;
  mSht4x.begin(mFdc2x1x.getWire(), SHT40_I2C_ADDR_44);

  uint32_t serialNumber;
  if(mSht4x.serialNumber(serialNumber) == HighLevelError::NoError) {
    mSht4xSerialNumber = serialNumber;
  }
}

uint32_t Somo1ELV::getSht4xSerialNumber() {
  return mSht4xSerialNumber;
}

void Somo1ELV::pickHumidityRawFor0Percent() {
  if(isHumiditySensorAvailable()) {
    uint16_t soilHumidityRaw;
    const ERROR err = measureSoilHumidity_raw(soilHumidityRaw);
    if (err == NO_ERROR) {
      mSoilHumCalibration.soilHumidity0percent = soilHumidityRaw;
    }
  }
}

void Somo1ELV::pickHumidityRawFor100Percent(int16_t rawValue) {
  if(isTemperatureSensorAvailable()) {
    uint16_t soilHumidityRaw;
    const ERROR err = measureSoilHumidity_raw(soilHumidityRaw);
    if (err == NO_ERROR) {
      mSoilHumCalibration.soilHumidity100percent = soilHumidityRaw;
    }
  }
}

Somo1ELV::ERROR Somo1ELV::measureSoilHumidity(humidityPercent_t& soilHumidityPercent, uint16_t* soilHumidityRaw) {
  uint16_t soilHumidityRaw_;
  if(soilHumidityRaw == nullptr) {
    soilHumidityRaw = &soilHumidityRaw_;
  }
  const ERROR err = measureSoilHumidity_raw(*soilHumidityRaw);
  if(err == NO_ERROR) {
    if(soilHumidityPercent) {
      const int32_t delta = mSoilHumCalibration.soilHumidity100percent - mSoilHumCalibration.soilHumidity0percent;
      soilHumidityPercent = (delta == 0) ? 0 // avoid division by zero
          : 100L * (static_cast<int32_t>(*soilHumidityRaw) - mSoilHumCalibration.soilHumidity0percent) / delta;
    }
  }
  return err;
}

Somo1ELV::ERROR Somo1ELV::measureSoilTemperatureDegC(temperatureDegC_t& soilTemperatureDegC) {
  sht4xTicks_t ticks;
  const ERROR error = measureSoilTemperature_raw(ticks);
  if(error == NO_ERROR) {
    soilTemperatureDegC = mSht4x.signalTemperature(ticks);
  }
  return error;
}

Somo1ELV::ERROR Somo1ELV::measureSoilTemperature_raw(sht4xTicks_t& soilTemperatureSht4xTicks) {
  if(isTemperatureSensorAvailable()) {
    int16_t error = HighLevelError::NoError;
    sht4xTicks_t airHumidityDummy;
    switch(mSht4xPrecision) {
      case TEMPERATURE_LOW_PRECISION:
      error = mSht4x.measureLowestPrecisionTicks(soilTemperatureSht4xTicks, airHumidityDummy);
      break;
      case TEMPERATURE_MEDIUM_PRECISION:
      error = mSht4x.measureMediumPrecisionTicks(soilTemperatureSht4xTicks, airHumidityDummy);
      break;
      case TEMPERATURE_HIGH_PRECISION:
      error = mSht4x.measureHighPrecisionTicks(soilTemperatureSht4xTicks, airHumidityDummy);
      break;
    }

    if(error != HighLevelError::NoError) {
      return TEMPERATRUR_MEASUREMENT_FAILED;
    }
    return NO_ERROR;
  }
  return TEMPERATRUR_SENSOR_NOT_AVAILABLE;
}

Somo1ELV::ERROR Somo1ELV::measureSoilHumidity_raw(uint16_t& soilHumidityRaw) {
  if(mSoilHumiditySensorAvailable) {
    const uint32_t reading = mFdc2x1x.getReading(0);
    if(reading == FDC2x1x::INVALID_READING) {
      return HUMIDITY_MEASUREMENT_FAILED;
    }
    soilHumidityRaw = reading >> 16;
    return NO_ERROR;
  } else {
    soilHumidityRaw = FDC2x1x::INVALID_READING;
  }
  return HUMIDITY_SENSOR_NOT_AVAILABLE;
}

Read soil temperature and soil humidity from ELV Somo1 sensor.

Refer to comment in function setupSomo1() of example sketch 'PrintSensorValues' for individual calibration of your sensor.
The assumption is, that the sensor operates almost linear within the applied bias point. It is very likely that this is the case,
since the ELV Homematic calibration dialog also offers only 2 calibration points with a similar range.

The library depends on other libraries: Sensirion, I2C, SHT4x, FDC2x1x.
The library has been tested so far on a XIAO-nRF52840.
  
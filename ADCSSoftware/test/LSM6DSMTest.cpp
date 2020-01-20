//
// test/LSM6DSMTest.cpp
// ADCS
//
// Contributors:
//   Kyle Krol  kpk63@cornell.edu
//
// Pathfinder for Autonomous Navigation
// Space Systems Design Studio
// Cornell Univeristy
//

#define DEBUG
#include <debug.hpp>

#include <i2c_t3.h>
#include <Arduino.h>
#include <LSM6DSM.hpp>

LSM6DSM gyro;

void setup() {
  DEBUG_init(9600)
  Wire2.begin(I2C_MASTER, 0x00, I2C_PINS_3_4, I2C_PULLUP_EXT, 400000);
  delay(5000); // Allow gyroscope to start up

  gyro.setup(&Wire2, LSM6DSM::ADDR::GND, 10000);
  if (gyro.reset()) {
    DEBUG_printlnF("Gyroscope initialized");
  } else {
    DEBUG_printlnF("Gyroscope initialization failed")
    while (1);
  }
}

unsigned long time_stamp = 0;

void loop() {

  while (!gyro.is_ready()) delay(2);

  if (gyro.read()) {
    if (millis() - 2000 > time_stamp) {
      time_stamp = millis();
      DEBUG_println("omega = " + String(gyro.get_omega_x()) + "," + String(gyro.get_omega_y()) + "," +
          String(gyro.get_omega_z()))
      DEBUG_println("temp = " + String(gyro.get_temp()));
    }
  } else {
    DEBUG_println("ERROR")
  }
}
#include "ICM42688.h"

static const uint8_t IMU_CS_PIN = PB12;
static const uint8_t IMU_INT_PIN = PA1;

static const uint8_t IMU_MOSI_PIN = PB15;
static const uint8_t IMU_MISO_PIN = PB14;
static const uint8_t IMU_SCLK_PIN = PB13;

static SPIClass IMU_spi = SPIClass(IMU_MOSI_PIN, IMU_MISO_PIN, IMU_SCLK_PIN);

// an ICM42688 object with the ICM42688 sensor on SPI bus 0 and chip select pin CS_PIN
ICM42688 imu(IMU_spi, IMU_CS_PIN);

volatile bool dataReady = false;

void setup() {
  // serial to display data
  Serial.begin(115200);
  while(!Serial) {}

  // start communication with IMU
  int status = imu.begin();
  if (status < 0) {
    Serial.println("IMU initialization unsuccessful");
    Serial.println("Check IMU wiring or try cycling power");
    Serial.print("Status: ");
    Serial.println(status);
    while(1) {}
  }

  // attaching the interrupt to microcontroller pin INT_PIN
  pinMode(IMU_INT_PIN, INPUT);
  attachInterrupt(IMU_INT_PIN, setImuFlag, RISING);

  // set output data rate
  imu.setAccelODR(ICM42688::odr32k);
  imu.setGyroODR(ICM42688::odr32k);

  // enabling the data ready interrupt
  imu.enableDataReadyInterrupt(); 
}

void loop() {
  if (!dataReady) return;

  dataReady = false;

  // read the sensor
  imu.getAGT();
  
  // display the raw data
  Serial.print(imu.getAccelX_count());
  Serial.print("\t");
  Serial.print(imu.getAccelY_count());
  Serial.print("\t");
  Serial.print(imu.getAccelZ_count());
  Serial.print("\t");
  Serial.print(imu.getGyroX_count());
  Serial.print("\t");
  Serial.print(imu.getGyroY_count());
  Serial.print("\t");
  Serial.println(imu.getGyroZ_count());
}

void setImuFlag() {
  dataReady = true;
}

#include <SPI.h>

#define PIN_SPI_SS   10
#define PIN_SPI_RST  9
#define PIN_IRQ      2
#define CHANNEL      5
#define NODE_ID      1

byte node_address[8] = {0xDE, 0xAD, 0xBE, 0xEF, 0x01, 0x23, 0x45, 0x67};

void setup() {
  Serial.begin(115200);
  while (!Serial);

  pinMode(PIN_SPI_SS, OUTPUT);
  pinMode(PIN_SPI_RST, OUTPUT);
  pinMode(PIN_IRQ, INPUT);

  digitalWrite(PIN_SPI_SS, HIGH);
  digitalWrite(PIN_SPI_RST, LOW);
  delay(50);
  digitalWrite(PIN_SPI_RST, HIGH);
  delay(50);

  SPI.begin();
  SPI.setBitOrder(MSBFIRST);
  SPI.setDataMode(SPI_MODE0);
  SPI.setClockDivider(SPI_CLOCK_DIV16);
}

void loop() {
    byte tx_buffer[16];
    byte rx_buffer[16];

  // Send a ranging request to all other nodes
    for (byte i = 1; i <= 8; i++) {
        if (i == NODE_ID) {
          continue;
        }

        // Construct the ranging request frame
        tx_buffer[0] = 0x0A;
        tx_buffer[1] = 0x41;
        tx_buffer[2] = i;   //dafuq?
        tx_buffer[3] = 0xC5;
        tx_buffer[4] = 0x02;
        tx_buffer[5] = 0x20;
        tx_buffer[6] = 0x03;
        tx_buffer[7] = 0x00;
        tx_buffer[8] = 0x00;
        tx_buffer[9] = 0x00;
        tx_buffer[10] = 0x00;
        tx_buffer[11] = 0x00;
        tx_buffer[12] = 0x00;
        tx_buffer[13] = 0x00;
        tx_buffer[14] = 0x00;
        tx_buffer[15] = 0x00;

        // Select the node to send to
        digitalWrite(PIN_SPI_SS, LOW);
        delayMicroseconds(10);
        SPI.transfer(0x01);
        SPI.transfer(0x00);
        delayMicroseconds(10);

        // Send the request frame
        for (byte j = 0; j < 16; j++) {
            rx_buffer[j] = SPI.transfer(tx_buffer[j]);
        }
        delayMicroseconds(10);
        digitalWrite(PIN_SPI_SS, HIGH);

        // Wait for the response frame
        unsigned long start_time = micros();
        while (digitalRead(PIN_IRQ) == LOW && (micros() - start_time) < 20000) {
            delayMicroseconds(10);
        }

        // Read the response frame
        if (digitalRead(PIN_IRQ) == HIGH) {
            digitalWrite(PIN_SPI_SS, LOW);
            delayMicroseconds(10);
            SPI.transfer(0x00);
            SPI.transfer(0x00);
            for (byte j = 0; j < 14; j++) {
                rx_buffer[j] = SPI.transfer(0x00);
            }
            delayMicroseconds(10);
            digitalWrite(PIN_SPI_SS, HIGH);
            // Parse the response frame and calculate the distance
            if (rx_buffer[0] == 0x10 && rx_buffer[1] == 0x41 && rx_buffer[2] == NODE_ID && rx_buffer[3] == i && rx_buffer[4] == 0x48) {
                unsigned long t_tx = (unsigned long)rx_buffer[5] | (unsigned long)rx_buffer[6] << 8 | (unsigned long)rx_buffer[7] << 16 | (unsigned long)rx_buffer[8] << 24;
                unsigned long t_rx = (unsigned long)rx_buffer[9] | (unsigned long)rx_buffer[10] << 8 | (unsigned long)rx_buffer[11] << 16 | (unsigned long)rx_buffer[12] << 24;
                unsigned long time_of_flight = (t_rx - t_tx) / 2;
                double distance = time_of_flight * 0.0343;

                // Print the distance to the other node
                Serial.print("Distance to node ");
                Serial.print(i);
                Serial.print(": ");
                Serial.print(distance);
                Serial.println(" m");
            }
        }
    }
}
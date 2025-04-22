#include <SPI.h>
#include <iostream>
#include <cmath>

// Constants for TDOA ranging
#define ANCHOR_A 0
#define ANCHOR_B 1
#define ANCHOR_C 2
#define ANCHOR_D 3
#define SPEED_OF_LIGHT 299792458.0

// DW1000 register addresses
#define TX_BUFFER        0x09
#define TX_FCTRL         0x0E
#define TX_FCTRL_LEN_8   0x08
#define TX_TIME          0x10
#define SYS_CTRL         0x0D
#define SYS_STATUS       0x0F
#define SYS_STATUS_TXFRS (1 << 8)
#define SYS_STATUS_RXDFR (1 << 7)
#define SYS_STATUS_RXFCG (1 << 6)
#define SYS_MASK         0x0E
#define SYS_MASK_MTXFRS  (1 << 8)
#define SYS_MASK_MRXPHE  (1 << 7)
#define SYS_MASK_MRXDFR  (1 << 6)
#define SYS_MASK_MRXFCG  (1 << 5)
#define SYS_MASK_MLDEERR (1 << 4)
#define SYS_MASK_MRXSFDTO (1 << 3)
#define SYS_MASK_MLWERR  (1 << 2)

// Anchor positions
double anchorPositions[4][3] = {
  {0.0, 0.0, 0.0},   // Anchor A
  {1.0, 0.0, 0.0},   // Anchor B
  {1.0, 1.0, 0.0},   // Anchor C
  {0.0, 1.0, 0.0}    // Anchor D
};

// Initialize SPI communication pins
const int ssPin = 10;
const int rstPin = 9;
const int mosiPin = 11;
const int misoPin = 12;
const int sckPin = 13;

void setup() {
    // Start serial communication
    Serial.begin(9600);
    while (!Serial) {}

    // Initialize SPI communication
    SPI.begin();
    SPI.setClockDivider(SPI_CLOCK_DIV2);
    SPI.setBitOrder(MSBFIRST);
    SPI.setDataMode(SPI_MODE0);

    // Initialize DW1000 module
    pinMode(ssPin, OUTPUT);
    pinMode(rstPin, OUTPUT);
    digitalWrite(ssPin, HIGH);
    digitalWrite(rstPin, LOW);
    delay(10);
    digitalWrite(rstPin, HIGH);
    delay(10);

    // Reset DW1000 module
    writeRegister(SYS_CTRL, 0x00);
    delay(10);
    writeRegister(SYS_CTRL, 0x04);
    delay(10);

    // Configure DW1000 module for TDOA ranging
    writeRegister(TX_FCTRL, TX_FCTRL_LEN_8);
    writeRegister(SYS_MASK, SYS_MASK_MTXFRS);
}

void loop() {
    // Get current anchor positions
    double anchorA[3] = {anchorPositions[ANCHOR_A][0], anchorPositions[ANCHOR_A][1], anchorPositions[ANCHOR_A][2]};
    double anchorB[3] = {anchorPositions[ANCHOR_B][0], anchorPositions[ANCHOR_B][1], anchorPositions[ANCHOR_B][2]};
    double anchorC[3] = {anchorPositions[ANCHOR_C][0], anchorPositions[ANCHOR_C][1], anchorPositions[ANCHOR_C][2]};
    double anchorD[3] = {anchorPositions[ANCHOR_D][0], anchorPositions[ANCHOR_D][1], anchorPositions[ANCHOR_D][2]};

    // Initiate TDOA ranging
    sendPulse(ANCHOR_A);
    sendPulse(ANCHOR_B);
    sendPulse(ANCHOR_C);
    sendPulse(ANCHOR_D);

    // Wait for response from tag
    while (!checkStatus(SYS_STATUS_RXDFR)) {}

    // Get timestamp for each received pulse
    unsigned long long timestampA = getRxTimestamp(ANCHOR_A);
    unsigned long long timestampB = getRxTimestamp(ANCHOR_B);
    unsigned long long timestampC = getRxTimestamp(ANCHOR_C);
    unsigned long long timestampD = getRxTimestamp(ANCHOR_D);

    // Calculate time difference of arrival (TDOA) between each pair of anchors
    double tdoaAB = ((double)(timestampA - timestampB) / SPEED_OF_LIGHT) * 1e9;
    double tdoaAC = ((double)(timestampA - timestampC) / SPEED_OF_LIGHT) * 1e9;
    double tdoaAD = ((double)(timestampA - timestampD) / SPEED_OF_LIGHT) * 1e9;
    double tdoaBC = ((double)(timestampB - timestampC) / SPEED_OF_LIGHT) * 1e9;
    double tdoaBD = ((double)(timestampB - timestampD) / SPEED_OF_LIGHT) * 1e9;
    double tdoaCD = ((double)(timestampC - timestampD) / SPEED_OF_LIGHT) * 1e9;

    // Calculate tag position based on TDOA ranging result
    double x, y, z;
    calculatePosition(x, y, z, anchorA, anchorB, anchorC, anchorD, tdoaAB, tdoaAC, tdoaAD, tdoaBC, tdoaBD, tdoaCD);

    // Print tag position
    Serial.print("Tag position: (");
    Serial.print(x);
    Serial.print(", ");
    Serial.print(y);
    Serial.print(", ");
    Serial.print(z);
    Serial.println(")");

    // Delay for a short period
    delay(100);
}

    // Write a value to a register on the DW1000 module
void writeRegister(byte reg, byte value) {
    digitalWrite(ssPin, LOW);
    SPI.transfer(reg | 0x80);
    SPI.transfer(value);
    digitalWrite(ssPin, HIGH);
}

    // Read a value from a register on the DW1000 module
byte readRegister(byte reg) {
    byte value;
    digitalWrite(ssPin, LOW);
    SPI.transfer(reg);
    value = SPI.transfer(0x00);
    digitalWrite(ssPin, HIGH);
    return value;
}

// Send a pulse from an anchor
void sendPulse(int anchor) {
    // Set transmit time to 0
    byte txTime[5] = {0x00, 0x00, 0x00, 0x00, 0x00};
    writeRegister(TX_TIME, txTime, 5);

    // Set transmit data buffer to pulse
    byte txBuffer[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01};
    writeRegister(TX_BUFFER, txBuffer, 8);

    // Set transmit mode
    writeRegister(SYS_CTRL, 0x09);

    // Wait for transmit complete
    while (!checkStatus(SYS_STATUS_TXFRS));
}

// Check the status of a register on the DW1000 module
bool checkStatus(byte reg, byte mask) {
    byte value = readRegister(reg);
    return ((value & mask) == mask);
}

// Check the status of a system status register on the DW1000 module
bool checkStatus(byte mask) {
    return checkStatus(SYS_STATUS, mask);
}

// Get the receive timestamp for a received pulse on a specific anchor
unsigned long long getRxTimestamp(int anchor) {
    // Get timestamp value
    byte rxTime[5];
    readRegister(RX_TIME, rxTime, 5);
    unsigned long long timestamp = ((unsigned long long)rxTime);

    // Get corrected timestamp
    double distance = calculateDistance(anchorPositions[anchor], tagPosition);
    double correction = distance / SPEED_OF_LIGHT * 1e9;
    timestamp -= (unsigned long long)correction;

    return timestamp;
}

void calculateDistance(double& PointA, double& PointB) {
    return sqrt(pow(PointB[0] - PointA[0], 2) + pow(PointB[1] - PointA[1], 2) + pow(PointB[2] - PointA[2], 2));
}

// Calculate the position of the tag based on TDOA ranging result
void calculatePosition(double& x, double& y, double& z, int anchorA, int anchorB, int anchorC, int anchorD,
    double tdoaAB, double tdoaAC, double tdoaAD, double tdoaBC, double tdoaBD, double tdoaCD) {
    // Calculate tag position based on TDOA ranging result
    double a[3] = {anchorPositions[anchorA][0], anchorPositions[anchorA][1], anchorPositions[anchorA][2]};
    double b[3] = {anchorPositions[anchorB][0], anchorPositions[anchorB][1], anchorPositions[anchorB][2]};
    double c[3] = {anchorPositions[anchorC][0], anchorPositions[anchorC][1], anchorPositions[anchorC][2]};
    double d[3] = {anchorPositions[anchorD][0], anchorPositions[anchorD][1], anchorPositions[anchorD][2]};

    double tdoa1 = tdoaAB;
    double tdoa2 = tdoaAC;
    double tdoa3 = tdoaAD;
    double tdoa4 = tdoaBC;
    double tdoa5 = tdoaBD;
    double tdoa6 = tdoaCD;

    double A[3], B[3], C[3], D[3], E[3], F[3], P[3], Q[3], R[3];
    double d1, d2, d3, d4, d5, d6;

    // First Anchor A
    A[0] = 0;
    A[1] = 0;
    A[2] = 0;

    // Second Anchor B
    B[0] = calculateDistance(a, b);
    B[1] = 0;
    B[2] = 0;

    // Third Anchor C
    d1 = calculateDistance(a, c);
    d2 = calculateDistance(b, c);
    d3 = calculateDistance(a, b);
    C[0] = (d1 * d1 - d2 * d2 + d3 * d3) / (2 * d3);
    C[1] = sqrt(d1 * d1 - C[0] * C[0]);
    C[2] = 0;

    // Fourth Anchor D
    d1 = calculateDistance(a, d);
    d2 = calculateDistance(b, d);
    d3 = calculateDistance(c, d);
    d4 = calculateDistance(a, b);
    d5 = calculateDistance(a, c);
    d6 = calculateDistance(b, c);
    D[0] = ((d1 * d1 - d2 * d2 + d4 * d4) / (2 * d4) - (d3 * d3 - d2 * d2 + d6 * d6) / (2 * d6)) / (1 - (d5 * d5) / (d6 * d6));
    D[1] = sqrt(d1 * d1 - D[0] * D[0] - (D[0] - C[0]) * (D[0] - C[0]));
    D[2] = 0;

    // Calculate E and F
    double a_x = a[0], a_y = a[1], a_z = a[2];
    double b_x = b[0], b_y = b[1], b_z = b[2];
    double c_x = c[0], c_y = c[1], c_z = c[2];
    double d_x = d[0], d_y = d[1], d_z = d[2];

    double m1, m2, m3, n1, n2, n3;
    m1 = (b_x - a_x) / (b_z - a_z);
    m2 = (b_y - a_y) / (b_z - a_z);
    m3 = (b_z - a_z) / (b_z - a_z);
    n1 = (d_x - c_x) / (d_z - c_z);
    n2 = (d_y - c_y) / (d_z - c_z);
    n3 = (d_z - c_z) / (d_z - c_z);

    double p1, p2, p3, q1, q2, q3;
    double t1, t2, t3, t4, t5, t6;
    t1 = tdoa1;
    t2 = tdoa2 - tdoa1;
    t3 = tdoa3 - tdoa1;
    t4 = tdoa4 - tdoa1;
    t5 = tdoa5 - tdoa1;
    t6 = tdoa6 - tdoa1;

    p1 = t1 * m1 - t2 * n1;
    p2 = t1 * m2 - t2 * n2;
    p3 = t1 * m3 - t2 * n3;
    q1 = t1 * (m1 * m1 + m2 * m2 + m3 * m3) - t2 * (n1 * n1 + n2 * n2 + n3 * n3);
    q2 = t3 * m1 - t4 * n1;
    q3 = t3 * m2 - t4 * n2;
    t4 = t4 * (n1 * n1 + n2 * n2 + n3 * n3) - t3 * (m1 * m1 + m2 * m2 + m3 * m3);
    p3 = (t4 * p3 - t2 * q3) / (q2 * p1 - p2 * q1);
    p2 = (t2 * p3 - t4 * p2) / (q2 * p1 - p2 * q1);
    p1 = (t1 - n1 * p3 - n2 * p2) / m1;

    t5 = t5 * m1 - t6 * n1;
    t6 = t6 * (n1 * n1 + n2 * n2 + n3 * n3) - t5 * (m1 * m1 + m2 * m2 + m3 * m3);
    p2 = (t6 * p2 - t5 * p3) / (q3 * p2 - q2 * p3);
    p3 = (t5 * p2 - t6 * p1) / (q3 * p2 - q2 * p3);
    p1 = (t3 - n1 * p3 - n2 * p2) / m1;

    // Calculate the position of the target point
    double x = a_x + p1 * (b_x - a_x) / d1 + p2 * (c_x - a_x) / d2 + p3 * (d_x - a_x) / d3;
    double y = a_y + p1 * (b_y - a_y) / d1 + p2 * (c_y - a_y) / d2 + p3 * (d_y - a_y) / d3;
    double z = a_z + p1 * (b_z - a_z) / d1 + p2 * (c_z - a_z) / d2 + p3 * (d_z - a_z) / d3;

    // Return the position of the target point
    return std::make_tuple(x, y, z);
}


// The `calculateDistance()` function is used to calculate the distance between two points, given their coordinates. 
// The `calculatePosition()` function takes six timestamps and six anchor positions, and returns the 3D coordinates of the target point. 

// Note that the code assumes that the first anchor is the reference anchor, and the timestamps are relative to this anchor. 
// If the anchors have different clock drifts, this will cause errors in the calculated position. 
// In this case, you may need to perform clock synchronization between the anchors to improve the accuracy of the ranging.
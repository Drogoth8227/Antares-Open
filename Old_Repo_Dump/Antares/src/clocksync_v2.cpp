// Define pins for DWM1000 module
#define DWM1000_IRQ 2
#define DWM1000_RST 4
#define DWM1000_SS 10

// Define clock sync message
#define MSG_TAG 0xDEADBEEF
#define MSG_LEN 8
uint8_t msg[MSG_LEN] = { 0 };

// Define variables for clock synchronization
bool synced = false;
uint32_t timestamp_local = 0;
uint32_t timestamp_remote = 0;

void setup() {
  // Initialize serial communication
  Serial.begin(9600);
  
  // Initialize DWM1000 module
  pinMode(DWM1000_IRQ, INPUT);
  pinMode(DWM1000_RST, OUTPUT);
  digitalWrite(DWM1000_RST, LOW);
  delay(10);
  digitalWrite(DWM1000_RST, HIGH);
  delay(10);
  SPI.begin();
  SPI.setBitOrder(MSBFIRST);
  SPI.setDataMode(SPI_MODE0);
  SPI.setClockDivider(SPI_CLOCK_DIV16);
  
  // Configure DWM1000 module
  dwt_initialise(DWT_LOADUCODE);
  dwt_configure(&dwt_config);
  dwt_setleds(1);
  dwt_setrxtimeout(0);
  dwt_setinterrupt(DWT_INT_RFCG, 1);
  attachInterrupt(digitalPinToInterrupt(DWM1000_IRQ), handleInterrupt, RISING);
}

void loop() {
  // Wait for clock synchronization
  while (!synced) {
    delay(1000);
    Serial.println("Waiting for clock sync...");
  }
  
  // Send clock sync message
  msg[0] = (uint8_t) (MSG_TAG >> 24);
  msg[1] = (uint8_t) (MSG_TAG >> 16);
  msg[2] = (uint8_t) (MSG_TAG >> 8);
  msg[3] = (uint8_t) (MSG_TAG >> 0);
  msg[4] = (uint8_t) (timestamp_local >> 24);
  msg[5] = (uint8_t) (timestamp_local >> 16);
  msg[6] = (uint8_t) (timestamp_local >> 8);
  msg[7] = (uint8_t) (timestamp_local >> 0);
  dwt_writetxdata(MSG_LEN, msg, 0);
  dwt_writetxfctrl(MSG_LEN, 0);
  dwt_starttx(DWT_START_TX_IMMEDIATE);
  
  // Wait for response
  uint32_t status = dwt_read32bitreg(SYS_STATUS_ID);
  while (!(status & SYS_STATUS_RXFCG)) {
    if (status & SYS_STATUS_RXRFTO) {
      Serial.println("RX timeout");
      dwt_rxreset();
      break;
    }
    status = dwt_read32bitreg(SYS_STATUS_ID);
  }
  
  // Handle response
  if (status & SYS_STATUS_RXFCG) {
    uint32_t frame_len = dwt_read32bitreg(RX_FINFO_ID) & RX_FINFO_RXFLEN_MASK;
    if (frame_len == MSG_LEN && dwt_read32bitreg(RX_BUFFER_ID) == MSG_TAG) {
        timestamp_remote = ((uint32_t) dwt_read32bitreg(RX_BUFFER_ID + 4)) & 0x00FFFFFF;
        Serial.print("Remote timestamp: ");
        Serial.println(timestamp_remote);
  
        // Calculate clock offset
        uint32_t timestamp_diff = (timestamp_remote > timestamp_local) ? 
                                    (timestamp_remote - timestamp_local) :
                                    (timestamp_local - timestamp_remote);
        uint32_t clock_offset = timestamp_diff / 2;
        Serial.print("Clock offset: ");
        Serial.println(clock_offset);
  
        // Apply clock offset
        uint32_t sys_time = dwt_readsystimestamphi32();
        uint64_t adjusted_time = ((uint64_t) sys_time) + ((uint64_t) clock_offset);
        dwt_settimeadjust(adjusted_time);
  
        // Reset timestamp variables
        timestamp_local = 0;
        timestamp_remote = 0;
  
        // Wait for next sync message
        synced = false;
        }
    }
}

void handleInterrupt() {
    uint32_t status = dwt_read32bitreg(SYS_STATUS_ID);
    if (status & SYS_STATUS_RXFCG) {
        dwt_readrxdata(msg, MSG_LEN, 0);
        if (msg[0] == (uint8_t) (MSG_TAG >> 24) &&
            msg[1] == (uint8_t) (MSG_TAG >> 16) &&
            msg[2] == (uint8_t) (MSG_TAG >> 8) &&
            msg[3] == (uint8_t) (MSG_TAG >> 0)) {
                timestamp_local = ((uint32_t) msg[4] << 24) |
                ((uint32_t) msg[5] << 16) |
                ((uint32_t) msg[6] << 8) |
                ((uint32_t) msg[7] << 0);
                synced = true;
                }
            dwt_rxreset();
            dwt_setrxtimeout(0);
            dwt_rxenable(DWT_START_RX_IMMEDIATE);
        } else {
            dwt_syncrxbuf();
            }
}
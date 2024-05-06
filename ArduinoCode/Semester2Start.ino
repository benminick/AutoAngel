
#include <Arduino.h>
#include <Wire.h>

// Hardware Serial objects for communication with host PC and Bluetooth module
#define HOST_PC_SERIAL Serial
#define BLUETOOTH_SERIAL Serial1

int weightPin1 = A0;   // select the input pin for the potentiometer
int weightValue1 = 0;  // variable to store the value coming from the sensor

int weightPin2 = A3;   // select the input pin for the potentiometer
int weightValue2 = 0;

int tempPin1 = A1;
int temperature1 = 0;
float degreesC1 = 0;

int tempPin2 = A2;
int temperature2 = 0;
float degreesC2 = 0;

int tempPin3 = A4;
int temperature3 = 0;
float degreesC3 = 0;

char child = 0;

// Buffers
#define BUFF_SIZE 512
char rx_buff_from_BlueTooth_module[BUFF_SIZE];
char tx_buff_to_BlueTooth_module[] = "1\r\n";

char tx_buff_to_HostPC[BUFF_SIZE];
char rx_buff_from_HostPC[BUFF_SIZE];

// Functions related to comm. with host PC via UART2;
void readFromHostPC(uint32_t timeout);

// Functions related to Bluetooth (BT) module;
void readFromBT(uint32_t timeout, int debug);
void sendCommandToBT(const char *command, uint32_t timeout, int debug);
void BT_module_rename(void);

void setup() {
  HOST_PC_SERIAL.begin(115200);  // Initialize serial communication with host PC
  BLUETOOTH_SERIAL.begin(9600);  // Initialize serial communication with Bluetooth module

}

void loop() {
  weightValue1 = analogRead(weightPin1);
  weightValue2 = analogRead(weightPin2);
  temperature1 = analogRead(tempPin1);
  temperature2 = analogRead(tempPin2);
  temperature3 = analogRead(tempPin3);

  delay(weightValue1);
  Serial.println("Weight Sensor1: ");
  Serial.println(weightValue1);
  Serial.println("Weight Sensor2: ");
  Serial.println(weightValue2);

  degreesC1 = ((temperature1 * 0.003223) - 0.5) * 100;
  degreesC2 = ((temperature2 * 0.003223) - 0.5) * 100;
  degreesC3 = ((temperature3 * 0.003223) - 0.5) * 100;

  Serial.print("degrees C1 = ");
  Serial.println(degreesC1);
  Serial.print("degrees C2 = ");
  Serial.println(degreesC2);
  Serial.print("degrees C3 = ");
  Serial.println(degreesC3);

memset(tx_buff_to_BlueTooth_module, 0, sizeof(tx_buff_to_BlueTooth_module));

if ((degreesC1 > 20 || degreesC2 > 20 || degreesC1 > 20) && (weightValue1 > 175 || weightValue2 > 175))
{
      strcpy(tx_buff_to_BlueTooth_module, "1\r\n");
      child = 1;
}

else 
{
  strcpy(tx_buff_to_BlueTooth_module, "0\r\n");
  child = 0;
}

  Serial.print("child: ");
  Serial.println(child);

  BLUETOOTH_SERIAL.print(tx_buff_to_BlueTooth_module);
  sprintf(tx_buff_to_HostPC, "%s", "Now sending message via BT module...\r\n");
  HOST_PC_SERIAL.print(tx_buff_to_HostPC);
  delay(5000);
}

void readFromHostPC(uint32_t timeout) {
  int i = 0;

  for (i = 0; i < BUFF_SIZE; i++) {
    rx_buff_from_HostPC[i] = 0;
  }

  HOST_PC_SERIAL.setTimeout(timeout);
  HOST_PC_SERIAL.readBytes(rx_buff_from_HostPC, BUFF_SIZE);
}

void readFromBT(uint32_t timeout, int debug) {
  int i = 0;

  for (i = 0; i < BUFF_SIZE; i++) {
    rx_buff_from_BlueTooth_module[i] = 0;
  }

  BLUETOOTH_SERIAL.setTimeout(timeout);
  BLUETOOTH_SERIAL.readBytes(rx_buff_from_BlueTooth_module, BUFF_SIZE);

  if (debug) {
    HOST_PC_SERIAL.print("\r\n<-------- START received data from BT -------->\r\n");
    HOST_PC_SERIAL.print(rx_buff_from_BlueTooth_module);
    HOST_PC_SERIAL.print("\r\n<-------- END received data from BT -------->\r\n");
  }
}

void sendCommandToBT(const char *command, uint32_t timeout, int debug) {
  int i = 0;

  for (i = 0; i < BUFF_SIZE; i++) {
    rx_buff_from_BlueTooth_module[i] = 0;
  }

  BLUETOOTH_SERIAL.print(command);

  BLUETOOTH_SERIAL.setTimeout(timeout);
  BLUETOOTH_SERIAL.readBytes(rx_buff_from_BlueTooth_module, BUFF_SIZE);

  if (debug) {
    HOST_PC_SERIAL.print("\r\n<-------- START response to sendCommandToBT() -------->\r\n");
    HOST_PC_SERIAL.print(rx_buff_from_BlueTooth_module);
    HOST_PC_SERIAL.print("\r\n<-------- END response to sendCommandToBT() -------->\r\n");
  }
}

void BT_module_rename() {
  sendCommandToBT("AT\r\n", 3000, 1); // timeout of 3000 ms should be enough; debug set to true;
  delay(1000);
  sendCommandToBT("AT+VERSION\r\n", 3000, 1); // ask version of firmware;
  delay(1000);
  sendCommandToBT("AT+NAME=JULIA-NUCLEO\r\n", 3000, 1); // change name of BT module to CRIS-NUCLEO
  delay(1000);
}
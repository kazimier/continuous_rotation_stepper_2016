#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
#include <OSCBundle.h>
#include "EulerStepCounter.h"
#include <AccelStepper.h>

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED } ;
byte ip[] = { 192, 168, 0, 100 };
int serverPort = 10000; // (incoming port)
int destPort = 9000; 
AccelStepper stepper1(4, 2, 3, 5, 6);
long val1 = 0;

EthernetUDP Udp;
EulerStepCounter stepCounter;



void setup() {
  Serial.begin(250000);
  Ethernet.begin(mac, ip);
  Udp.begin(serverPort);
  stepper1.setMaxSpeed(500.0);
  stepper1.setAcceleration(200.0);

}

void loop() {

  stepper1.run();
  float reading = parseOSCMessage();

  if( reading != -1.0 )
    stepCounter.update(reading);


  Serial.print("Euler:   ");
  Serial.println(reading);
  Serial.print("Stepper: ");
  Serial.println(stepCounter.linear_position());

  delay(50);
}

// either returns the euler angle or -1.0 if packet not received
float parseOSCMessage() {
  OSCMessage msgIn;
  int size;

  if ((size = Udp.parsePacket()) > 0) {
    while (size--)
      msgIn.fill(Udp.read());
    return(msgIn.getFloat(0));
  } else {
    return(-1.0);
    msgIn.route("/compass/2",funcValue1);
  }
  
}


void funcValue1(OSCMessage &msg, int addrOffset ){

  float value = stepCounter.linear_position();
  val1 = (long)(value*200);
  OSCMessage msgOUT("/compass/2");
  stepper1.moveTo(val1);
  //Serial.print("Value = : ");
  //Serial.println(value);

  msgOUT.add(val1);

  Udp.beginPacket(Udp.remoteIP(), destPort);
  msgOUT.send(Udp); // send the bytes
  Udp.endPacket(); // mark the end of the OSC Packet
  msgOUT.empty(); // free space occupied by message
}


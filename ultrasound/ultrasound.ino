/*
 * This example code shows how to get a distance measurement from a HC-SR04 
 * ultrasound sensor and send the distance measured over NB-IoT using the 
 * EE-NBIOT-01 breakout board.
 *
 * In this example the payload is formatted as a JSON message. In real
 * life you would most likely make use of a binary format for
 * compactness.
 *
 *
 * In order to use this code you need to install two libraries in your
 * Arduino IDE:
 *
 *     ArduinoNBIoT 
 *     ------------
 *       which can be found at https://github.com/ExploratoryEngineering/ArduinoNBIoT/releases
 *       Download the ZIP file, navigate your Arduino IDE to 
 *       
 *        Sketch > Include Libraries > Add .ZIP library
 *
 *       And select the ZIP file containing the ArduinoNBIoT release you downloaded.
 *
 *
 *      NewPing
 *      -------
 *       Which can be installed by navigating your Arduino IDE to 
 *
 *         Sketch > Include Libraries > Manage libraries
 *
 *       searching for "NewPing" and then clicking install
 */

#include <TelenorNBIoT.h>
#include <NewPing.h>

// These are the pins that are used to communicate with the ultrasound
// sensor
#define TRIGGER_PIN 8
#define ECHO_PIN    9

// Maximum distance we will be measuring expressed in centimeters.
// Note that if the distance you are measuring exceeds this distance
// any measurement performed will be 0.
#define MAX_DISTANCE_CM                  260

// How long between each message we send.  If you send too often the
// connection might get throttled so keep it civil.
#define MILLISECONDS_BETWEEN_MESSAGES 15000

// These pins are used to communicate with the NB-IoT Breakout board.
#define RX_PIN 10
#define TX_PIN 11

// This creates an instance of the TelenorNBIoT class connected to the
// breakout board via the pins defined above.
TelenorNBIoT nbiot = TelenorNBIoT(RX_PIN, TX_PIN);

// This creates an instance of the NewPing class which will take care
// of talking to the ultrasound sensor
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE_CM);

// If the message you send back is longer than 60 bytes you must
// adjust this setting appropriately.
#define BUFFER_SIZE 50

// Buffer into which we render the response we send back 
char buffer[BUFFER_SIZE];

// The setup function is used to run code when the device boots
// up. This is where you typically do setup and initializations.
void setup() {
    // This initializes the serial console and sets the baud rate to 9600 baud. In order
    // to see what the device is doing while it is running you should open the 
    // Serial Monitor window. (Look under the "Tools" menu).
    Serial.begin(9600);
    while (!Serial) {
        // Wait for the serial connection with the console to be established.
    }

    Serial.println("Booting");
    Serial.println("----------------------------------------");
    
    // Initialize the NB-IoT breakout board
    Serial.println(" - Initializing NB-IoT");
    nbiot.begin();

    // Print some data on the device
    nbiot.i64toa(nbiot.imei(), buffer);
    Serial.print(" - IMEI: ");
    Serial.println(buffer);

    nbiot.i64toa(nbiot.imsi(), buffer);
    Serial.print(" - IMSI: ");
    Serial.println(buffer);

    Serial.println(nbiot.connected() ? " - Connected to network" : " - Not connected to network");

    Serial.println("----------------------------------------");
}
    
// For as long as the device is powered up the loop() function will be called
// over and over.  This is where you will be putting the meat of your code.
void loop() {
    // Send out a ping and record the distance in centimeters.
    unsigned long distance = sonar.ping_cm();
    Serial.print("*** Measured distance to ");
    Serial.print(distance);
    Serial.println("cm");

    // Format the message
    int len = snprintf(buffer, BUFFER_SIZE, "{\"dist\" : %lu}", distance);
    Serial.print("*** Sending message: ");
    Serial.println(buffer);
   
    // Create a socket for sending the message
    if (! nbiot.createSocket()) {
        Serial.println("*** Failed to create socket.  Will retry in 5 seconds");
        delay(5000);
        return;
    }

    // Send the buffer via NB-IoT
    if (nbiot.send(buffer, strlen(buffer))) {
        Serial.println("*** Succeeded in sending message");
    } else {
        Serial. println("*** Failed to send message");
    }
    
    // Close the socket.
    nbiot.closeSocket();
    
    // Wait for 10.000 milliseconds, which is 10 seconds
    Serial.print("*** Waiting for ");
    Serial.print(MILLISECONDS_BETWEEN_MESSAGES / 1000);
    Serial.println(" seconds");
    delay(MILLISECONDS_BETWEEN_MESSAGES);
}
    


//SYSTEM_THREAD(ENABLED);
//SYSTEM_MODE(AUTOMATIC);
//SYSTEM_MODE(SEMI_AUTOMATIC);

//SYSTEM_MODE(MANUAL);
//SYSTEM_THREAD(ENABLED);

int transmittingData = 0;
// -1 to skip loging initial start

// contains timestamp of each event as string
String eventTimestamps = "";
int maxDataLength = 240;
int eventCounter = -1;
String timestampSep = ";";

// timestamp of last publisch in seconds
long lastPublish = 0;
int txCounter = 0;
// timestamp of last event in seconds
long lastEvent = 0;

// defines how often data gets published to the cloud, in seconds
long publishInterval = 30*60;
// definde min. time between two events in seconds
long minEventInterval = 5*60;

// defines how often at least data gets published to the cloud, in seconds
int maxSleep = 4*60*60;

int ext1 = D0;
int ext2 = D1;
int ext3 = D2;

int wakeup = ext1;

// A FuelGauge named 'fuel' for checking on the battery state
FuelGauge fuel;

void setup() {
  logDebug("Setup 1");

  Serial.begin(9600);
  pinMode(ext1, INPUT_PULLUP);
  pinMode(ext2, INPUT_PULLUP);
  pinMode(ext3, INPUT_PULLUP);

  if(Particle.connected()) {
    Particle.disconnect();
  }

  logDebug("Setup 2");
  //Particle.function("tmode", transmitMode);
  //Particle.function("batt", batteryStatus);

}

void loop() {
  //if(digitalRead(extint) == LOW) {
  //  movement++;
  //}

  eventCounter++;
  long currenTimestamp = Time.now();

  long timeSinceLastPublish = currenTimestamp - lastPublish;
  long timeSinceLastEvent = currenTimestamp - lastEvent;

  logDebug("Step 1");
  if ((timeSinceLastEvent > minEventInterval) && eventCounter>0) {
    eventTimestamps = eventTimestamps + timestampSep + String(currenTimestamp);
    lastEvent = currenTimestamp;
    logDebug("Step 1.1 / " + eventTimestamps);
  }

  logDebug("Step  2 / " + String(currenTimestamp) + " / " + String(eventCounter));

  if(((timeSinceLastPublish > publishInterval) && (timeSinceLastEvent > minEventInterval) && (eventCounter>0)) || (strlen(eventTimestamps) > maxDataLength )) {
    logDebug("Step  2.1");

    if(!Particle.connected()) {
      logDebug("Step  2.1.1");
      //Cellular.on();
      //delay(2000);
      Particle.connect();
      delay(500);
    }

    txCounter ++;
    String data = String(txCounter) + timestampSep + String(eventTimestamps);
    logDebug("Step  2.2 " + String(data));
    Particle.publish("movementCounter: ", data , PRIVATE);

    logDebug("Step  2.3");
    eventTimestamps = "";
    lastPublish = Time.now();
  }

  logDebug("Step  3");
  if(Particle.connected()) {
    logDebug("Step  3.1");
    Particle.disconnect();
    //Cellular.off();
	  delay(1000);
  }

  logDebug("Step  4");

  System.sleep(wakeup, CHANGE, maxSleep);

}

void logDebug(String message) {
  if(strlen(message) > 0) {
    if(transmittingData) {
      delay(500);
      Particle.publish("debug", message, PRIVATE);
    }
    else {
      Serial.println(message);
    }
  }
}

// setting logging mode
int transmitMode(String command) {
    transmittingData = atoi(command);
    return transmittingData;
}

// Lets you remotely check the battery status by calling the function "batt"
// Triggers a publish with the info (so subscribe or watch the dashboard)
// and also returns a '1' if there's >10% battery left and a '0' if below
int batteryStatus(String command){
    // Publish the battery voltage and percentage of battery remaining
    // if you want to be really efficient, just report one of these
    // the String::format("%f.2") part gives us a string to publish,
    // but with only 2 decimal points to save space

    Particle.publish("B",
          "v:" + String::format("%.2f",fuel.getVCell()) +
          ",c:" + String::format("%.2f",fuel.getSoC()),
          60, PRIVATE
    );
    // if there's more than 10% of the battery left, then return 1
    if (fuel.getSoC()>10){ return 1;}
    // if you're running out of battery, return 0
    else { return 0;}
}

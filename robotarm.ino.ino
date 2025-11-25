#include <Servo.h>

#define grabPin     14   // D5  (GPIO14)
#define wristPin    12   // D6  (GPIO12)
#define elbowPin     0   // D3  (GPIO0)
#define shoulderPin  4   // D2  (GPIO4)
#define hipPin       5   // D1  (GPIO5)

Servo servo_grab;   
Servo servo_wrist;
Servo servo_elbow;
Servo servo_shoulder;
Servo servo_hip;

// Firstly we define the base angles:
int baseHome     = 0;   
int shoulderHome = 90;
int elbowHome    = 90;
int wristHome    = 90;

// This is a good closing angle for the different devices:
int gripperOpen  = 0;
int gripperClose = 80;
int gripperClose_cylinder = 60;
int gripperClose_cube = 60;
int gripperClose_boat = 80;
int gripperClose_hat = 100;


String inBuffer = "";

void setup() {
  Serial.begin(115200);

  servo_grab.attach(grabPin);
  servo_wrist.attach(wristPin);
  servo_elbow.attach(elbowPin);
  servo_shoulder.attach(shoulderPin);
  servo_hip.attach(hipPin);

  servo_grab.write(gripperClose); 
  servo_wrist.write(wristHome);
  servo_elbow.write(elbowHome);
  servo_shoulder.write(shoulderHome);
  servo_hip.write(baseHome);

  delay(300);
  Serial.println("Setup complete. Send 'cube', 'cylinder', 'hat', 'boat', 'home' or 'set <param> <angle>'"); // the set param is to help figure out the angles in the serial monitor
}

void loop() {
  while (Serial.available() > 0) {
    char c = Serial.read();
    if (c == '\n' || c == '\r') {
      if (inBuffer.length() > 0) {
        handleCommand(inBuffer);
        inBuffer = "";
      }
    } else {
      inBuffer += c;
    }
  }
}

// To make the serial monitor easier to navigate: 
void handleCommand(String cmd) {
  cmd.trim();
  cmd.toLowerCase();

  int firstSpace = cmd.indexOf(' ');
  String keyword = (firstSpace == -1) ? cmd : cmd.substring(0, firstSpace);
  String rest = (firstSpace == -1) ? "" : cmd.substring(firstSpace + 1);

  if (keyword == "cube") {
    Serial.println("Running pickAndPlaceCube()");
    pickAndPlaceCube();
    Serial.println("Done: cube");
  } else if (keyword == "cylinder" || keyword == "square") {
    Serial.println("Running pickAndPlaceCylinder()");
    pickAndPlaceCylinder();
    Serial.println("Done: cylinder");
  } else if (keyword == "hat") {
    Serial.println("Running pickAndPlaceHat()");
    pickAndPlaceHat();
    Serial.println("Done: hat");
  } else if (keyword == "boat") {
    Serial.println("Running pickAndPlaceBoat()");
    pickAndPlaceBoat();
    Serial.println("Done: boat");
  } else if (keyword == "home") {
    goHome();
    Serial.println("Moved to home position");
  } else if (keyword == "set") {
    int sp = rest.indexOf(' ');
    if (sp == -1) {
      Serial.println("Error: use 'set <param> <value>'");
      return;
    }
    String param = rest.substring(0, sp);
    String valS = rest.substring(sp + 1);
    valS.trim();
    int val = valS.toInt();
    setParam(param, val);
  } else {
    Serial.print("Unknown command: ");
    Serial.println(cmd);
    Serial.println("Valid: cube, cylinder, hat, boat, home, set <param> <value>");
  }
}

// For the parameter setting in the serial monitor, we can adjust the angles of the motors easier.
void setParam(String name, int v) {
  if (name == "basehome") {
    baseHome = constrain(v, 0, 180);
    Serial.print("baseHome = "); Serial.println(baseHome);
    servo_hip.write(baseHome);
  } else if (name == "shoulderhome") {
    shoulderHome = constrain(v, 0, 180);
    Serial.print("shoulderHome = "); Serial.println(shoulderHome);
    servo_shoulder.write(shoulderHome);
  } else if (name == "elbowhome") {
    elbowHome = constrain(v, 0, 180);
    Serial.print("elbowHome = "); Serial.println(elbowHome);
    servo_elbow.write(elbowHome);
  } else if (name == "wristhome") {
    wristHome = constrain(v, 0, 180);
    Serial.print("wristHome = "); Serial.println(wristHome);
    servo_wrist.write(wristHome);
  } else if (name == "gripperopen") {
    gripperOpen = constrain(v, 0, 180);
    Serial.print("gripperOpen = "); Serial.println(gripperOpen);

  } else if (name == "gripperclose") {
    gripperClose = constrain(v, 0, 180);
    Serial.print("gripperClose = "); Serial.println(gripperClose);

  } else {
    Serial.print("Unknown param: "); Serial.println(name);
    Serial.println("Valid params: baseHome, shoulderHome, elbowHome, wristHome, gripperOpen, gripperClose");
  }
}

// Our home-base
void goHome() {
  servo_hip.write(baseHome);
  servo_shoulder.write(shoulderHome);
  servo_elbow.write(elbowHome);
  servo_wrist.write(wristHome);
  servo_grab.write(gripperClose); 
  delay(700);
}

void openGripper() {
  servo_grab.write(gripperOpen);
  delay(400);
}

// Make different voids to put in the correct closing-functions
void closeGripper_cube() {
  servo_grab.write(gripperClose_cube);
  delay(500);
}
void closeGripper_cylinder() {
  servo_grab.write(gripperClose_cylinder);
  delay(500);
}
void closeGripper_boat() {
  servo_grab.write(gripperClose_boat);
  delay(500);
}
void closeGripper_hat() {
  servo_grab.write(gripperClose_hat);
  delay(500);
}

// Making a moveto void to be able to make a function to pick up and place the objects
void moveTo(int b, int s, int e, int w, int delayMs=700) {
  // b = base (hip), s = shoulder, e = elbow, w = wrist
  servo_hip.write(b);
  servo_shoulder.write(s);
  servo_elbow.write(e);
  servo_wrist.write(w);
  delay(delayMs);
}

// Pick-and-place
// hip, shoulder, elbow, wrist, delay
void pickAndPlaceCube() {
  openGripper();
  // Go down to pick up the cube
  moveTo(0, 100, 45, 180, 900);
  // Pick up the cube
  closeGripper_cube();
  //Go back to a neutral state so the arm doesnt hit anything on its way to the boxes. 
  moveTo(0, 90, 90, 90, 300);
  // Move the hip
  moveTo(170, 90, 90, 90, 300);
  // Place the cube
  moveTo(170, 180, 0, 110, 700);
  //Leave cube in box
  openGripper();
  // Go back to a neutral state so the arm doesnt hit anything on its way to home state 
  moveTo(170, 90, 90, 90, 300);
  delay(300);
  goHome();
}

void pickAndPlaceCylinder() {
  openGripper();
  // Go down to pick up the cylinder
  moveTo(0, 100, 45, 180, 900);
  // Pick up the cylinder
  closeGripper_cylinder();
  //Go back to a neutral state so the arm doesnt hit anything on its way to the boxes. 
  moveTo(0, 90, 90, 90, 300);
  // Move the hip
  moveTo(170, 90, 90, 90, 300);
  // Place the cylinder
  moveTo(170, 180, 0, 110, 700);
  //Leave cylinder in box
  openGripper();
  // Go back to a neutral state so the arm doesnt hit anything on its way to home state 
  moveTo(170, 90, 90, 90, 300);
  delay(300);
  goHome();
}

void pickAndPlaceHat() {
  moveTo(0, 90, 90, 90, 900);
  openGripper();
  // Go down to pick up the hat
  moveTo(0, 140, 0, 100, 900);
  // Pick up the hat
  closeGripper_hat();
  //Go back to a neutral state so the arm doesnt hit anything on its way to the boxes. 
  moveTo(0, 90, 90, 90, 300);
  // Move the hip
  moveTo(170, 90, 90, 90, 300);
  // Place the hat
  moveTo(170, 180, 0, 110, 700);
  //Leave hat in box
  openGripper();
  // Go back to a neutral state so the arm doesnt hit anything on its way to home state 
  moveTo(170, 90, 90, 90, 300);
  delay(300);
  goHome();
}

// hip, shoulder, elbow, wrist, delay
void pickAndPlaceBoat() {
  moveTo(0, 90, 90, 90, 900);
  // Open the gripper first, so it doesn't hit the hat
  openGripper();
  // Go down to pick up the boat
  moveTo(0, 140, 0, 100, 900);
  // Pick up the boat
  closeGripper_boat();
  //Go back to a neutral state so the arm doesnt hit anything on its way to the boxes. 
  moveTo(0, 90, 90, 90, 300);
  // Move the hip
  moveTo(170, 90, 90, 90, 300);
  // Place the boat
  moveTo(145, 180, 0, 110, 700);
  //Leave boat in box
  openGripper();
  // Go back to a neutral state so the arm doesnt hit anything on its way to home state 
  moveTo(145, 90, 90, 90, 300);
  delay(300);
  goHome();
}

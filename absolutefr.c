
//----------------------------------------BASE CODE DEFINED----------------------------------------//
float deg = 45; // Rotation degree
float s = 0;    // Encoder counts
int sm1 = 0;    // Built-in chanel 1
int sm2 = 0;    // Built-in chanel 2
int r = 0;      // indicator for reading builtin encoder to avoid the reading redundancy
float er;       // Proportional error for PI controller
float eri;      // Integral error for PI controller

int t = 0;  // time in ms
int t0 = 0; // memory for time in ms

int finish = 0; // finish indicator
int rep = 1;    // Repetition indicator


//----------------------------------------USER DEFINED----------------------------------------//
#define ENCODER_SECTIONS 32
#define CW 0
#define CCW 1
int zeroPos, currentPos, previousPos;
float currentAngle;
int encoders[5];
int expectedDirection = CW;


/*light values
ALL BLACK 
834.00    569.00    791.00    730.00    861.00    
834.00    569.00    791.00    730.00    861.00  

ALL WHITES
337.00    68.00    255.00    56.00    469.00    
337.00    68.00    255.00    56.00    468.00    
*/

#define ENC4HI 800
#define ENC4LO 400
#define ENC3HI 500
#define ENC3LO 100
#define ENC2HI 700
#define ENC2LO 300
#define ENC1HI 700
#define ENC1LO 100
#define ENC0HI 800
#define ENC0LO 500

void setup()
{

    Serial.begin(250000); // Baud rate of communication

    Serial.println("Enter the desired rotation in degree.");

    while (Serial.available() == 0) // Obtaining data from user
    {
        // Wait for user input
    }

    deg = Serial.readString().toFloat(); // Reading the Input string from Serial port.
    if (deg < 0)
    {
        analogWrite(3, 255); // change the direction of rotation by applying voltage to pin 3 of arduino
        expectedDirection = CCW;
    }
    deg = abs(deg);

    pinMode(9, INPUT); 
    pinMode(10, INPUT);
    pinMode(11, INPUT); 
    pinMode(12, INPUT);
    pinMode(13, INPUT); 

    zeroPosition(); //
}

float kp = .6 * 90 / deg; // proportional gain of PI
float ki = .02;           // integral gain of PI

void loop()
{
    // put your main code here, to run repeatedly:

    t = millis();                      // reading time
    t0 = t;                            // sving the current time in memory
    while (t < t0 + 4000 && rep <= 10) // let the code to ran for 4 seconds each with repetitions of 10
    {

        if (t % 10 == 0) // PI controller that runs every 10ms
        {
            if (s < deg * 114 * 2 / 360)
            {
                er = deg - s * 360 / 228;
                eri = eri + er;
                analogWrite(6, kp * er + ki * eri);
            }

            if (s >= deg * 228 / 360)
            {
                analogWrite(6, 0);
                eri = 0;
            }
            delay(1);
        }

        sm1 = digitalRead(7); // reading chanel 1
        sm2 = digitalRead(8); // reading chanel 2

        if (sm1 != sm2 && r == 0)
        { // counting the number changes for both chanels
            s = s + 1;
            r = 1; // this indicator wont let this condition, (sm1 != sm2), to be counted until the next condition, (sm1 == sm2), happens
        }
        if (sm1 == sm2 && r == 1)
        {
            s = s + 1;
            r = 0; // this indicator wont let this condition, (sm1 == sm2), to be counted until the next condition, (sm1 != sm2), happens
        }

        t = millis(); // updating time
        finish = 1;   // cghanging finish indicator
    }

    if (finish == 1)
    {                  // this part of the code is for displaying the result
        delay(500);    // half second delay
        rep = rep + 1; // increasing the repetition indicator

        readEncoders();
        currentPos = grayCodetoDec(encoders);
        currentAngle = relativeAngle(currentPos, previousPos);
        
        Serial.print("current position number: ");
        Serial.println(currentPos);
        Serial.print("previous position number: ");
        Serial.println(previousPos);

        Serial.print("shaft position from optical absolute sensor from home position: ");
        Serial.println(absoluteAngle(currentPos));

        Serial.print("shaft displacement from optical absolute sensor: ");
        Serial.println(currentAngle);

        Serial.print("Shaft displacement from motor's builtin encoder: ");
        Serial.println(s * 360 / 228); // every full Revolution of the shaft is associated with 228 counts of builtin
                                       // encoder so to turn it to degre we can use this formula (s * 360 / 228), "s" is the number of  built-in encoder counts

        float Error = currentAngle - s * 360 / 228;
        Serial.print("Error :");
        Serial.println(Error); // displaying error

        Serial.print("direction expected: ");
            if (expectedDirection == CW)
            {
                Serial.print("CW");
            }
            else
            {
                Serial.print("CCW");
            }
            Serial.print("  ,   ");

            Serial.print("direction read by absolute sensor:  ");
            if (direction(currentPos, previousPos) == CW)
            {
                Serial.print("CW");
            }
            else
            {
                Serial.print("CCW");
            }
        Serial.println();
        Serial.println();

        previousPos = currentPos; //update previousPos

        s = 0;
        finish = 0;
    }
    analogWrite(6, 0); // turning off the motor
}

// ------------------------------- HELPER FUNCTIONS !!! -------------------------------- //

int grayCodetoDec(int *gray) {
    
    //convert gray code to binary
    int binary[5];
    binary[4] = gray[4];
    Serial.print(binary[4]);
    Serial.print("    ");
    for (int i = 3; i >= 0; i-- ) { 
        binary[i] = gray[i] ^ binary[i+1];
        Serial.print(binary[i]);
        Serial.print("    ");
    }
    Serial.println();

    //convert to decimal
    int decimal = 0;
    for (int i = 0; i<= 4; i++ ) {
        decimal = decimal + ceil(binary[i]*pow(2, i)); //powers of 2
        Serial.print(binary[i]*pow(2, i));
        Serial.print("    ");
    }
    Serial.print(decimal);
    Serial.println();
    return decimal;
}

void readEncoders(void) {
    encoders[0] = digitalRead(9);
    encoders[1] = digitalRead(10);
    encoders[2] = digitalRead(11);
    encoders[3] = digitalRead(12);
    encoders[4] = digitalRead(13);

    for (int i = 4; i >= 0; i-- ) { 
        
        Serial.print(encoders[i]);
        Serial.print("    ");
    }
    Serial.println();

    return encoders;
}

void zeroPosition() {
    readEncoders(); 
    zeroPos = grayCodetoDec(encoders); //zero value
    previousPos = zeroPos;

    Serial.print("Zero position");
    Serial.println(zeroPos);
}

float absoluteAngle(int position) {
    int absPos = position - zeroPos; 
    if (absPos < 0) { absPos+= 32; } //if less than zero

    float absAngle = (float)absPos*360/ENCODER_SECTIONS;
    return absAngle;
}

float relativeAngle(int currentPosition, int previousPosition) {
    int relPos = currentPosition - previousPosition; 
    // if (relPos < 0) { relPos+= 32; } //if less than zero

    float relAngle = (float)relPos*360/ENCODER_SECTIONS;
    if (relAngle >= 180) {relAngle = 360 - relAngle; }
    return abs(relAngle);

}

int direction(float initial, float final) {
    int direction;
    if (abs(final - initial) < 120) { //if less than 90 degrees moved
        if (final > initial) { direction = CW; }
        else { direction = CCW; }
    } else { //passed by 0 
        if (final > initial) { direction = CCW; }
        else { direction = CW; }
    }
    return direction; 
}

int analogueToDigital(int transistorVal, int transistorNum){
  switch(transistorNum)
  {
    case 4:
      if(transistorVal>t4h){
        return 0;   //return white
      }else if(transistorVal<t4l){
        return 1;   //return black
      }

    case 3:
      if(transistorVal>t3h){
        return 0;   //return white
      }else if(transistorVal<t3l){
        return 1;   //return black
      }

    case 2:
      if(transistorVal>t2h){
        return 0;   //return white
      }else if(transistorVal<t2l){
        return 1;   //return black
      }

    case 1:
      if(transistorVal>t1h){
        return 0;   //return white
      }else if(transistorVal<t1l){
        return 1;   //return black
      }

    case 0:
      if(transistorVal>t0h){
        return 0;   //return white
      }else if(transistorVal<t0l){
        return 1;   //return black
      }
  }
}


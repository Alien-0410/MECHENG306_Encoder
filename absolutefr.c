
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
int zeroPos;
int currentPos;
int previousPos;
int encoders[5];
int expectedDirection = CW;



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

    pinMode(10, INPUT); //2 pin attempt
    pinMode(11, INPUT);
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


        Serial.print("shaft position from optical absolute sensor from home position: ");
        Serial.println(absoluteAngle(currentPos));

        Serial.print("shaft displacement from optical absolute sensor: ");
        Serial.println(currentPos - previousPos);

        Serial.print("Shaft displacement from motor's builtin encoder: ");
        Serial.println(s * 360 / 228); // every full Revolution of the shaft is associated with 228 counts of builtin
                                       // encoder so to turn it to degre we can use this formula (s * 360 / 228), "s" is the number of  built-in encoder counts

        float Error = currentPos - previousPos - s * 360 / 228;
        Serial.print("Error :");
        Serial.println(Error); // displaying error
        Serial.println();

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
    for (int i = 4; i >= 0; i-- ) { 
        binary[i] = gray[i] ^ binary[i+1];
    }

    //convert to decimal
    int decimal;
    for (int i = 0; i<= 4; i++ ) {
        decimal+= binary[i]*pow(2, i); //powers of 2
    }

    return decimal;
}

void readEncoders(void) {
    encoders[0] = digitalRead(X);
    encoders[1] = digitalRead(X);
    encoders[2] = digitalRead(X);
    encoders[3] = digitalRead(X);
    encoders[4] = digitalRead(X);

    return encoders;
}

void zeroPosition(void) {
    readEncoders(); 
    zeroPos = grayCodetoDec(encoders); //zero value
    previousPos = zeroPos;
}

float absoluteAngle(int position) {
    int absPos = position - zeroPos; 
    if (absPos < 0) { absPos+= 32; } //if less than zero

    float absAngle = (float)absPos*360/ENCODER_SECTIONS;

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
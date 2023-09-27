// ----------------------------------------- BASE CODE ----------------------------------------- //
int b = 0; // reading the time for main loop to be run for 15s
int c = 0; // memory for the time in mainloop

float s = 0; // built-in encoder counts
float s_2;   // built-in encoder counts for RPM calculation for PI controler

float rpmm; // rpm obtained each 5s from built-in encoder

int s1 = 0;         // built-in encoder chanel one outpot
int s2 = 0;         // built-in encoder chanel two outpot
int r = 0;          // repetition indicator for reading counts of bult-in encoder
int s2m = 0;        // memory of built-in encoder chanel two outpot
int directionm = 0; // indicator for direction read by built-in encoder
int dirm;           // indicator for direction read by built-in encode
int RPM;            // Commanded RPM

int exitt = 0; // mainloop exit condition

float ctrl;     // PI controller outpot
float kp = .4;  // proportional gain of PI controller
float ki = .01; // integral gain of PI controller
float eri;      // integral of error of PI controller

int repc = 1;   // repetition condition of PI controller
int t0;         // memory of time for the Purpose of displaying the results
int repeat = 0; // repeat indicator to only let the memory of time for the Purpose of displaying the results be updated once

// ----------------------------------------- USER DEFINED  ----------------------------------------- //
int enc1 = 0;
int enc2 = 0;
int enc2mem = 0;
int encrep = 0;
int countTot = 0;
int countRecent = 0;
float rpmRecent;
int encoderdirect1 = 0;
int encoderdirect2 = 0;

#define ENCODER_SECTIONS 64

void setup()
{
    // put your setup code here, to run on
    Serial.begin(250000); // Baud rate of communication

    Serial.println("Enter the desired RPM.");

    while (Serial.available() == 0)
    {
        // Wait for user input
    }

    RPM = Serial.readString().toFloat(); // Reading the Input string from Serial port.
    if (RPM < 0)
    {
        analogWrite(3, 255); // changing the direction of motor's rotation
    }
    RPM = abs(RPM);

    //configure digital outputs?
    pinMode(10, INPUT);
    pinMode(11, INPUT);

    enc1 = digitalRead(10); // reading Chanel 1 of user encoder
    enc2 = digitalRead(11); // reading Chanel 1 of user encoder


    //configure encrep
    if (enc1 == enc2) {
      encrep = 1;
    }
    else {
      encrep = 0;      
    }
}

void loop()
{

    b = millis(); // reading time
    c = b;        // storing the current time

    while ((b >= c) && (b <= (c + 15500)) && exitt == 0) // let the main loop to be run for 15s
    {

        if (b % 13 == 0 && repc == 1) // PI controller
        {
            eri = ki * (RPM - rpmm) + eri;
            ctrl = 50 + kp * (RPM - rpmm) + eri;
            analogWrite(6, ctrl);
            repc = 0;
        }
        if (b % 13 == 1)
        {
            repc = 1;
        }

        s1 = digitalRead(7); // reading Chanel 1 of builtin encoder
        s2 = digitalRead(8); // reading Chanel 2 of builtin encoder
        
        enc1 = digitalRead(10); // reading Chanel 1 of quad encoder
        enc2 = digitalRead(11); // reading Chanel 2 of quad encoder

        if (s1 != s2 && r == 0)
        {
            s = s + 1;     // counters for rpm that displyed every 5s
            s_2 = s_2 + 1; // counters for rpm that used in PI contoller
            r = 1;         // this indicator wont let this condition, (s1 != s2), to be counted until the next condition, (s1 == s2), happens
        }

        if (s1 == s2 && r == 1)
        {
            s = s + 1;     // counters for rpm that displyed every 5s
            s_2 = s_2 + 1; // counters for rpm that used in PI contoller
            r = 0;         // this indicator wont let this condition, (sm1 == sm2), to be counted until the next condition, (sm1 != sm2), happens
        }


        if (enc1 != enc2 && encrep == 0) //first time encoders are different
        {
            countTot++;
            countRecent++;
            encrep = 1;     //prevent extra readings
        }

        if (enc1 == enc2 && encrep == 1) //first time encoders are the same
        {
            countTot++;
            countRecent++;
            encrep = 0;     //prevent extra readings
        }
        
        b = millis(); // updating time
        if (b % 100 <= 1 && repeat == 0)
        {
            t0 = b; // storing the current time once
            repeat = 1;
        }

        if (b % 100 == 0)
        {
            Serial.print("time in ms: ");
            Serial.println(b - t0);

            Serial.print("  spontaneous speed from builtin encoder:  ");
            rpmm = (s_2 / (2 * 114)) * 600; // formulation for rpm in each 100ms for PI controller
            Serial.println(rpmm);
            s_2 = 0; // reseting the counters of PI controller rpm meter

            Serial.print("  spontaneous speed from user encoder:  ");
            rpmRecent = (float)600*countRecent/ENCODER_SECTIONS; // 60/0.1 seconds
            Serial.println(rpmRecent);

            //debugging code
            // Serial.print(enc1);
            // Serial.print(" | ");
            // Serial.println(enc2);
            // Serial.print(encrep);
            // Serial.print(" | ");
            // Serial.println(countRecent);
            countRecent = 0; // reseting the counters of PI controller rpm meter

            if ((b - t0) % 5000 == 0)
            {
                Serial.println();
                Serial.print("RPM from builtin encoder: ");
                Serial.println((s / (228)) * 12); // formula for rpm in each 5s

                Serial.print("RPM from optical quadrature encoder: ");
                Serial.println(12*(float)countTot/ENCODER_SECTIONS);  // formula for rpm in each 5s
                // DERIVE FORMULA

                Serial.print("Error: ");
                Serial.println((12*(float)countTot/ENCODER_SECTIONS)-(12*(float)s/228 ));

                Serial.print("direction read by motor's sensor: ");
                if (dirm == 0)
                {
                    Serial.print("CW");
                }
                else
                {
                    Serial.print("CCW");
                }
                Serial.print("  ,   ");

                Serial.print("direction read by sensor:  ");
                if (encoderdirect1 == 0)
                {
                    Serial.print("CW");
                }
                else
                {
                    Serial.print("CCW");
                }
                Serial.println();

                s = 0;
                directionm = 0;

                countTot = 0;
                encoderdirect2 = 0;

            }
            delay(1);
        }

        if ((s1 == HIGH) && (s2 == HIGH) && (s2m == LOW)) // reading the direction of motor by cheaking which chanel follows which
        {
            directionm = directionm + 1;
        }

        if ((s1 == LOW) && (s2 == LOW) && (s2m == HIGH))
        {
            directionm = directionm + 1;
        }

        s2m = s2; // memory of the previous builtin encoder chanel 2

        if (directionm > 100)
        {
            dirm = 0;
        }
        if (directionm < 20)
        {
            dirm = 1;
        }

        //THIS COULD BE WAY SIMPLIFIED
        if ((enc1 == 1) && (enc2 == 1) && (enc2mem == 0)) // reading the direction of motor by cheaking which chanel follows which
        {
            encoderdirect2++;
        }

        if ((enc1 == 0) && (enc2 == 0) && (enc2mem == 1))
        {
            encoderdirect2++;
        }

        if (enc1 == enc2 && enc2 != enc2mem) {
            encoderdirect2++;
        }

        enc2mem = enc2; // memory of the previous builtin encoder chanel 2

        if (encoderdirect2 > 100)
        {
            encoderdirect1 = 0;
        }
        if (encoderdirect2 < 20)
        {
            encoderdirect1 = 1;
        }

        b = millis(); // updating time
    }
    analogWrite(6, 0); // turning off the motor
    exitt = 1;         // changing the exit condition to prevent the motor to run after 15s
}

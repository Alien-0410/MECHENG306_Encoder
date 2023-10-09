int enc1,enc2, enc3, enc4, enc5 = 0;

void setup() {
  Serial.begin(250000); // Baud rate of communication

  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);
  pinMode(A3, INPUT);
  pinMode(A4, INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  enc1 = analogRead(A0); // reading Chanel 1 of user encoder
  enc2 = analogRead(A1); // reading Chanel 1 of user encode
  enc3 = analogRead(A2);
  enc4 = analogRead(A3); // reading Chanel 1 of user encode
  enc5 = analogRead(A4);

  Serial.print(enc5);
  Serial.print(" | ");
  Serial.print(enc4);
  Serial.print(" | ");
  Serial.print(enc3);
  Serial.print(" | ");
  Serial.print(enc2);
  Serial.print(" |");
  Serial.println(enc1);

}

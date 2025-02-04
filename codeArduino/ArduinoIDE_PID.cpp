// IR Sensors
int sensor1 = 2;      // Left most sensor
int sensor2 = 3;
int sensor3 = 4;
int sensor4 = 5;      
//int sensor5 = 5;      // Right most sensor
// Initial Values of Sensors
int sensor[4] = {0, 0, 0, 0};

// Motor Variables
int ENA = 6;              //Motor Trái
int motorInput1 = 7;
int motorInput2 = 8;
int motorInput3 = 9;
int motorInput4 = 10;
int ENB = 11;             //Motor Phải

//Initial Speed of Motor
int initial_motor_speed = 100;

// Output Pins for Led
int ledPin1 = A3;
int ledPin2 = A4;

// PID Constants
float Kp = 25;
float Ki = 0;
float Kd = 15;

float error = 0, P = 0, I = 0, D = 0, PID_value = 0;
float previous_error = 0, previous_I = 0;

int flag = 0;

void setup()
{
  pinMode(sensor1, INPUT);
  pinMode(sensor2, INPUT);
  pinMode(sensor3, INPUT);
  pinMode(sensor4, INPUT);
  //pinMode(sensor5, INPUT);

  pinMode(motorInput1, OUTPUT);
  pinMode(motorInput2, OUTPUT);
  pinMode(motorInput3, OUTPUT);
  pinMode(motorInput4, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);

  pinMode(ledPin1, OUTPUT);
  pinMode(ledPin2, OUTPUT);

  digitalWrite(ledPin1, LOW);
  digitalWrite(ledPin2, LOW);

  Serial.begin(9600);                     //setting serial monitor at a default baund rate of 9600
  delay(500);
  Serial.println("Started !!");
  delay(1000);
}
void loop()
{
  read_sensor_values();
  Serial.print(error);
  if (error == 100) {               // Rẽ Trái 90*    Make left turn untill it detects straight path
    //Serial.print("\t");
    //Serial.println("Left");
    //stop_bot();
    //delay(100);
    do {                            // Quay sang trái cho tới khi phát hiện ngay giữa line
      left();
      analogWrite(ENA, 80);        //Left Motor Speed
      analogWrite(ENB, 90);         //Right Motor Speed
      read_sensor_values();
    } while (error == 0);
    //stop_bot();
    //delay(200);
    //read_sensor_values();
  } else if (error == 101) {          // Rẽ Phải 90* Make right turn in case of it detects only right path (it will go into forward direction in case of staright and right "|--")
    // untill it detects straight path.
    //Serial.print("\t");
    //Serial.println("Right");
    //  Quay đầu về bên trái cho tới khi phát hiện line thì dừng lại
      //stop_bot();
      //delay(100);
      do {                          // Quay sang trái cho tới khi phát hiện ngay giữa line
        analogWrite(ENA, 80);        //Left Motor Speed
        analogWrite(ENB, 80);        //Right Motor Speed
        sharpRightTurn();
        read_sensor_values();
      } while (error == 0);
      //stop_bot();
      //delay(200);
      //read_sensor_values();
  } else if (error == 102) {          // Quay đầu về bên trái  Make left turn untill it detects straight path
    //Serial.print("\t");
    //Serial.println("Sharp Left Turn");
      //stop_bot();
      //delay(100); 
      do {                              // Quay sang trái cho tới khi phát hiện ngay giữa line
      read_sensor_values();
      analogWrite(ENA, 80);        //Left Motor Speed
      analogWrite(ENB, 80);             //Right Motor Speed
      sharpLeftTurn();
      } while (error == 0);
      //stop_bot();
      //delay(200);
      //read_sensor_values();
  } else if (error == 103) {          // Make left turn untill it detects straight path or stop if dead end reached.
    stop_bot();
    //delay(100);
    if (flag == 0) {
      analogWrite(ENA, 80);        //Left Motor Speed
      analogWrite(ENB, 80);             //Right Motor Speed
      forward();
      delay(100);
      //stop_bot();
      read_sensor_values();
      if (error == 103) {             /**** Dead End Reached, Stop! ****/
        stop_bot();
        delay(200);
        digitalWrite(ledPin1, HIGH);
        digitalWrite(ledPin2, HIGH);
        flag = 1;
       } else {                        /**** Move Left ****/
        do {                                      // Quay sang trái cho tới khi phát hiện ngay giữa line
          //Serial.print("\t");
          //Serial.println("Left Here");
          analogWrite(ENA, 80);        //Left Motor Speed
          analogWrite(ENB, 80);        //Right Motor Speed
          sharpLeftTurn();
          read_sensor_values();
        } while (error == 0);
        //stop_bot();
        //delay(200);
        //read_sensor_values();
        }
    }
  }else {
    calculate_pid();                  // Tính giá trị PID
    motor_control();                  // Điều chỉnh motor theo giá trị PID mới tính, cho xe chạy thẳng
      }
}

void read_sensor_values()
{
  sensor[0] = digitalRead(sensor1);
  sensor[1] = digitalRead(sensor2);
  sensor[2] = digitalRead(sensor3);
  sensor[3] = digitalRead(sensor4);
  //sensor[4] = digitalRead(sensor5);
  /*
    Serial.print(sensor[0]);
    Serial.print("\t");
    Serial.print(sensor[1]);
    Serial.print("\t");
    Serial.print(sensor[2]);
    Serial.print("\t");
    Serial.println(sensor[3]);*/

  if ((sensor[0] == 1) && (sensor[1] == 0) && (sensor[2] == 0) && (sensor[3] == 0))       // lệch phải nhiều
    error = -3;
  else if ((sensor[0] == 1) && (sensor[1] == 1) && (sensor[2] == 0) && (sensor[3] == 0))
    error = -2;
  else if ((sensor[0] == 0) && (sensor[1] == 1) && (sensor[2] == 0) && (sensor[3] == 0))
    error = -1;
  else if ((sensor[0] == 0) && (sensor[1] == 1) && (sensor[2] == 1) && (sensor[3] == 0))  // ko lệch
    error = 0;
  else if ((sensor[0] == 0) && (sensor[1] == 0) && (sensor[2] == 1) && (sensor[3] == 0))
    error = 1;
  else if ((sensor[0] == 0) && (sensor[1] == 0) && (sensor[2] == 1) && (sensor[3] == 1))
    error = 2;
  else if ((sensor[0] == 0) && (sensor[1] == 0) && (sensor[2] == 0) && (sensor[3] == 1))  // lệch trái nhiều
    error = 3;
  else if ((sensor[0] == 1) && (sensor[1] == 1) && (sensor[2] == 1) && (sensor[3] == 0)) // Turn robot left side
    error = 100;
  else if ((sensor[0] == 0) && (sensor[1] == 1) && (sensor[2] == 1) && (sensor[3] == 1)) // Turn robot right side
    error = 101;
  else if ((sensor[0] == 0) && (sensor[1] == 0) && (sensor[2] == 0) && (sensor[3] == 0)) // Make U turn
    error = 102;
  else if ((sensor[0] == 1) && (sensor[1] == 1) && (sensor[2] == 1) && (sensor[3] == 1)) // Turn left side or stop
    error = 103;
}

void calculate_pid()
{
  P = error;
  I = I + previous_I;
  D = error - previous_error;

  PID_value = (Kp * P) + (Ki * I) + (Kd * D); // er =3; PID = 120 motor phải chay nhanh hơn

  previous_I = I;
  previous_error = error;
}

void motor_control()
{
  // Calculating the effective motor speed:
  
  int left_motor_speed = initial_motor_speed - PID_value;   //-30
  int right_motor_speed = initial_motor_speed + PID_value;  //210

  // The motor speed should not exceed the max PWM value
  left_motor_speed = constrain(left_motor_speed, 0, 255);   //0
  right_motor_speed = constrain(right_motor_speed, 0, 255); //210

  /*Serial.print(PID_value);
    Serial.print("\t");
    Serial.print(left_motor_speed);
    Serial.print("\t");
    Serial.println(right_motor_speed);*/

  analogWrite(ENA, left_motor_speed-30); //Left Motor Speed    //0
  analogWrite(ENB, right_motor_speed); //Right Motor Speed  //120

  //following lines of code are to make the bot move forward
  forward();
}

void forward()
{
  /*The pin numbers and high, low values might be different depending on your connections */
  digitalWrite(motorInput1, HIGH);
  digitalWrite(motorInput2, LOW);
  digitalWrite(motorInput3, HIGH);
  digitalWrite(motorInput4, LOW);
}
void reverse()
{
  /*The pin numbers and high, low values might be different depending on your connections */
  digitalWrite(motorInput1, LOW);
  digitalWrite(motorInput2, HIGH);
  digitalWrite(motorInput3, LOW);
  digitalWrite(motorInput4, HIGH);
}
void right()
{
  /*The pin numbers and high, low values might be different depending on your connections */
  digitalWrite(motorInput1, HIGH);
  digitalWrite(motorInput2, LOW);
  digitalWrite(motorInput3, LOW);
  digitalWrite(motorInput4, LOW);
}
void left()
{
  /*The pin numbers and high, low values might be different depending on your connections */
  digitalWrite(motorInput1, LOW);
  digitalWrite(motorInput2, LOW);
  digitalWrite(motorInput3, HIGH);
  digitalWrite(motorInput4, LOW);
}
void sharpLeftTurn() {
  /*The pin numbers and high, low values might be different depending on your connections */
  digitalWrite(motorInput1, LOW);
  digitalWrite(motorInput2, HIGH);
  digitalWrite(motorInput3, HIGH);
  digitalWrite(motorInput4, LOW);
}
void sharpRightTurn() {
  /*The pin numbers and high, low values might be different depending on your connections */
  digitalWrite(motorInput1, HIGH);
  digitalWrite(motorInput2, LOW);
  digitalWrite(motorInput3, LOW);
  digitalWrite(motorInput4, HIGH);
}
void stop_bot()
{
  /*The pin numbers and high, low values might be different depending on your connections */
  digitalWrite(motorInput1, LOW);
  digitalWrite(motorInput2, LOW);
  digitalWrite(motorInput3, LOW);
  digitalWrite(motorInput4, LOW);
}
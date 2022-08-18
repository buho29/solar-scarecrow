
// i2s pins
i2s_pin_config_t i2sPins = {
    .bck_io_num = 25,//this is BCK pin
    .ws_io_num = 33,// this is LRCK pin
    .data_out_num = 26,// this is DATA output pin
    .data_in_num = -1
};



const int pinPWMA = 13;
const int pinAIN2 = 12;
const int pinAIN1 = 14;
const int pinSTBY = 27;


// motor

enum direction {
   clockwise,
   counterClockwise
};


int speed = 255;//velocidad de giro
direction dir;

const int pinMotorA[3] = { pinPWMA, pinAIN2, pinAIN1 };

void enableMotors()
{
   digitalWrite(pinSTBY, HIGH);
}

void disableMotors()
{
   digitalWrite(pinSTBY, LOW);
}

void setupMotor()
{
   pinMode(pinPWMA, OUTPUT);
   pinMode(pinAIN2, OUTPUT);
   pinMode(pinAIN1, OUTPUT);
   pinMode(pinSTBY, OUTPUT);
   
   enableMotors();
}

//Funciones que controlan los motores
void moveMotorForward(const int pinMotor[3], int speed)
{
   digitalWrite(pinMotor[1], HIGH);
   digitalWrite(pinMotor[2], LOW);

   analogWrite(pinMotor[0], speed);
}

void moveMotorBackward(const int pinMotor[3], int speed)
{
   digitalWrite(pinMotor[1], LOW);
   digitalWrite(pinMotor[2], HIGH);

   analogWrite(pinMotor[0], speed);
}


void turn(int d)
{
   if (d == clockwise) moveMotorBackward(pinMotorA, speed);
   else moveMotorForward(pinMotorA, speed);
}
.

void setup() {
  Serial.begin( 115200 ); 
  setupMotor();
  //
  moveMotorForward(pinMotorA, 255);
  //delay(5000);
}

#include <NewPing.h>

#define ECHO1 48
#define TRIG1 49
#define ECHO2 50
#define TRIG2 51
#define ECHO3 22
#define TRIG3 23
const int pwm1m = 2;
const int pwm2m = 7;
const int pinOfMotorLeft1 = 4;
const int pinOfMotorLeft2 = 3;
const int pinOfMotorRight1 = 6;
const int pinOfMotorRight2 = 5;

float dist_1[3] = {0.0, 0.0, 0.0};  
float dist_2[3] = {0.0, 0.0, 0.0}; 
float dist_3[3] = {0.0, 0.0, 0.0}; 
float middle, dist[3], dist_filtered[3];
float k1, k2, k3;
byte l, delta1, delta2, delta3;

NewPing sonar1(TRIG1, ECHO1, 400);
NewPing sonar2(TRIG2, ECHO2, 400);
NewPing sonar3(TRIG3, ECHO3, 400);


const int hidden_neurons = 10;
const int output_neurons = 4, inputs_neurons = 3;

double Wnn[][10] = {{-29.6227, 4.65064, 4.74703, -24.2615, 34.0842, 1.42989, 0.277893, 7.95809, 15.9884, -6.19429, },
{22.3368, -3.36784, 2.4061, -33.0984, 41.1311, -7.76774, 4.80323, -1.299, -21.4115, -1.2911, },
{17.8846, -11.6776, -11.378, 0.118423, -0.872409, -0.00427335, -3.52163, 0.970718, 7.89753, -6.66342, } };

double Wss[][10] = { {0.846268, 10.7538, 8.28577, -8.93836, 5.77751, -9.73192, 4.35633, 3.57345, -1.57288, -2.65127, },
{11.5261, 26.6425, -8.73377, -13.8948, 9.01105, -7.41404, -19.4357, -13.8748, -0.356079, 7.54458, },
{-23.9052, 7.35887, -6.59392, 1.28684, 3.08477, 8.19254, -9.53369, 6.36401, 7.37543, 4.09325, },
{-0.846263, -10.8338, -8.28572, 5.71098, -5.93684, 9.71051, -4.35398, -3.10713, 1.28729, 0.65426, }
};


double Xn[3] = {0, 0, 0};
double Yn[4] = {0, 0, 0, 0};

void setup()
{
  pinMode(pwm1m, OUTPUT);
  pinMode(pwm2m, OUTPUT);
  pinMode(pinOfMotorLeft1, OUTPUT);
  pinMode(pinOfMotorLeft2, OUTPUT);
  pinMode(pinOfMotorRight1, OUTPUT);
  pinMode(pinOfMotorRight2, OUTPUT);
  Serial.begin(115200);
}

void loop()
{
  measureDistance();
  neuralNetwork(dist_filtered[0], dist_filtered[1],dist_filtered[2]);
  directionMove();
}

void measureDistance(){
  if (l > 1) l = 0;
  else l++;

  dist_1[l] = (float)sonar1.ping() / 57.5; 
  dist_2[l] = (float)sonar2.ping() / 57.5;
  dist_3[l] = (float)sonar3.ping() / 57.5;                // получить расстояние в текущую ячейку массива
  dist[0] = middle_of_3(dist_1[0], dist_1[1], dist_1[2]);
  dist[1]= middle_of_3(dist_2[0], dist_2[1], dist_2[2]); 
  dist[2] = middle_of_3(dist_3[0], dist_3[1], dist_3[2]);   // фильтровать медианным фильтром из 3ёх последних измерений

  delta1 = abs(dist_filtered[0] - dist[0]);
  delta2 = abs(dist_filtered[1] - dist[1]); 
  delta3 = abs(dist_filtered[2] - dist[2]);                   // расчёт изменения с предыдущим

  if (delta1 > 1) k1 = 0.7;                               
  else k1 = 0.1;                                          
  if (delta2 > 1) k2 = 0.7;                               
  else k2 = 0.1;                                          
  if (delta3 > 1) k3 = 0.7;                               // если большое - резкий коэффициент
  else k3 = 0.1;                                          // если маленькое - плавный коэффициент

  dist_filtered[0] = dist[0] * k1 + dist_filtered[0] * (1 - k1);
  dist_filtered[1] = dist[1] * k2 + dist_filtered[1] * (1 - k2);
  dist_filtered[2] = dist[2] * k3 + dist_filtered[2] * (1 - k3);// фильтр "Скользящая средняя"

  
}

// медианный фильтр из 3ёх значений
float middle_of_3(float a, float b, float c) {
  if ((a <= b) && (a <= c)) {
    middle = (b <= c) ? b : c;
  }
  else {
    if ((b <= a) && (b <= c)) {
      middle = (a <= c) ? a : c;
    }
    else {
      middle = (a <= b) ? a : b;
    }
  }
  return middle;
}

void neuralNetwork(float in1, float in2, float in3)
{
  double N[hidden_neurons], Y[output_neurons];
  Xn[0] = in1;
  Xn[1] = in2;
  Xn[2] = in3;
  for(int i = 0;i<3;i++){
    if(Xn[i]>100)Xn[i] = 100;
    Xn[i] = Xn[i] - 10;
  }
  Xn[1] = Xn[1] - 10;

  for (int m = 0; m < hidden_neurons; m++){
    N[m] = 0.0;
    for (int n = 0; n < inputs_neurons; n++){
      N[m] += Wnn[n][m] * Xn[n];
    }
    N[m] = 1 / (1 + exp(-N[m]));
  }

  for (int n = 0; n < output_neurons; n++){
    Y[n] = 0.0;
    for (int m = 0; m < hidden_neurons; m++){
      Y[n] += N[m] * Wss[n][m];
    }
    Y[n] = 1 / (1 + exp(-Y[n]));
  }
  for (int i = 0; i < 4; i++){
    Yn[i] = Y[i];
  }
}
int sf = 30;
int st = 200;
void directionMove()
{
  //Serial.println(String(dist_filtered[0])+","+ String(dist_filtered[1])+","+  String(dist_filtered[2])+ ":"+ String(Yn[0])+","+ String(Yn[1])+","+ String(Yn[2])+","+ String(Yn[3]));
  if (Yn[0] > 0.50){
    leftMotorUp(100);
    rightMotorUp(150);
    if (Yn[1] > 0.50){
      rightMotorUp(sf);
      leftMotorUp(150);
      //rightMotorStop();
    }
    else if (Yn[2] > 0.50){
      leftMotorUp(sf);
      rightMotorUp(st);
      //leftMotorStop();
    }
  }
  else{
    leftMotorDown(100);
    rightMotorDown(150);
    if (Yn[1] > 0.50){
      rightMotorDown(sf);
      leftMotorDown(150);
      //rightMotorStop();
    }
    else if (Yn[2] > 0.50){
      leftMotorDown(sf);
      rightMotorDown(st);
      //leftMotorStop();
    }
  }
}

void leftMotorUp(int speedMotor)
{
  digitalWrite(pinOfMotorLeft1, HIGH);
  digitalWrite(pinOfMotorLeft2, LOW);
  analogWrite(pwm1m, speedMotor);
}

void leftMotorDown(int speedMotor)
{
  digitalWrite(pinOfMotorLeft1, LOW);
  digitalWrite(pinOfMotorLeft2, HIGH);
  analogWrite(pwm1m, speedMotor);
}

void leftMotorStop()
{
  digitalWrite(pinOfMotorLeft1, LOW);
  digitalWrite(pinOfMotorLeft2, LOW);
  analogWrite(pwm1m, 0);
}

void rightMotorUp(int speedMotor)
{
  digitalWrite(pinOfMotorRight1, HIGH);
  digitalWrite(pinOfMotorRight2, LOW);
  analogWrite(pwm2m, speedMotor);
}

void rightMotorDown(int speedMotor)
{
  digitalWrite(pinOfMotorRight1, LOW);
  digitalWrite(pinOfMotorRight2, HIGH);
  analogWrite(pwm2m, speedMotor);
}
void rightMotorStop()
{
  digitalWrite(pinOfMotorRight1, LOW);
  digitalWrite(pinOfMotorRight2, LOW);
  analogWrite(pwm2m, 0);
}

#include <Servo.h>

Servo myservo;

int buff_x[400];      //描画のx座標群
int buff_y[400];      //描画のy座標群
int buff_p[2];        //現在のロボットの位置
int buff_t[2];        //前回のロボットの位置
int counter = 0;      //シリアル通信した回数
int count = 0;        //ロボットが動作を行った回数
int motion = 0;       //絵を描く動作を行った回数
int splitResult = 0;
const size_t si = 400;
String sep[si];

double distance[100]; //描画の各点間の距離
double angle[100];    //描画の各点間の回転角
double right_angle;   //基準座標系にロボット座標系を合わせる
double now_angle = 0; //ロボットの基準座標系での回転角

int en[3] = {7, 6, 5};
int in_1[3] = {22, 28, 34};
int in_2[3] = {24, 30, 36};
int Enc_A[3] = {3, 18, 20};
int Enc_B[3] = {2, 19, 21};
volatile int enc0_count = 0;
volatile int enc1_count = 0;
volatile int enc2_count = 0;

/*--------------------------------------------------------*/
//ロボットの関数設定
int speed_0 = 80;
int speed_1 = 85;
int speed_2 = 80;

void goForward() {
    digitalWrite(in_1[0], LOW);
    digitalWrite(in_2[0], HIGH);
    digitalWrite(in_1[1], HIGH);
    digitalWrite(in_2[1], LOW);
    digitalWrite(in_1[2], LOW);
    digitalWrite(in_2[2], LOW);
    analogWrite(en[0], speed_0);
    analogWrite(en[1], speed_1);
    analogWrite(en[2], speed_2);
}
void goBack() {
    digitalWrite(in_1[0], HIGH);
    digitalWrite(in_2[0], LOW);
    digitalWrite(in_1[1], LOW);
    digitalWrite(in_2[1], HIGH);
    digitalWrite(in_1[2], LOW);
    digitalWrite(in_2[2], LOW);
    analogWrite(en[0], speed_0);
    analogWrite(en[1], speed_1);
    analogWrite(en[2], speed_2);
}
void goRight() {
    digitalWrite(in_1[0], LOW);
    digitalWrite(in_2[0], HIGH);
    analogWrite(en[0], 80);
    digitalWrite(in_1[1], LOW);
    digitalWrite(in_2[1], HIGH);
    analogWrite(en[1], 80);
    digitalWrite(in_1[2], HIGH);
    digitalWrite(in_2[2], LOW);
    analogWrite(en[2], 360);
}
void goLeft() {
    digitalWrite(in_1[0], HIGH);
    digitalWrite(in_2[0], LOW);
    analogWrite(en[0], 80);
    digitalWrite(in_1[1], HIGH);
    digitalWrite(in_2[1], LOW);
    analogWrite(en[1], 80);
    digitalWrite(in_1[2], LOW);
    digitalWrite(in_2[2], HIGH);
    analogWrite(en[2], 360);
}
void moveStop() {
  for(int i = 0; i < 3; i++){
    digitalWrite(in_1[i], LOW);
    digitalWrite(in_2[i], LOW);
  }
//  delay(time);
}
void turnLeft() {
  digitalWrite(in_1[0], HIGH);
  digitalWrite(in_2[0], LOW);
  digitalWrite(in_1[1], HIGH);
  digitalWrite(in_2[1], LOW);
  digitalWrite(in_1[2], HIGH);
  digitalWrite(in_2[2], LOW);
  analogWrite(en[0], speed_0);
  analogWrite(en[1], speed_1);
  analogWrite(en[2], speed_2);
}
void turnRight() {
  digitalWrite(in_1[0], LOW);
  digitalWrite(in_2[0], HIGH);
  digitalWrite(in_1[1], LOW);
  digitalWrite(in_2[1], HIGH);
  digitalWrite(in_1[2], LOW);
  digitalWrite(in_2[2], HIGH);
  analogWrite(en[0], speed_0);
  analogWrite(en[1], speed_1);
  analogWrite(en[2], speed_2);
}
void countReset(){
  enc0_count = 0;
  enc1_count = 0;
  enc2_count = 0;
}
/*--------------------------------------------------------*/

/*--------------------------------------------------------*/
//エンコーダーのカウンター
void changeEnc_A1(){
  if(digitalRead(Enc_A[0])){
    if(digitalRead(Enc_B[0]))  --enc0_count;
    else  ++enc0_count;
  }
  else{
    if(digitalRead(Enc_B[0]))  ++enc0_count;
    else  --enc0_count;
  }
}
void changeEnc_B1(){
  if(digitalRead(Enc_B[0])){
    if(digitalRead(Enc_A[0]))  ++enc0_count;
    else  --enc0_count;
  }
  else{
    if(digitalRead(Enc_A[0]))  --enc0_count;
    else  ++enc0_count;
  }
}
void changeEnc_A2(){
  if(digitalRead(Enc_A[1])){
    if(digitalRead(Enc_B[1]))  --enc1_count;
    else  ++enc1_count;
  }
  else{
    if(digitalRead(Enc_B[1]))  ++enc1_count;
    else  --enc1_count;
  }
}
void changeEnc_B2(){
  if(digitalRead(Enc_B[1])){
    if(digitalRead(Enc_A[1]))  ++enc1_count;
    else  --enc1_count;
  }
  else{
    if(digitalRead(Enc_A[1]))  --enc1_count;
    else  ++enc1_count;
  }
}
void changeEnc_A3(){
  if(digitalRead(Enc_A[2])){
    if(digitalRead(Enc_B[2]))  --enc2_count;
    else  ++enc2_count;
  }
  else{
    if(digitalRead(Enc_B[2]))  ++enc2_count;
    else  --enc2_count;
  }
}
void changeEnc_B3(){
  if(digitalRead(Enc_B[2])){
    if(digitalRead(Enc_A[2]))  ++enc2_count;
    else  --enc2_count;
  }
  else{
    if(digitalRead(Enc_A[2]))  --enc2_count;
    else  ++enc2_count;
  }
}
/*--------------------------------------------------------*/

void setup() {
  Serial.begin(9600);
  pinMode(50, OUTPUT);
  myservo.attach(40);
  myservo.write(150);     //ペンを初期位置に設定
  for(int i = 0; i < 3; i ++){
    pinMode(in_1[i], OUTPUT);
    pinMode(in_2[i], OUTPUT);
    pinMode(Enc_A[i], INPUT);
    pinMode(Enc_B[i], INPUT);
  }
  attachInterrupt(0, changeEnc_A1, CHANGE);
  attachInterrupt(1, changeEnc_B1, CHANGE);
  attachInterrupt(4, changeEnc_A2, CHANGE);
  attachInterrupt(5, changeEnc_B2, CHANGE);
  attachInterrupt(2, changeEnc_A3, CHANGE);
  attachInterrupt(3, changeEnc_B3, CHANGE);
}

String str;
void loop(){
  int inputchar;

  inputchar = Serial.read();

  if(inputchar != -1){
    switch(inputchar){
      case 'k':
        myservo.write(240);
        break;
      case 'l':
        myservo.write(150);
        break;
      case 'e':
        delay(1000);
        goForward();
        break;
      case 'd':
        delay(1000);
        goBack();
        break;
      case 'f':
        delay(1000);
        turnRight();
        break;
      case 's':
        delay(1000);
        turnLeft();
        break;
      case 'c':
        moveStop();
    }
  }
}

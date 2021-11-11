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
int speed_1 = 80;
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
void moveStop(int time) {
  for(int i = 0; i < 3; i++){
    digitalWrite(in_1[i], LOW);
    digitalWrite(in_2[i], LOW);
  }
  delay(time);
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

int split(String origin, char delim, size_t resultsize, String *ret) {
  int index = 0;
  int datalength = origin.length();
  for (int i = 0; i < datalength; ++i) {
    char tmp = origin.charAt(i);
    if (tmp == delim) {
      ++index;
      if (index > (resultsize - 1)) return -1;
    } else {
      ret[index] += tmp;
    }
  }
  return (index + 1);
}

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
//シリアル通信で受信したデータを格納
  if (Serial.available()) {
    digitalWrite(50, HIGH);
    str = Serial.readStringUntil('\0');
    splitResult = split(str, ',', si, sep);
    if(splitResult > 0 && sep[0] == "t"){
//      digitalWrite(50, HIGH);
      while(1){
        goForward();
        if(enc1_count == 228){
          moveStop(2000);
          countReset();
          //
          splitResult = 0;
          //
          break;
        }
      }
      for(int i = 1; i < 3; i++){
        buff_t[i-1] = sep[i].toInt(); ////
        sep[i] = "";
      }
      sep[0] = ""; ////
      digitalWrite(50, LOW);////
    }
    while(1){
      delay(1000);
      if (Serial.available()) {
        digitalWrite(50, HIGH);
        str = Serial.readStringUntil('\0');
        splitResult = split(str, ',', si, sep);
        if(splitResult > 0  && sep[0] != "t"){
          for(int i = 0; i < 2; i++) {
          buff_p[i] = sep[i].toInt();
          sep[i] = "";
        }
        int xcount = 0;
        int ycount = 0;
        for(int i = 2; i < splitResult; i++){
          if(i % 2 == 0){
            buff_x[xcount] = sep[i].toInt();
            xcount++;
            sep[i] = "";
          }
          else {
            buff_y[ycount] = sep[i].toInt();
            ycount++;
            sep[i] = "";
          }
        }
        digitalWrite(50, LOW);
        delay(1000);
        }
        break;
      }
    }
    
//各点の距離と角度を算出
    digitalWrite(50, HIGH);
    for(int number = 0; number < splitResult; number++){
      if(number == 0){
        distance[number] = sqrt((buff_x[number] - buff_p[0])*(buff_x[number] - buff_p[0]) + (buff_y[number] - buff_p[1])*(buff_y[number] - buff_p[1]));
        angle[number] = 90 - atan2(buff_y[number] - buff_p[1], buff_x[number] - buff_p[0]) * 180 / 3.14;
      }
      else{
        distance[number] = sqrt((buff_x[number] - buff_x[number-1])*(buff_x[number] - buff_x[number-1]) + (buff_y[number] - buff_y[number-1])*(buff_y[number] - buff_y[number-1]));
        angle[number] = 90 - atan2(buff_y[number] - buff_y[number-1], buff_x[number] - buff_x[number-1]) * 180 / 3.14;
      }
    }
    right_angle = 90 - atan2(buff_p[1] - buff_t[1], buff_p[0] - buff_t[0]) * 180 / 3.14;
    for(int i = 0; i < splitResult; i++){
      if(angle[i] < 0)  angle[i] = 360 + angle[i];
    }
    if(right_angle < 0)  right_angle = 360 + right_angle;
    digitalWrite(50, LOW);
    delay(1000);

//基準座標系とロボット座標系を一致させる
    digitalWrite(50, HIGH);
    countReset();
    if(right_angle <= 180){
      while(1){
        turnRight();
        if(abs(enc2_count) >= right_angle/0.25){
        moveStop(500);
        countReset();
        break;
        }
      }
    }
    else if(right_angle > 180){
      while(1){
        turnLeft();
        if(abs(enc2_count) >= (360-right_angle)/0.25){
        moveStop(500);
        countReset();
        break;
        }
      }
    }
    now_angle = 0;
    digitalWrite(50, LOW);
    delay(1000);

//機体を移動させる
    for(int i = 0; i < splitResult; i++){
      digitalWrite(50, HIGH);
  //次の座標が(0, 0)ならペンを上げて座標系を基準座標系に修正する
      if(buff_x[i] == 0 && buff_y[i] == 0){
        myservo.write(150);
        
        distance[i+1] = sqrt((buff_x[i+1] - buff_x[i-2])*(buff_x[i+1] - buff_x[i-2]) + (buff_y[i+1] - buff_y[i-2])*(buff_y[i+1] - buff_y[i-2]));
        angle[i+1] = 90 - atan2(buff_y[i+1] - buff_y[i-2], buff_x[i+1] - buff_x[i-2]) * 180 / 3.14;
        if(angle[i+1] < 0)  angle[i+1] = 360 + angle[i+1];

        while(1){
          turnLeft();
          if(abs(enc2_count) >= now_angle/0.25){
          moveStop(500);
          countReset();
          break;
          }
          now_angle = 0;
        }
      }
  //今の座標が(0, 0)だったら,次の描画開始位置に移動してペンを下ろす
      if(buff_x[i-1] == 0 && buff_y[i-1] == 0){
        if(angle[i] >= 0 && angle[i] < 180){
          while(1){
            turnRight();
            if(abs(enc2_count) >= angle[i]/0.25){
            moveStop(500);
            countReset();
            break;
            }
          }
          while(1){
            goForward();
            if(enc1_count >= distance[i]/0.2){
            moveStop(500);
            countReset();
            break;
            }
          }
          now_angle = angle[i];
        }
        if(angle[i] >= 180 && angle[i] < 360){
          while(1){
            turnLeft();
            if(abs(enc2_count) >= (360-angle[i])/0.25){
            moveStop(500);
            countReset();
            break;
            }
          }
          while(1){
            goForward();
            if(enc1_count >= distance[i]/0.2){
            moveStop(500);
            countReset();
            break;
            }
          }
          now_angle = angle[i];
        }
        myservo.write(240);
      }
  //位置座標に(0, 0)以外が格納されている場合,描画を行う
      else if(buff_x[i] != 0 && buff_y[i] != 0){
        countReset();
    //初期位置に移動してペンを下ろす
        if(i == 0){
          if(angle[i] >= 0 && angle[i] < 180){
            while(1){
              turnRight();
              if(abs(enc2_count) >= angle[i]/0.25){
              moveStop(500);
              countReset();
              break;
              }
            }
            while(1){
              goForward();
              if(enc1_count >= distance[i]/0.2){
              moveStop(500);
              countReset();
              break;
              }
            }
          }
          if(angle[i] >= 180 && angle[i] < 360){
            while(1){
              turnRight();
              if(abs(enc2_count) >= angle[i]/0.25){
              moveStop(500);
              countReset();
              break;
              }
            }
            while(1){
              goForward();
              if(enc1_count >= distance[i]/0.2){
              moveStop(500);
              countReset();
              break;
              }
            }
          }
          now_angle = angle[i];
          myservo.write(240);
        }
    //描画を行う
        else{
          if(angle[i] == 0){
            if(now_angle >= 0 && now_angle < 90){
              while(1){
                turnLeft();
                if(abs(enc2_count) >= now_angle/0.25){
                moveStop(500);
                countReset();
                break;
                }
              }
              while(1){
                goForward();
                if(enc1_count >= distance[i]/0.2){
                moveStop(500);
                countReset();
                break;
                }
              }
              now_angle = 0;
            }
            else if(now_angle >= 90 && now_angle < 180){
              while(1){
                turnRight();
                if(abs(enc2_count) >= (180-now_angle)/0.25){
                moveStop(500);
                countReset();
                break;
                }
              }
              while(1){
                goBack();
                if(abs(enc1_count) >= distance[i]/0.2){
                moveStop(500);
                countReset();
                break;
                }
              }
              now_angle = 90;
            }
            else if(now_angle >= 180 && now_angle < 270){
              while(1){
                turnLeft();
                if(abs(enc2_count) >= (now_angle-180)/0.25){
                moveStop(500);
                countReset();
                break;
                }
              }
              while(1){
                goBack();
                if(abs(enc1_count) >= distance[i]/0.2){
                moveStop(500);
                countReset();
                break;
                }
              }
              now_angle = 90;
            }
            else if(now_angle >= 270 && now_angle <360){
              while(1){
                turnRight();
                if(abs(enc2_count) >= (360-now_angle)/0.25){
                moveStop(500);
                countReset();
                break;
                }
              }
              while(1){
                goForward();
                if(enc1_count >= distance[i]/0.2){
                moveStop(500);
                countReset();
                break;
                }
              }
              now_angle = 0;
            }
          }
          if(angle[i] == 90){
            if(now_angle >= 0 && now_angle < 90){
              while(1){
                turnRight();
                if(abs(enc2_count) >= (90-now_angle)/0.25){
                moveStop(500);
                countReset();
                break;
                }
              }
              while(1){
                goForward();
                if(enc1_count >= distance[i]/0.2){
                moveStop(500);
                countReset();
                break;
                }
              }
              now_angle = 90;
            }
            else if(now_angle >= 90 && now_angle < 180){
              while(1){
                turnLeft();
                if(abs(enc2_count) >= (now_angle-90)/0.25){
                moveStop(500);
                countReset();
                break;
                }
              }
              while(1){
                goForward();
                if(enc1_count >= distance[i]/0.2){
                moveStop(500);
                countReset();
                break;
                }
              }
              now_angle = 90;
            }
            else if(now_angle >= 180 && now_angle < 270){
              while(1){
                turnRight();
                if(abs(enc2_count) >= (270-now_angle)/0.25){
                moveStop(500);
                countReset();
                break;
                }
              }
              while(1){
                goBack();
                if(abs(enc1_count) >= distance[i]/0.2){
                moveStop(500);
                countReset();
                break;
                }
              }
              now_angle = 270;
            }
            else if(now_angle >= 270 && now_angle < 360){
              while(1){
                turnLeft();
                if(abs(enc2_count) >= (now_angle-270)/0.25){
                moveStop(500);
                countReset();
                break;
                }
              }
              while(1){
                goBack();
                if(abs(enc1_count) >= distance[i]/0.2){
                moveStop(500);
                countReset();
                break;
                }
              }
              now_angle = 270;
            }
          }
          if(angle[i] == 180){
            if(now_angle >= 0 && now_angle < 90){
              while(1){
                turnLeft();
                if(abs(enc2_count) >= (now_angle)/0.25){
                moveStop(500);
                countReset();
                break;
                }
              }
              while(1){
                goBack();
                if(abs(enc1_count) >= distance[i]/0.2){
                moveStop(500);
                countReset();
                break;
                }
              }
              now_angle = 0;
            }
            else if(now_angle >= 90 && now_angle < 180){
              while(1){
                turnRight();
                if(abs(enc2_count) >= (180-now_angle)/0.25){
                moveStop(500);
                countReset();
                break;
                }
              }
              while(1){
                goForward();
                if(enc1_count >= distance[i]/0.2){
                moveStop(500);
                countReset();
                break;
                }
              }
              now_angle = 90;
            }
            else if(now_angle >= 180 && now_angle < 270){
               while(1){
                turnLeft();
                if(abs(enc2_count) >= (now_angle-180)/0.25){
                moveStop(500);
                countReset();
                break;
                }
              }
              while(1){
                goForward();
                if(enc1_count >= distance[i]/0.2){
                moveStop(500);
                countReset();
                break;
                }
              }
              now_angle = 90;
            }
            else if(now_angle >= 270 && now_angle < 360){
              while(1){
                turnRight();
                if(abs(enc2_count) >= (360-now_angle)/0.25){
                moveStop(500);
                countReset();
                break;
                }
              }
              while(1){
                goBack();
                if(abs(enc1_count) >= distance[i]/0.2){
                moveStop(500);
                countReset();
                break;
                }
              }
              now_angle = 0;
            }
          }
          if(angle[i] == 270){
            if(now_angle >= 0 && now_angle < 90){
              while(1){
                turnRight();
                if(abs(enc2_count) >= (90-now_angle)/0.25){
                moveStop(500);
                countReset();
                break;
                }
              }
              while(1){
                goBack();
                if(abs(enc1_count) >= distance[i]/0.2){
                moveStop(500);
                countReset();
                break;
                }
              }
              now_angle = 90;
            }
            else if(now_angle >= 90 && now_angle < 180){
              while(1){
                turnLeft();
                if(abs(enc2_count) >= (now_angle-90)/0.25){
                moveStop(500);
                countReset();
                break;
                }
              }
              while(1){
                goBack();
                if(abs(enc1_count) >= distance[i]/0.2){
                moveStop(500);
                countReset();
                break;
                }
              }
              now_angle = 90;
            }
            else if(now_angle >= 180 && now_angle < 270){
              while(1){
                turnRight();
                if(abs(enc2_count) >= (270-now_angle)/0.25){
                moveStop(500);
                countReset();
                break;
                }
              }
              while(1){
                goForward();
                if(enc1_count >= distance[i]/0.2){
                moveStop(500);
                countReset();
                break;
                }
              }
              now_angle = 270;
            }
            else if(now_angle >= 270 && now_angle < 360){
              while(1){
                turnLeft();
                if(abs(enc2_count) >= (now_angle-270)/0.25){
                moveStop(500);
                countReset();
                break;
                }
              }
              while(1){
                goForward();
                if(enc1_count >= distance[i]/0.2){
                moveStop(500);
                countReset();
                break;
                }
              }
              now_angle = 270;
            }
          }
          else if(angle[i] > 0 && angle[i] < 90){
            if(now_angle >= 0 && now_angle < angle[i]){
              while(1){
                turnRight();
                if(abs(enc2_count) >= (angle[i]-now_angle)/0.25){
                moveStop(500);
                countReset();
                break;
                }
              }
              while(1){
                goForward();
                if(enc1_count >= distance[i]/0.2){
                moveStop(500);
                countReset();
                break;
                }
              }
              now_angle = angle[i];
            }
            else if(now_angle >= angle[i] && now_angle < angle[i]+90){
              while(1){
                turnLeft();
                if(abs(enc2_count) >= (now_angle-angle[i])/0.25){
                moveStop(500);
                countReset();
                break;
                }
              }
              while(1){
                goForward();
                if(enc1_count >= distance[i]/0.2){
                moveStop(500);
                countReset();
                break;
                }
              }
              now_angle = angle[i];
            }
            else if(now_angle >= angle[i]+90 && now_angle < angle[i]+180){
              while(1){
                turnRight();
                if(abs(enc2_count) >= (angle[i]+180-now_angle)/0.25){
                moveStop(500);
                countReset();
                break;
                }
              }
              while(1){
                goBack();
                if(abs(enc1_count) >= distance[i]/0.2){
                moveStop(500);
                countReset();
                break;
                }
              }
              now_angle = angle[i]+180;
            }
            else if(now_angle >= angle[i]+180 && now_angle < angle[i]+270){
              while(1){
                turnLeft();
                if(abs(enc2_count) >= (now_angle-angle[i]-180)/0.25){
                moveStop(500);
                countReset();
                break;
                }
              }
              while(1){
                goBack();
                if(abs(enc1_count) >= distance[i]/0.2){
                moveStop(500);
                countReset();
                break;
                }
              }
              now_angle = angle[i]+180;
            }
            else if(now_angle >= angle[i]+270 && now_angle < 360){
              while(1){
                turnRight();
                if(abs(enc2_count) >= (360-now_angle+angle[i])/0.25){
                moveStop(500);
                countReset();
                break;
                }
              }
              while(1){
                goForward();
                if(enc1_count >= distance[i]/0.2){
                moveStop(500);
                countReset();
                break;
                }
              }
              now_angle = angle[i];
            }
          }
          else if(angle[i] > 90 && angle[i] < 180){
            if(now_angle >= 0 && now_angle < angle[i]-90){
              while(1){
                turnLeft();
                if(abs(enc2_count) >= (now_angle+180-angle[i])/0.25){
                moveStop(500);
                countReset();
                break;
                }
              }
              while(1){
                goBack();
                if(abs(enc1_count) >= distance[i]/0.2){
                moveStop(500);
                countReset();
                break;
                }
              }
              now_angle = angle[i]+180;
            }
            else if(now_angle >= angle[i]-90 && now_angle < angle[i]){
              while(1){
                turnRight();
                if(abs(enc2_count) >= (angle[i]-now_angle)/0.25){
                moveStop(500);
                countReset();
                break;
                }
              }
              while(1){
                goForward();
                if(enc1_count >= distance[i]/0.2){
                moveStop(500);
                countReset();
                break;
                }
              }
              now_angle = angle[i];
            }
            else if(now_angle >= angle[i] && now_angle < angle[i]+90){
              while(1){
                turnLeft();
                if(abs(enc2_count) >= (now_angle-angle[i])/0.25){
                moveStop(500);
                countReset();
                break;
                }
              }
              while(1){
                goForward();
                if(enc1_count >= distance[i]/0.2){
                moveStop(500);
                countReset();
                break;
                }
              }
              now_angle = angle[i];
            }
            else if(now_angle >= angle[i]+90 && now_angle < angle[i]+180){
              while(1){
                turnRight();
                if(abs(enc2_count) >= (angle[i]+180-now_angle)/0.25){
                moveStop(500);
                countReset();
                break;
                }
              }
              while(1){
                goBack();
                if(abs(enc1_count) >= distance[i]/0.2){
                moveStop(500);
                countReset();
                break;
                }
              }
              now_angle = angle[i]+180;
            }
            else if(now_angle >= angle[i]+180 && now_angle < 360){
              while(1){
                turnLeft();
                if(abs(enc2_count) >= (now_angle-angle[i]-180)/0.25){
                moveStop(500);
                countReset();
                break;
                }
              }
              while(1){
                goBack();
                if(abs(enc1_count) >= distance[i]/0.2){
                moveStop(500);
                countReset();
                break;
                }
              }
              now_angle = angle[i]+180;
            }
          }
          else if(angle[i] > 180 && angle[i] < 270){
            if(now_angle >= 0 && now_angle < angle[i]-180){
              while(1){
                turnRight();
                if(abs(enc2_count) >= (angle[i]-180-now_angle)/0.25){
                moveStop(500);
                countReset();
                break;
                }
              }
              while(1){
                goBack();
                if(abs(enc1_count) >= distance[i]/0.2){
                moveStop(500);
                countReset();
                break;
                }
              }
              now_angle = angle[i]-180;
            }
            else if(now_angle >= angle[i]-180 && now_angle < angle[i]-90){
              while(1){
                turnLeft();
                if(abs(enc2_count) >= (now_angle-angle[i]+180)/0.25){
                moveStop(500);
                countReset();
                break;
                }
              }
              while(1){
                goBack();
                if(abs(enc1_count) >= distance[i]/0.2){
                moveStop(500);
                countReset();
                break;
                }
              }
              now_angle = angle[i]-180;
            }
            else if(now_angle >= angle[i]-90 && now_angle < angle[i]){
              while(1){
                turnRight();
                if(abs(enc2_count) >= (angle[i]-now_angle)/0.25){
                moveStop(500);
                countReset();
                break;
                }
              }
              while(1){
                goForward();
                if(enc1_count >= distance[i]/0.2){
                moveStop(500);
                countReset();
                break;
                }
              }
              now_angle = angle[i];
            }
            else if(now_angle >= angle[i] && now_angle < angle[i]+90){
              while(1){
                turnLeft();
                if(abs(enc2_count) >= (now_angle-angle[i])/0.25){
                moveStop(500);
                countReset();
                break;
                }
              }
              while(1){
                goForward();
                if(enc1_count >= distance[i]/0.2){
                moveStop(500);
                countReset();
                break;
                }
              }
              now_angle = angle[i];
            }
            else if(now_angle >= angle[i]+90 && now_angle < 360){
              while(1){
                turnRight();
                if(abs(enc2_count) >= (180-now_angle+angle[i])/0.25){
                moveStop(500);
                countReset();
                break;
                }
              }
              while(1){
                goBack();
                if(abs(enc1_count) >= distance[i]/0.2){
                moveStop(500);
                countReset();
                break;
                }
              }
              now_angle = angle[i]-180;
            }
          }
          else if(angle[i] > 270 && angle[i] < 360){
            if(now_angle >= 0 && now_angle < angle[i]-270){
              while(1){
                turnLeft();
                if(abs(enc2_count) >= (now_angle-angle[i]+360)/0.25){
                moveStop(500);
                countReset();
                break;
                }
              }
              while(1){
                goForward();
                if(enc1_count >= distance[i]/0.2){
                moveStop(500);
                countReset();
                break;
                }
              }
              now_angle = angle[i];
            }
            else if(now_angle >= angle[i]-270 && now_angle < angle[i]-180){
              while(1){
                turnRight();
                if(abs(enc2_count) >= (angle[i]-180-now_angle)/0.25){
                moveStop(500);
                countReset();
                break;
                }
              }
              while(1){
                goBack();
                if(abs(enc1_count) >= distance[i]/0.2){
                moveStop(500);
                countReset();
                break;
                }
              }
              now_angle = angle[i]-180;
            }
            else if(now_angle >= angle[i]-180 && now_angle < angle[i]-90){
              while(1){
                turnLeft();
                if(abs(enc2_count) >= (now_angle-angle[i]+180)/0.25){
                moveStop(500);
                countReset();
                break;
                }
              }
              while(1){
                goBack();
                if(abs(enc1_count) >= distance[i]/0.2){
                moveStop(500);
                countReset();
                break;
                }
              }
              now_angle = angle[i]-180;
            }
            else if(now_angle >= angle[i]-90 && now_angle < angle[i]){
              while(1){
                turnRight();
                if(abs(enc2_count) >= (angle[i]-now_angle)/0.25){
                moveStop(500);
                countReset();
                break;
                }
              }
              while(1){
                goForward();
                if(enc1_count >= distance[i]/0.2){
                moveStop(500);
                countReset();
                break;
                }
              }
              now_angle = angle[i];
            }
            else if(now_angle >= angle[i] && now_angle < 360){
              while(1){
                turnLeft();
                if(abs(enc2_count) >= (now_angle-angle[i])/0.25){
                moveStop(500);
                countReset();
                break;
                }
              }
              while(1){
                goForward();
                if(enc1_count >= distance[i]/0.2){
                moveStop(500);
                countReset();
                break;
                }
              }
              now_angle = angle[i];
            }
          }
        }
      }
      digitalWrite(50, LOW);
      delay(1000);
    }
//最後の座標まで行ったら停止する
    while(1){
      moveStop(5000);
    }
  }
}

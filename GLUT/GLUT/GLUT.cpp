#include "stdafx.h"
#include "socket_client.h"
#include "arduino.h"
#include <iostream>
#include <sstream>
#include <vector>
#include <thread>
#include <stdlib.h>
#include <gl/glut.h>
#include <tchar.h>

Sock_client client;
Arduino arduino;

int WINW = 800;
int WINH = 600;
const int TEXTMAX = 40;
const int POINTMAX = 10000; // 大量の角を持つ折れ線を描いているのだから、バッファサイズが10点分しかないと線がすぐ消えていってしまう
char text[TEXTMAX];
double xList[POINTMAX];
double yList[POINTMAX];
int point[POINTMAX][2];
int textnum;
int pointnum;
std::vector<std::string> position(4);

std::string send1;
std::string send2;

//GLUT
/*-------------------------------------------------------------------------------------------*/
void DrawString() {
	glRasterPos3d(0, 0, 0);//0,0,0位置をスタート位置にする

	glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 'P');
	glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 'a');
	glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 'i');
	glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 'n');
	glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, 't');
	glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, '!');
}

void myDisplay() {
	int i, j;

	glClearColor(1.0, 1.0, 1.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);

	DrawString();

	glColor3d(1.0, 0.0, 0.0);
	glBegin(GL_LINE_STRIP);
	for (i = 0; i < POINTMAX; i++) {
		j = (pointnum + i) % POINTMAX;
		if (xList[j] == -20.0) {
			// 座標バッファに目印となる値が見つかったら、そこをラインストリップの区切りとする
			glEnd();
			glBegin(GL_LINE_STRIP);
		}
		else if (xList[j] >= -1.0 && yList[j] >= -1.0) {
			glVertex2d(xList[j], yList[j]);
		}
	}
	glEnd();

	glRasterPos2d(-0.9, -0.7);

	glutSwapBuffers();

}


void myKeyboard(unsigned char key, int x, int y) {
	if (key == 0x1B) exit(0);
	text[textnum] = key;
	textnum = (textnum + 1) % TEXTMAX;
	glutPostRedisplay();
}


void myMouseFunc(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
		// マウスボタンが放されたら、目印となる適当な値を座標バッファに入れてやる
		// myDisplay中でその値が見つかったら、そこでラインストリップを区切ってやる
		xList[pointnum] = -20.0;
		yList[pointnum] = 0.0;
		pointnum = (pointnum + 1) % POINTMAX;
		glutPostRedisplay();
	}
}

void myMotionFunc(int x, int y) {
	// 前回のマウス座標と相違があれば、変更前のmyMouseFuncの
	// ifブロック内でやっていたのと同じことを行う
	static int prevX = -10.0, prevY = -10.0;
	if (x == prevX && y == prevY) return;
	prevX = x;
	prevY = y;
	xList[pointnum] = (((double)x / WINW) * 2) - 1;
	yList[pointnum] = ((((double)y / WINH) * 2) - 1)*(-1);

	//COMポート送信用
	point[pointnum][0] = ((double)x / WINW) * 500;
	point[pointnum][1] = ((double)y / WINH) * 500;

	pointnum = (pointnum + 1) % POINTMAX;
	glutPostRedisplay();
}

void myReshape(int width, int height) {
	glViewport(0, 0, width, height);
	WINW = width;
	WINH = height;
}

void myInit(char* progname) {
	int i;
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowSize(WINW, WINH);
	glutInitWindowPosition(0, 0);
	glutCreateWindow(progname);
	pointnum = 0;
	for (i = 0; i < POINTMAX; i++) {
		xList[i] = -10.0;
		yList[i] = -10.0;
	}
	textnum = 0;
	for (i = 0; i < TEXTMAX; i++) {
		text[i] = '?0';
	}
}
/*-------------------------------------------------------------------------------------------*/

//QPComm
/*-------------------------------------------------------------------------------------------*/
//文字列を分割できるように変換．
void Split(std::string origin, const char delim, std::vector<std::string> &ret) {
	ret.clear();

	std::stringstream ss(origin);
	std::string buffer;

	while (std::getline(ss, buffer, delim))ret.push_back(buffer);
}

void SplitMsg(std::string origin, char moji, std::vector<std::string>&ret) {
	std::vector<std::string> sep;
	Split(origin, moji, ret);
}

void Output2Console(std::vector<std::string> after) {
		/*std::cout << "number: " << after[0] << std::endl
			<< "x: " << after[1] << std::endl
			<< "y: " << after[2] << std::endl
			<< "z: " << after[3] << std::endl << std::endl;	*/
	int i;
	for (i = 0; i < 4; i++){
		position[i] = after[i];
	}
}
/*-------------------------------------------------------------------------------------------*/

//Serial
/*-------------------------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------------------------*/


//GLUTのmain関数
/*-------------------------------------------------------------------------------------------*/
void func1(int argc, char* argv[]) {
	glutInit(&argc, argv);
	myInit(argv[0]);
	glutKeyboardFunc(myKeyboard);
	glutMouseFunc(myMouseFunc);
	glutMotionFunc(myMotionFunc); // モーションイベント用のハンドラーを登録
	glutReshapeFunc(myReshape);
	glutDisplayFunc(myDisplay);
	glutMainLoop();
}
/*-------------------------------------------------------------------------------------------*/

//QPcommのmain関数
/*-------------------------------------------------------------------------------------------*/
void func2(){
	client.Init(55555, "10.26.70.228");
	while (1) {
		Sleep(500);
		std::string response;
		std::vector<std::string> buf, buf2, after;
		bool isSucceeded;
		isSucceeded = client.Recv_msg(response);
		if (isSucceeded) {
			if (response != "") {
				SplitMsg(response, '=', buf);
				SplitMsg(buf[1], 'P', buf2);
				for (int i = 1; i < buf2.size(); i++) {
					SplitMsg(buf2[i], ',', after);
					//ここでデータ格納
					int j;
					//for (j = 0; j < after.size() - 1; j++)std::cout << after[i] << ",";
					//std::cout << after[j] << std::endl;
					Output2Console(after);
				}

			}
		}

	}
}
/*-------------------------------------------------------------------------------------------*/

//Serialのmain関数
/*-------------------------------------------------------------------------------------------*/
void func3(){
	while (1){
		int i,j;
		std::stringstream ss1;
		std::stringstream ss2;
		std::stringstream ss3;
		
		if (GetKeyState(VK_RETURN) & 0x8000){
			ss1 << position[1] << ',' << position[2] << ',';
			
			for (i = 0; i < pointnum; i++){
				int num1 = point[i][0];
				int num2 = point[i][1];
				ss1 << num1 << ',' << num2 << ',';
			}
			ss1 << '\0';

			//ss3 << position[1] << ',' << position[2] << ',';
			//ss3 << '\0';

			std::cout << ss1.str() << std::endl;
			//std::cout << ss2.str() << std::endl;
			//std::cout << ss3.str() << std::endl;
			arduino.Send1_String(ss1.str());
			//arduino.Send2_String(ss1.str());
			//arduino.Send3_String(ss3.str());

			//出力確認用
			/*
			for (k = 0; k < pointnum - 1; k++){
				std::cout << point[k][0] << "\t" << point[k][1] << std::endl;
			}
			std::cout << "number: " << position[0] << std::endl
				<< "x: " << position[1] << std::endl
				<< "y: " << position[2] << std::endl
				<< "z: " << position[3] << std::endl << std::endl;
			Sleep(2000);
			*/
		}
	}
}
/*-------------------------------------------------------------------------------------------*/

int main(int argc, char* argv[]){
	std::thread th1(func1,argc,argv);
	std::thread th2(func2);
	std::thread th3(func3);

	th1.join();
	th2.join();
	th3.join();

	return 0;
}



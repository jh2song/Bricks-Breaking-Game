/*
* 컴퓨터그래픽스I 최종 프로젝트 - 벽돌 깨기 게임
* 
* Member
* 학번: 20183016 / 이름: 송지훈 / 학과: 컴퓨터공학과
* 
*/
#include <windows.h>
#include <cmath>
#include <gl/GL.h>
#include <gl/glut.h>
#include <iostream>
using namespace std;

#define PI 3.1415926

// Windows Setting
float windowWidth = 500.0f;
float windowHeight = 500.0f;
// --------------------------



// Game Variables
bool isGameOver = false;

GLfloat ball_start_x = 250.0f;
GLfloat ball_start_y = 350.0f;
GLfloat ball_start_x_velocity = 0.0f;
GLfloat ball_start_y_velocity = 0.5f;
GLfloat ball_radius = 5.0f;

typedef struct
{
	GLfloat cx;
	GLfloat cy;
	GLfloat x_velocity;
	GLfloat y_velocity;
}Ball;

Ball ball;

GLfloat block_x_start = 50.0;
GLfloat block_y_start = 50.0;
GLfloat block_x_size = 100.0;
GLfloat block_y_size = 60.0;

typedef struct
{
	GLfloat x;
	GLfloat y;
	bool isAlive;
}Block;

Block block[12];

int aliveBlock = 12;

int point[100][2];
int num = 0;

typedef struct
{
	int xStart;
	int xEnd;
	// ax + by + c = 0
	int a;
	int b;
	int c;
}Segment;

Segment seg[30];
// ---------------------------



// Functions' Header
void Init();
void BlockInit();
void BallInit();
void RenderScene();
void Collision_Check();
void Draw_Circle();
void Draw_Bricks();
void MyMouse(int button, int state, int x, int y);
void MyReshape(int w, int h);
double Combination(int n, int k);
int Factorial(int k);
double Bernstein(int n, int k, double time);
void Gameover_Check();
void GameFinish_Check();
// ----------------------------



// Main Function
int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(windowWidth, windowHeight);
	glutCreateWindow("컴퓨터그래픽스I 최종 프로젝트 - 벽돌 깨기 게임");
	Init();
	glutDisplayFunc(RenderScene);
	/*glutSpecialFunc(Keys);
	glutKeyboardFunc(MyKey);*/
	glutMouseFunc(MyMouse);
	glutReshapeFunc(MyReshape);
	glutIdleFunc(RenderScene);
	glutMainLoop();

	return 0;
}
// ------------------------------



// Functions
void Init()
{
	BlockInit();
	BallInit();
}

void BlockInit()
{
	block[0].x = block_x_start;
	block[0].y = block_y_start;
	block[1].x = block_x_start + block_x_size;
	block[1].y = block_y_start;
	block[2].x = block_x_start + block_x_size * 2;
	block[2].y = block_y_start;
	block[3].x = block_x_start + block_x_size * 3;
	block[3].y = block_y_start;

	block[4].x = block_x_start;
	block[4].y = block_y_start + block_y_size;
	block[5].x = block_x_start + block_x_size;
	block[5].y = block_y_start + block_y_size;
	block[6].x = block_x_start + block_x_size * 2;
	block[6].y = block_y_start + block_y_size;
	block[7].x = block_x_start + block_x_size * 3;
	block[7].y = block_y_start + block_y_size;

	block[8].x = block_x_start;
	block[8].y = block_y_start + block_y_size * 2;
	block[9].x = block_x_start + block_x_size;
	block[9].y = block_y_start + block_y_size * 2;
	block[10].x = block_x_start + block_x_size * 2;
	block[10].y = block_y_start + block_y_size * 2;
	block[11].x = block_x_start + block_x_size * 3;
	block[11].y = block_y_start + block_y_size * 2;

	for (int i = 0; i < 12; i++)
		block[i].isAlive = true;
}

void BallInit()
{
	ball.cx = ball_start_x;
	ball.cy = ball_start_y;
	ball.x_velocity = ball_start_x_velocity;
	ball.y_velocity = ball_start_y_velocity;
}

int Factorial(int k) 
{
	if (k <= 1) return 1;
	else return k * Factorial(k - 1);
}

double Combination(int n, int k)
{
	double value;
	if (n >= 1 && k <= n)
		value = Factorial(n) / (Factorial(n - k) * Factorial(k));
	else
		value = 1;
	return value;
}

double Bernstein(int n, int k, double time) 
{
	double value;
	if (0.0 <= time && time <= 1.0)
		value = Combination(n, k) * pow(1.0 - time, n - k) * pow(time, k);
	else
		value = 1.0;
	return value;
}

void Draw_Bezier_Curve(void) {
	double px, py;
	double sumx, sumy;
	double time;
	double function_value;
	double partial_time;
	int curvepoint_num = 30;
	int curve_degree;
	glColor3f(1.0, 1.0, 0.0);
	curve_degree = num - 1;
	partial_time = 1.0 / curvepoint_num;
	px = point[0][0];
	py = point[0][1];

	glBegin(GL_LINES);
	int idx = 0;
	for (time = partial_time; time <= 1.0; time += partial_time) {
		sumx = sumy = 0.0;
		for (int k = 0; k <= curve_degree; k++) {
			function_value = Bernstein(curve_degree, k, time);
			sumx += function_value * point[k][0];
			sumy += function_value * point[k][1];
		}
		glVertex2d(px, py);
		glVertex2d(sumx, sumy);

		seg[idx].xStart = px <= sumx ? px : sumx;
		seg[idx].xEnd = px > sumx ? px : sumx;
		// 두 점이 주어졌을 때 직선의 방정식 ax+by+c=0에서 a,b,c를 구하는 공식
		seg[idx].a = (sumy - py);
		seg[idx].b = (px - sumx);
		seg[idx].c = (sumx - px) * py - (sumy - py) * px;
		idx++;

		px = sumx;
		py = sumy;
	}

	sumx = point[curve_degree][0];
	sumy = point[curve_degree][1];
	glVertex2d(px, py);
	glVertex2d(sumx, sumy);

	seg[idx].xStart = px <= sumx ? px : sumx;
	seg[idx].xEnd = px > sumx ? px : sumx;
	// 두 점이 주어졌을 때 직선의 방정식 ax+by+c=0에서 a,b,c를 구하는 공식
	seg[idx].a = (sumy - py);
	seg[idx].b = (px - sumx);
	seg[idx].c = (sumx - px) * py - (sumy - py) * px;
	idx++;

	glEnd();
}

void RenderScene()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	Gameover_Check();
	GameFinish_Check();

	if (!isGameOver && aliveBlock > 0)
		Collision_Check();

	// Moving Ball
	glColor3f(1.0f, 0.0f, 0.0f);
	Draw_Circle();

	// Bricks
	glColor3f(0.0f, 0.0f, 1.0f);
	Draw_Bricks();

	// Bezier Curve
	if (num >= 2)
		Draw_Bezier_Curve();

	glFlush();
	glutSwapBuffers();
}

void Gameover_Check()
{
	if (!isGameOver)
		return;
	
	glColor3f(1.0f, 1.0f, 1.0f);
	glRasterPos2f(20, 450);
	
	glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_10, 'Y');
	glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_10, 'O');
	glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_10, 'U');
	glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_10, ' ');
	glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_10, 'L');
	glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_10, 'O');
	glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_10, 'S');
	glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_10, 'E');
	glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_10, '!');
	glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_10, '!');
	glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_10, '!');
}

void GameFinish_Check()
{
	if (aliveBlock > 0)
		return;

	glColor3f(1.0f, 1.0f, 1.0f);
	glRasterPos2f(20, 450);

	glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_10, 'Y');
	glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_10, 'O');
	glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_10, 'U');
	glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_10, ' ');
	glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_10, 'W');
	glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_10, 'I');
	glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_10, 'N');
	glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_10, '!');
	glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_10, '!');
	glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_10, '!');
}

void Collision_Check()
{
	// board boundary's collision check
	if (ball.cx - ball_radius <= 0 || ball.cx + ball_radius >= windowWidth)
		ball.x_velocity = -ball.x_velocity;
	if (ball.cy - ball_radius <= 0)
		ball.y_velocity = -ball.y_velocity;
	if (ball.cy + ball_radius >= windowHeight)
		isGameOver = true;

	// Bezier's Curve collision check

	if (num > 0)
	{
		for (int i = 0; i < 30; i++)
		{
			float ta;
			float tb;
			float tc;

			float d;

			ta = seg[i].a;
			tb = seg[i].b;
			tc = seg[i].c;

			d = abs(ta * ball.cx + tb * ball.cy + tc) / (float)sqrt(pow(ta, 2) + pow(tb, 2));

			if (d <= ball_radius)
			{
				num = 0;
				memset(&seg, 0, sizeof(Segment));
				memset(point, 0, sizeof(int));
				ball.y_velocity = -ball.y_velocity;
				ball.x_velocity = ta;
				cout << d << "\n";
				break;
			}
		}
	}

	// Bricks collision check
	for (int i = 0; i < 12; i++)
	{
		if (block[i].isAlive)
		{
			// 벽돌의 4면(직선)과 공의 거리를 구하는 방식으로 충돌 체크
			float ta;
			float tb;
			float tc;

			GLfloat x1;
			GLfloat y1;
			GLfloat x2;
			GLfloat y2;

			float d;

			// brick's up collision check
			x1 = block[i].x;
			y1 = block[i].y;
			x2 = block[i].x + block_x_size;
			y2 = y1;

			ta = (y2 - y1);
			tb = (x1 - x2);
			tc = (x2 - x1) * y1 - (y2 - y1) * x1;

			d = abs(ta * ball.cx + tb * ball.cy + tc) / (float)sqrt(pow(ta, 2) + pow(tb, 2));
			if (d <= ball_radius
				&& ball.cx + ball_radius >= x1 && ball.cx - ball_radius <= x2) // 해당 공의 x축이 벽돌 위에 있는지 체크
			{
				block[i].isAlive = false;
				aliveBlock--;
				ball.y_velocity = -ball.y_velocity;
				break;
			}

			// brick's down collision check
			x1 = block[i].x;
			y1 = block[i].y + block_y_size;
			x2 = block[i].x + block_x_size;
			y2 = y1;

			ta = (y2 - y1);
			tb = (x1 - x2);
			tc = (x2 - x1) * y1 - (y2 - y1) * x1;

			d = abs(ta * ball.cx + tb * ball.cy + tc) / (float)sqrt(pow(ta, 2) + pow(tb, 2));
			if (d <= ball_radius
				&& ball.cx + ball_radius >= x1 && ball.cx - ball_radius <= x2) // 해당 공의 x축이 벽돌 아래에 있는지 체크
			{
				block[i].isAlive = false;
				aliveBlock--;
				ball.y_velocity = -ball.y_velocity;
				break;
			}

			// brick's left collision check
			x1 = block[i].x;
			y1 = block[i].y;
			x2 = x1;
			y2 = block[i].y + block_y_size;

			ta = (y2 - y1);
			tb = (x1 - x2);
			tc = (x2 - x1) * y1 - (y2 - y1) * x1;

			d = abs(ta * ball.cx + tb * ball.cy + tc) / (float)sqrt(pow(ta, 2) + pow(tb, 2));
			if (d <= ball_radius
				&& ball.cy + ball_radius >= y1 && ball.cy - ball_radius <= y2) // 해당 공의 x축이 벽돌 왼쪽에 있는지 체크
			{
				block[i].isAlive = false;
				aliveBlock--;
				ball.x_velocity = -ball.x_velocity;
				break;
			}

			// brick's right collision check
			x1 = block[i].x + block_x_size;
			y1 = block[i].y;
			x2 = x1;
			y2 = block[i].y + block_y_size;

			ta = (y2 - y1);
			tb = (x1 - x2);
			tc = (x2 - x1) * y1 - (y2 - y1) * x1;

			d = abs(ta * ball.cx + tb * ball.cy + tc) / (float)sqrt(pow(ta, 2) + pow(tb, 2));
			if (d <= ball_radius
				&& ball.cy + ball_radius >= y1 && ball.cy - ball_radius <= y2) // 해당 공의 x축이 벽돌 오른쪽에 있는지 체크
			{
				block[i].isAlive = false;
				aliveBlock--;
				ball.x_velocity = -ball.x_velocity;
				break;
			}
		}
	}

	ball.cx += ball.x_velocity;
	ball.cy += ball.y_velocity;
}

void Draw_Circle()
{
	float delta;
	int num = 36;

	delta = 2 * PI / num;
	glBegin(GL_POLYGON);
	for (int i = 0; i < num; i++)
		glVertex2f(ball.cx + ball_radius * cos(delta * i), 
			ball.cy + ball_radius * sin(delta * i));
	glEnd();
}

void Draw_Bricks()
{
	for (int i = 0; i < 12; i++)
	{
		if (block[i].isAlive)
		{
			glBegin(GL_POLYGON);
				glVertex2f(block[i].x, 
					block[i].y);
				glVertex2f(block[i].x + block_x_size, 
					block[i].y);
				glVertex2f(block[i].x + block_x_size,
					block[i].y + block_y_size);				
				glVertex2f(block[i].x,
					block[i].y + block_y_size);
			glEnd();
		}
	}
}

void MyMouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		point[num][0] = x;
		point[num][1] = windowHeight - y;
		num++;
	}

	glutPostRedisplay();
	RenderScene();
}

void MyReshape(int w, int h)
{
	glViewport(0, 0, w, h);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, 500, 0, 500);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}
// -------------------------------

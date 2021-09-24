#include <iostream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <glut.h>
#define GLUT_KEY_ESCAPE 27
#define DEG2RAD(a) (a * 0.0174532925)
int roundscore = 0;
int width;
int height;
bool playing = true;
int gamecounter = 1;
using namespace std;
bool movingsphere = false;
char previousmotion = 'a';
char cameramode = 's';
float z = 1;//1
int cgl, cgr, cyl, cyr = 0;
int topblue, bottomyellow, bottomred = 0;
float bounce = -0.5;//-0.5
float xbounce = 0;//0
double r = 180;
double r1 = -40;
void drawText(const char* text, int length, int x, int y);
bool moveup, movedown, moveright, nomove, moveleft = false;
int score = 0;
//bool moveupslowly = false;
bool horizontal = true;
bool up = true;
bool slowmotion = false;

//
/*int movex = -1;
int movey = 0;
int movez = 0.9;
bool up, down, left, right = false;*/
void Key(unsigned char key, int x, int y);

static int useRGB = 1;
static int useLighting = 1;
static int useDB = 1;

static int useQuads = 1;

class Vector3f {
public:
	float x, y, z;

	Vector3f(float _x = 0.0f, float _y = 0.0f, float _z = 0.0f) {
		x = _x;
		y = _y;
		z = _z;
	}

	Vector3f operator+(Vector3f& v) {
		return Vector3f(x + v.x, y + v.y, z + v.z);
	}

	Vector3f operator-(Vector3f& v) {
		return Vector3f(x - v.x, y - v.y, z - v.z);
	}

	Vector3f operator*(float n) {
		return Vector3f(x * n, y * n, z * n);
	}

	Vector3f operator/(float n) {
		return Vector3f(x / n, y / n, z / n);
	}

	Vector3f unit() {
		return *this / sqrt(x * x + y * y + z * z);
	}

	Vector3f cross(Vector3f v) {
		return Vector3f(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
	}
};

class Camera {
public:
	Vector3f eye, center, up;

	Camera(float eyeX = 0.0f, float eyeY = 0, float eyeZ = 3.13, float centerX = 0.0f, float centerY = 0.0f, float centerZ = 0.0f, float upX = 0.0f, float upY = 1.0f, float upZ = 0.0f) {
		eye = Vector3f(eyeX, eyeY, eyeZ);
		center = Vector3f(centerX, centerY, centerZ);
		up = Vector3f(upX, upY, upZ);
	}
	//eyeZ el naf3t 3.13
	void moveX(float d) {
		Vector3f right = up.cross(center - eye).unit();
		eye = eye + right * d;
		center = center + right * d;
	}

	void moveY(float d) {
		eye = eye + up.unit() * d;
		center = center + up.unit() * d;
	}

	void moveZ(float d) {
		Vector3f view = (center - eye).unit();
		eye = eye + view * d;
		center = center + view * d;
	}

	void rotateX(float a) {
		Vector3f view = (center - eye).unit();
		Vector3f right = up.cross(view).unit();
		view = view * cos(DEG2RAD(a)) + up * sin(DEG2RAD(a));
		up = view.cross(right);
		center = eye + view;
	}

	void rotateY(float a) {
		Vector3f view = (center - eye).unit();
		Vector3f right = up.cross(view).unit();
		view = view * cos(DEG2RAD(a)) + right * sin(DEG2RAD(a));
		right = view.cross(up);
		center = eye + view;
	}

	void look() {
		if (cameramode == 'e') {
			gluLookAt(
				eye.x, eye.y, eye.z + z - 1 + 0.1,
				center.x+xbounce, center.y+bounce, center.z,
				up.x, up.y, up.z
			);
		}
		else {
			gluLookAt(
				eye.x, eye.y, eye.z + z - 1 + 0.2,
				center.x, center.y, center.z,
				up.x, up.y, up.z
			);
		}
	}
};

Camera camera;


#define GREY	0
#define RED	1
#define GREEN	2
#define BLUE	3
#define CYAN	4
#define MAGENTA	5
#define YELLOW	6
#define BLACK	7

static float materialColor[8][4] =
{
  {0.8, 0.8, 0.8, 1.0},
  {0.8, 0.0, 0.0, 1.0},
  {0.0, 0.8, 0.0, 1.0},
  {0.0, 0.0, 0.8, 1.0},
  {0.0, 0.8, 0.8, 1.0},
  {0.8, 0.0, 0.8, 1.0},
  {0.8, 0.8, 0.0, 1.0},
  {0.0, 0.0, 0.0, 0.6},
};
static float lightDiff[4] =
{ 0.8, 0.8, 0.8, 1.0 };
static float lightSpec[4] =
{ 0.4, 0.4, 0.4, 1.0 };

static float lightPos[4] =
{ 2.0, 4.0, 2.0, 1.0 };
#if 0
static float lightDir[4] =
{ -2.0, -4.0, -2.0, 1.0 };
#endif
static float lightAmb[4] =
{ 0.2, 0.2, 0.2, 1.0 };



static float groundPlane[4] =
{ 0.0, 1.0, 0.0, 1.499 };
static float backPlane[4] =
{ 0.0, 0.0, 1.0, 0.899 };


static void
setColor(int c)
{

	if (useRGB) {
		glMaterialfv(GL_FRONT_AND_BACK,
			GL_AMBIENT_AND_DIFFUSE, &materialColor[c][0]);
	}
	else {
		glMaterialfv(GL_FRONT_AND_BACK,
			GL_COLOR_INDEXES, &materialColor[c][0]);
	}


}


static void
drawCheck(int w, int h, int evenColor, int oddColor, int evenColor1, int oddColor1)
{
	static int initialized = 0;
	static int usedLighting = 0;


	if (!initialized || (usedLighting != useLighting)) {
		static float square_normal[4] =
		{ 0.0, 0.0, 1.0, 0.0 };
		static float square[4][4];
		int i, j;



		if (useQuads) {
			glNormal3fv(square_normal);
			glBegin(GL_QUADS);
		}
		for (j = 0; j < h; ++j) {
			for (i = 0; i < w; ++i) {
				square[0][0] = -1.0 + 2.0 / w * i;
				square[0][1] = -1.0 + 2.0 / h * (j + 1);
				square[0][2] = 0.0;
				square[0][3] = 1.0;

				square[1][0] = -1.0 + 2.0 / w * i;
				square[1][1] = -1.0 + 2.0 / h * j;
				square[1][2] = 0.0;
				square[1][3] = 1.0;

				square[2][0] = -1.0 + 2.0 / w * (i + 1);
				square[2][1] = -1.0 + 2.0 / h * j;
				square[2][2] = 0.0;
				square[2][3] = 1.0;

				square[3][0] = -1.0 + 2.0 / w * (i + 1);
				square[3][1] = -1.0 + 2.0 / h * (j + 1);
				square[3][2] = 0.0;
				square[3][3] = 1.0;
				if (i % 2 == 0) {
					if (i & 1 ^ j & 1) {
						setColor(oddColor);
					}
					else {
						setColor(evenColor);
					}

				}
				else {
					if (i & 1 ^ j & 1) {
						setColor(oddColor1);
					}
					else {
						setColor(evenColor1);
					}
				}


				if (!useQuads) {
					glBegin(GL_POLYGON);
				}
				glVertex4fv(&square[0][0]);
				glVertex4fv(&square[1][0]);
				glVertex4fv(&square[2][0]);
				glVertex4fv(&square[3][0]);
				if (!useQuads) {
					glEnd();
				}
			}
		}

		if (useQuads) {
			glEnd();
		}


		initialized = 1;

	}

}



static char* windowNameRGBDB = "BOUNCY BALL";
void Key(unsigned char key, int x, int y)//mouse function takes 4 parameters: button: which button has been clicked (GLUT_RIGHT_BUTTON or GLUT_LEFT_BUTTON),
											//state wether the button is clicked or released (GLUT_UP or GLUT_DOWN)
											// x and y are the position of the mouse cursor
{
	if (playing) {

		if (key == 'e') {
			
			if (cameramode == 'e')
			{
				cameramode = 's';
			}
			else
			{
				cameramode = 'e';
			}
		}
		if (key == 'x') {

			if (r1 < -45)
			{
				moveup = true;
			}
			else {
				if (r < 145)
				{
					moveright = true;
				}
				if (r >= 145 && r <= 215 && r1 >= -45 && r1 <= -15)
				{
					nomove = true;
				}
			}
			if (r > 215)
			{
				moveleft = true;

			}



			movingsphere = true;
		}




		if (movingsphere == false) {
			if (key == 'c') {

				if (previousmotion == 'u') {
					moveup = true;
					slowmotion = true;
					movingsphere = true;

				}
				if (previousmotion == 'n') {
					nomove = true;
					slowmotion = true;
					movingsphere = true;

				}
				if (previousmotion == 'r') {
					moveright = true;
					slowmotion = true;
					movingsphere = true;

				}
				if (previousmotion == 'l') {
					moveleft = true;
					slowmotion = true;
					movingsphere = true;
				}
				if (previousmotion == 'o') {
					moveleft = true;
					slowmotion = true;
					movingsphere = true;
					moveup = true;
				}


			}
			if (key == 'w') {
				if (r1 > -70)
					r1--;

			}

			if (key == 's') {
				//movedown = true;
				if (r1 < 10)
					r1++;

			}
			if (key == 'd') {
				//moveright = true;
				if (r > 110)
					r--;
			}
			if (key == 'a') {
				//moveleft = true;
				if (r < 250)
					r++;
			}

		}
	}
	glutPostRedisplay();//redisplay to update the screen with the new paraeters
}

void Anim() {

	if (slowmotion) {
		if (moveup) {


			if (up)
			{
				bounce += 0.001;//bounce += 0.005;z -= 0.0005;
				z -= 0.0001;
			}
			else {
				bounce -= 0.001;
				z -= 0.0001;
			}
			if (bounce >= 1) {
				up = false;
			}
			if (bounce <= -1.5) {
				up = true;
			}

		}
		if (nomove) {
			z -= 0.0009;
		}
		if (moveright) {
			if (horizontal) {
				xbounce += 0.001;
				z -= 0.0002;
			}
			else {
				xbounce -= 0.001;
				z -= 0.0001;

			}
			if (xbounce >= 1) {
				horizontal = false;
				//cout << xbounce;
			}
			if (xbounce <= -1) {
				horizontal = true;
				//cout << xbounce;
			}
		}
		if (moveleft) {

			if (horizontal) {
				xbounce -= 0.001;
				z -= 0.0001;
			}
			else {
				xbounce += 0.001;
				z -= 0.0001;
			}
			if (xbounce >= 1) {
				horizontal = true;
			}
			if (xbounce <= -1) {
				horizontal = false;
			}
		}

	}
	else {
		if (nomove) {
			previousmotion = 'n';
			z -= 0.009;
		}

		/*if (movedown) {
			if (up)
			{
				bounce += 0.005;
				z -= 0.0005;
			}
			else {
				bounce -= 0.005;
				z -= 0.0005;
			}
			if (bounce >= 1) {
				up = false;
			}
			if (bounce <= -1.5) {
				up = true;
			}

		}*/
		else {
			if (moveup) {
				previousmotion = 'u';
				if (up)
				{
					bounce += 0.005;//bounce += 0.005;z -= 0.0005;
					z -= 0.0005;
				}
				else {
					bounce -= 0.005;
					z -= 0.0005;
				}
				if (bounce >= 1) {
					up = false;
				}
				if (bounce <= -1.5) {
					up = true;
				}

			}

			if (moveleft) {
				previousmotion = 'l';
				if (horizontal) {
					xbounce -= 0.005;
					z -= 0.00054;
				}
				else {
					xbounce += 0.005;
					z -= 0.00054;
				}
				if (xbounce >= 1) {
					horizontal = true;
				}
				if (xbounce <= -1) {
					horizontal = false;
				}
			}
			if (moveright) {
				previousmotion = 'r';
				if (horizontal) {
					xbounce += 0.005;
					z -= 0.0006;
				}
				else {
					xbounce -= 0.005;
					z -= 0.0006;

				}
				if (xbounce >= 1) {
					horizontal = false;
					cout << xbounce;
				}
				if (xbounce <= -1) {
					horizontal = true;
					cout << xbounce;
				}
			}
			if (moveup && moveleft) {
				previousmotion = 'o';
			}

		}

		if (z <= 0.08 && xbounce <= -0.29 && xbounce >= -0.61 && bounce <= 0) {//green brick left wall
			if (cgl == 0)
			{
				cgl += 1;
				score -= 1;
				roundscore -= 1;
			}
		}

		if (xbounce > 0.62 && bounce <= 0) {//green brick right wall
			if (cgr == 0)
			{
				cgr += 1;
				score -= 1;
				roundscore -= 1;
			}

		}
		if (xbounce <= -0.62 && xbounce >= -1.04 && bounce < 0) {

			if (cyl == 0)
			{
				score += 20;
				roundscore += 20;
				cyl += 1;
			}

		}


		if (xbounce >= 0.3 && xbounce < 0.52 && bounce < 0 && z < 0.5) {
			if (cyl == 1 && cgr == 1 && cyr == 0) {
				score += 20;
				roundscore += 20;
				cyr += 1;
			}
		}
		if (xbounce >= 0 && xbounce <= 0.7 && bounce >= 0.5 && bounce <= 1) {//green top wall
			if (cgl == 0)
			{
				cgl += 1;
				score -= 1;
				roundscore -= 1;
			}
		}
		if (xbounce >= 0 && xbounce <= 0.7 && bounce >= 0.5 && bounce <= 1 && z < 0.5) {
			if (topblue == 0) {
				topblue += 1;
				score -= 5;
				roundscore -= 5;
			}
		}

		if (xbounce >= 0 && xbounce <= 0.7 && bounce >= -1.3 && bounce <= -0.8 && z < 0.5) {
			if (bottomyellow == 0) {
				bottomyellow += 1;
				score += 20;
				roundscore += 20;
			}
		}
		if (xbounce >= 0 && xbounce <= 0.7 && bounce < -1.3) {
			if (bottomred == 0) {
				bottomred += 1;
				score += 30;
				roundscore += 30;
			}
		}


	}


	if (z < -0.00005) {
		z = 1;
		topblue = 0;
		roundscore = 0;
		cyl = 0;
		cgl = 0;
		cgr = 0;
		cyr = 0;
		moveup = false;
		if (slowmotion == false) {
			gamecounter += 1;
		}
		slowmotion = false;
		movedown = false;
		nomove = false;
		moveright = false;
		moveleft = false;
		movingsphere = false;
		r = 180;
		r1 = -45;
		up = true;
		horizontal = true;
		bounce = -0.5;
		xbounce = 0;

		if (gamecounter > 5) {
			playing = false;
		}
	}


	glutPostRedisplay();
}

//void setupCamera() {

	//glOrtho(-0.5, 0.5, -2, 0.5, -1, 1);//regionwhere i draw
	//gluPerspective(60, 640 / 480, 0.001, 100);

	//glMatrixMode(GL_MODELVIEW);
//	glLoadIdentity();
	//gluLookAt(10, 0, 0, 0, 0, 0, 0, 1.0, 0);
//}
void setupCamera() {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, width / height, 0.001, 100);


	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	camera.look();
	glutPostRedisplay();
}

void
display(void)
{
	setupCamera();
	GLfloat cubeXform[4][4];

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPushMatrix();
	glTranslatef(0.0, -1.5, 1.3);//z was o
	glRotatef(-90.0, 1, 0, 0);
	glScalef(2.0, 2.0, 2.0);

	drawCheck(4, 4, RED, YELLOW, GREEN, BLUE); // draw ground 
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-1, 0, -1);
	glRotatef(75, 0, 1, 0);
	glScalef(2.0, 2.0, 2.0);

	drawCheck(4, 4, RED, YELLOW, GREEN, BLUE);  //draw left 
	glPopMatrix();

	glPushMatrix();
	glTranslatef(1, 0.0, -1);
	glRotatef(-75, 0, 1, 0);
	glScalef(2.0, 2.0, 2.0);

	drawCheck(4, 4, RED, YELLOW, GREEN, BLUE);  //draw RIGHT 
	glPopMatrix();
	glPushMatrix();
	glTranslatef(0, -0.19, 0.1);
	glScalef(0.7, 1, 2);

	drawCheck(2, 2, GREY, GREY, GREY, GREY);  //draw END WALL 
	glPopMatrix();


	glPushMatrix();
	glTranslatef(1, 1, 1.4);//z was -1
	glRotatef(90, 1, 0, 0);
	glScalef(2.0, 2.0, 2.0);

	drawCheck(4, 4, RED, YELLOW, GREEN, BLUE);  // draw up 
	glPopMatrix();

	//put this back

	glPushMatrix();
	glTranslated(0.1 + xbounce, bounce, z);
	glutSolidSphere(0.1, 11, 11);
	glPopMatrix();

	glPushMatrix();
	glColor3f(0.6, 0.6, 0.6);
	glTranslated(0, -1.4, 0.3);//0,-1.5,0.3
	glRotated(r, 0, 1, 0);
	glRotated(r1, 1, 0, 0);
	glutSolidCone(0.06, 0.2, 12, 9);
	glPopMatrix();

	std::string text;
	std::string txt2 = "The score is:";
	std::string game_instr = "Press 'w' as Up";
	std::string game_instr1 = "Press 's' as down";
	std::string game_instr2 = "Press 'd' as right";
	std::string game_instr3 = "Press 'a' as left";
	std::string game_instr4 = "Press 'x' to shoot";
	std::string game_instr5 = "Press 'c' for slow replay";
	std::string game_instr6 = "RED for +30! YELLOW for +20! GREEN for -1! BLUE for -5!";
	std::string txt3;
	if (cameramode == 's') {
		txt3 = "SIMPLE CAMERA ON";
	}
	else {
		txt3 = "ENHANCED CAMERA ON";
	}
	
	std::stringstream sstm;
	sstm << txt2 << score;
	text = sstm.str();

	std::string txt22 = "This round's score is:";
	std::string rscore;
	std::stringstream sstm1;
	sstm1 << txt22 << roundscore;
	rscore = sstm1.str();
	drawText(rscore.data(), rscore.size(), 276, 300);
	glPushMatrix();


	drawText(text.data(), text.size(), 30, 480);//130

	drawText(game_instr.data(), game_instr.size(), 30, 460);//130
	drawText(game_instr1.data(), game_instr1.size(), 30, 450);//130
	drawText(game_instr2.data(), game_instr2.size(), 30, 440);//130
	drawText(game_instr3.data(), game_instr3.size(), 30, 430);//130
	drawText(game_instr4.data(), game_instr4.size(), 30, 420);//130
	drawText(game_instr5.data(), game_instr5.size(), 30, 410);//130
	drawText(game_instr6.data(), game_instr6.size(), 273, 310);
	drawText(txt3.data(), txt3.size(), 30, 400);//130
	glPopMatrix();

	/*glPushMatrix();

	glTranslated(-1.81, -0.5, 1.2);
	glRotatef(45, 0, 1, 0);
	glBegin(GL_QUADS);
	glColor3f(0, 0, 0);
	glVertex3f(1.2, 0, 0.0f);
	glVertex3f(1.52, 0, 0.0f);
	glVertex3f(1.52, 0.5, 0.0f);
	glVertex3f(1.2, 0.5, 0.0f);
	glEnd();
	glPopMatrix();
	glPushMatrix();
	glTranslated(0.2, -0.5, 0.5);
	//glRotatef(-90, 0, 0, 1);
	glBegin(GL_QUADS);
	glColor3f(0, 0, 0);
	glVertex3f(0, 0, 0.0f);
	glVertex3f(0.7, 0, 0.0f);
	glVertex3f(0.7, 0.5, 0.0f);
	glVertex3f(0, 0.5, 0.0f);
	glEnd();
	glPopMatrix();*/



	glDepthMask(GL_FALSE);
	if (useRGB) {
		glEnable(GL_BLEND);
	}


	glDepthMask(GL_TRUE);
	if (useRGB) {
		glDisable(GL_BLEND);
	}


	glutSwapBuffers();

	glFlush();


}




void drawText(const char* text, int length, int x, int y) {
	glMatrixMode(GL_PROJECTION);
	double* matrix = new double[16];
	glGetDoublev(GL_PROJECTION_MATRIX, matrix);
	glLoadIdentity();
	glOrtho(0, 800, 0, 600, -5, 5);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glPushMatrix();
	glLoadIdentity();
	glRasterPos2i(x, y);
	for (int i = 0;i < length;i++) {
		glutBitmapCharacter(GLUT_BITMAP_9_BY_15, (int)text[i]);

	}
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixd(matrix);
	glMatrixMode(GL_MODELVIEW);
}


int
main(int argc, char** argv)
{
	width = glutGet(GLUT_SCREEN_WIDTH);
	height = glutGet(GLUT_SCREEN_HEIGHT) - 90;
	int i;
	char* name;


	glutInitWindowSize(width, height);
	glutInit(&argc, argv);


	/* choose visual */
	if (useRGB) {
		if (useDB) {
			glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
			name = windowNameRGBDB;
		}

	}


	glutCreateWindow(name);



	glutDisplayFunc(display);


	glutKeyboardFunc(Key);
	glutIdleFunc(Anim);



	/* setup context */
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-1.0, 1.0, -1.0, 1.0, 1.0, 3.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0, 0.0, -2.0);




	glEnable(GL_DEPTH_TEST);
	if (useLighting) {
		glEnable(GL_LIGHTING);
	}
	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmb);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiff);
	glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpec);
#if 0
	glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, lightDir);
	glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, 80);
	glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 25);
#endif

	glEnable(GL_NORMALIZE);

	glClearColor(0.0, 0.0, 0.0, 1);
	glClearIndex(0);
	glClearDepth(1);

	glutMainLoop();
	return 0;             /* ANSI C requires main to return int. */
}
#include <GL/glut.h>
#include <iostream>
#include <algorithm>
#include <cstdio>
#define _USE_MATH_DEFINES
#include <cmath>
using namespace std;

//global variables
int width = 800;
int height = 800;

//color
GLubyte R = 0, B = 0, G = 0;
int colorIncrementer = 25;

//state variables
bool isMousePressed = false;

//tool states
bool isPenToolActive = true;
bool isKaleidoscope = true;
int symmetryLines = 30;
bool doubleSymmetry = true;
bool isRandom=false;
//Window_display
void InitWindow();
void display(void);
void CreateMyMenu();

//contoller
void keyboard(unsigned char, int, int);
void keyboardUp(unsigned char, int, int);
void mouseFunc(int, int, int, int);
void motionFunc(int, int);
void passiveMotionFunc(int, int);

//tools
void PenTool(int, int, int, int);
void ChangeColorUp();
void ChangeColorDown();

//drawing
void RecordPoints(int, int);
void SetColor(GLubyte, GLubyte, GLubyte);

//Ansilary drawing
void ToggleTools(bool &);

void RotatePoint(int &, int &, int, int, double);

//menu
void MainMenu(int);
void ToolSelectorMenu(int);
void ColorSelectorMenu(int);

//Drawing Algorithms
void DrawLine(int x1, int y1, int x2, int y2);
//void DrawCircle(int h, int k, int radius);
//double CalculateEpsilon(int r);
//void FillColor(int x, int y);

int main(int argc, char *argv[])
{
	glutInit(&argc, argv);
	InitWindow();

	glutPassiveMotionFunc(passiveMotionFunc); //capture free moment of the mouse
	glutMotionFunc(motionFunc);				  //capture hold state of the mouse
	glutMouseFunc(mouseFunc);				  //capture clicks
	glutKeyboardFunc(keyboard);				  //capture key pressed
	glutKeyboardUpFunc(keyboardUp);			  //capture released keys
	glutDisplayFunc(display);

	CreateMyMenu();

	glutMainLoop();
}

void InitWindow() //-----INIT_WINDOW-----
{
	//create window
	glutInitWindowSize(width, height);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("RADIAL");

	//initialise canvas
	glMatrixMode(GL_PROJECTION);
	gluOrtho2D(0, width, 0, height); //set the projection area
	//glClearColor(0, 1, 1, 1);		 //set color of the screen
	glClear(GL_COLOR_BUFFER_BIT);	//clear the buffer of the screen
	glFlush();						 //force update of the screen

	//set pencil color
	glColor3ub(255, 0, 0);
}

void display(void) //-----DISPLAY-----
{
	//glClear(GL_COLOR_BUFFER_BIT);			//clear the buffer of the screen
	glFlush(); //force update of the screen
}

void CreateMyMenu() //-----MENU-----
{

	int subMenuColor = glutCreateMenu(ColorSelectorMenu); //Color_Menu
	glutAddMenuEntry("RED", 1);
	glutAddMenuEntry("BLUE", 2);
	glutAddMenuEntry("BLACK", 3);
	glutAddMenuEntry("WHITE", 4);
	glutAddMenuEntry("YELLOW", 5);
	glutAddMenuEntry("GREEN", 6);
	glutAddMenuEntry("SPRING_GREEN", 7);
	glutAddMenuEntry("DEEP_PINK", 8);
	glutAddMenuEntry("LAWNGREEN", 9);
	glutAddMenuEntry("AQUAMARINE", 10);
	glutAddMenuEntry("DYNAMIC RANDOM",11);

	//Creating Main Menu
	glutCreateMenu(MainMenu); //Tool_selctor_tool
	glutAddSubMenu("COLORS", subMenuColor);
	glutAddMenuEntry("EXIT", 1);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

//=======================================CONTROLLER====================================

void keyboard(unsigned char key, int x, int y) //-----KEYBOARD-----
{
	switch (key)
	{
	case 'd':						  //clear the screen
		glClear(GL_COLOR_BUFFER_BIT); //clear the buffer of the screen
		glFlush();
		break;
	case 's': //set the color
		cout << "ENTER COLOR" << endl;
		double r, b, g;
		cin >> r >> b >> g;
		SetColor(r, b, g);
		break;

	case 'p': //toggle PencilTool on/off
		ToggleTools(isPenToolActive);
		break;
	case '+': //increase symmetry
		if (isKaleidoscope && symmetryLines < 360)
		{
			symmetryLines++;
			cout << "SYMMETRY: " << symmetryLines << endl;
		}
		break;
	case '-': //decrease symmetry
		if (isKaleidoscope && symmetryLines > 2)
		{
			symmetryLines--;
			cout << "SYMMETRY: " << symmetryLines << endl;
		}
		break;

	case '1':				 //multifunction
		if (isPenToolActive) //toggle kaleidoScope if pen is selected
		{
			isKaleidoscope = !isKaleidoscope;
			cout << "KALEIIDOSCOPE: " << isKaleidoscope << endl;
		}
		break;
	case '2':
		if (isKaleidoscope)
		{
			doubleSymmetry = !doubleSymmetry;
			cout << "DOUBLE SYMMETRY: " << doubleSymmetry << endl;
		}
	}
}

void mouseFunc(int button, int state, int x, int y) //-----mouseFunc-----
{
	if (state == GLUT_DOWN)
	{
		isMousePressed = true;
	}
	else if (state == GLUT_UP)
	{
		//cout << "MOUSE IS RELEASED" << endl;
		isMousePressed = false;
		if (isPenToolActive)
		{
			RecordPoints(0, 0); //to reset penTool so i can lift up my pen
		}
	}
	if (button == 3) //Scroll Up
	{
		ChangeColorUp();
	}
	else if (button == 4) //Scroll Down
	{
		ChangeColorDown();
	}
}

void motionFunc(int x, int y) //-----motionFunc-----
{
	if (isMousePressed && isPenToolActive) //only draw on mouse if pentool is active
	{
		RecordPoints(x, -y + height);
	}
}

void passiveMotionFunc(int x, int y) //-----passiveMotionFunc-----
{
	//cout << "*";
}

void keyboardUp(unsigned char key, int x, int y) //-----relasing of keyboard_inputs-----
{
	//cout << "key released" << key << endl;
}


//=============================CONTROLLER_END======================================

void RecordPoints(int x, int y) //-----RECORD_POINTS-----
{
	static int count = 0;
	static int x1, y1;

	if (!isMousePressed) //so  i can lift my pen up
	{
		count = 0;
		return;
	}

	if (count == 0) //record first point
	{
		x1 = x;
		y1 = y;
		count++;
	}
	else //record second point
	{
		if (isPenToolActive)
		{
			PenTool(x1, y1, x, y);
		}
		if (!isPenToolActive) //reset points
		{
			count = 0;
		}
		else //pen tool is active or recording is on
		{
			x1 = x;
			y1 = y;
		}
	}

	glFlush();
}

//===========================ANSILARRY_DRAWING==============================

void SetColor(GLubyte r, GLubyte b, GLubyte g) //SetColor
{
	glColor3ub(r, b, g);
}

void ToggleTools(bool &current) //ToggleTools
{
	system("clear");
	bool temp = current;
	isKaleidoscope = isPenToolActive = true;
	current = !temp;
	cout << "PENTOOL  : " << isPenToolActive << endl;
}

void RotatePoint(int &X_new, int &Y_new, int x, int y, double angle) //Rotate Point Around Centre
{
	double rad = (M_PI / (double)180) * angle;
	int Tx = width / 2;
	int Ty = height / 2;

	X_new = x * cos(rad) + y * sin(rad) - Tx * cos(rad) - Ty * sin(rad) + Tx;
	Y_new = -x * sin(rad) + y * cos(rad) + Tx * sin(rad) - Ty * cos(rad) + Ty;
}
//==============================ANISILARY_DRAWING_END===========================

//====================================TOOLS=====================================

void PenTool(int x1, int y1, int x2, int y2) //<+ PEN_TOOL +>
{
	DrawLine(x1, y1, x2, y2);
	//reafactor this
	if (isKaleidoscope) //4 symmetry
	{
		if (doubleSymmetry)
		{
			DrawLine(-x1 + width, y1, -x2 + width, y2);
		}
		int x3, y3, x4, y4;
		double angle = (double)360 / symmetryLines;
		double angleInc = angle;
		for (int i = 0; i < symmetryLines - 1; i++)
		{
			RotatePoint(x3, y3, x1, y1, angle);
			RotatePoint(x4, y4, x2, y2, angle);
			DrawLine(x3, y3, x4, y4);
			if (doubleSymmetry)
			{
				DrawLine(-x3 + width, y3, -x4 + width, y4);
			}
			angle += angleInc;
		}
	}
}

void ChangeColorUp() //<+- COLOR_CHANGER_TOOL -+>
{
	R += colorIncrementer;
	if (R > 240)
	{
		B += colorIncrementer;
		if (B > 240)
			G += colorIncrementer;
	}
	SetColor(R, B, G);
}
void ChangeColorDown() //<+- COLOR_CHANGER_TOOL -+>
{
	if (G < 10)
	{
		if (B < 10)
			R -= colorIncrementer;
		B -= colorIncrementer;
	}
	G -= colorIncrementer;
	SetColor(R, B, G);
}
//===============================TOOLS_END=======================================

//==================================MENU=========================================
void MainMenu(int choice) //-----Simple_MENU-----
{
	switch (choice)
	{
	case 1: //EXIT
		exit(0);
		break;
	}
}

void ColorSelectorMenu(int choice) //-----ColorSelector-----
{
	switch (choice)
	{
	case 1: //RED
		SetColor(255, 0, 0);
		isRandom=false;
		break;
	case 2: //BLUE
		SetColor(0, 0, 255);
		isRandom=false;
		break;
	case 3: //BLACK
		SetColor(0, 0, 0);
		isRandom=false;
		break;
	case 4: //WHITE
		SetColor(255, 255, 255);
		isRandom=false;
		break;
	case 5: //YELLOW
		SetColor(255, 255, 0);
		isRandom=false;
		break;
	case 6: //GREEN
		SetColor(0, 128, 0);
		isRandom=false;
		break;
	case 7: //SPRINGGREEN
		SetColor(0, 255, 127);
		isRandom=false;
		break;
	case 8: //DEEPPINK
		SetColor(255, 20, 147);
		isRandom=false;
		break;
	case 9: //LAWNGREEN
		SetColor(124, 252, 0);
		isRandom=false;
		break;
	case 10: //AQUAMARINE
		SetColor(127, 255, 212);
		isRandom=false;
		break;
	case 11:
			isRandom=true;
	}
}

//================================DRAWING_ALGORITHMS==============================

void DrawLine(int x1, int y1, int x2, int y2) //-----DDA-----
{
	srand(time(0));
	double dx = x2 - x1,
		   dy = y2 - y1;
	int step = max(abs(dx), abs(dy));
	double xinc = dx / step,
		   yinc = dy / step;
	double x = x1,
		   y = y1;
	while (step--)
	{
		x += xinc;
		y += yinc;
		if(isRandom)
			glColor3ub(rand()%255,rand()%255,rand()%255);;
		glPointSize(2.5);
		glBegin(GL_POINTS);
		glVertex2f(x, y);
		glEnd();
	}
	/*if(isRandom)
		{
			//cout<<"lol";
			glPointSize(3.0);
			glColor3ub( rand()%255, rand()%255, rand()%255 );;
		}
	glLineWidth(3.0);
	glBegin(GL_LINES);
	glVertex2i(x1,y1);
	glVertex2i(x2,y2);
	glEnd();*/
}
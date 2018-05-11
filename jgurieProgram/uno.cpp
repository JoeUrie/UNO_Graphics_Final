/*
Official Name: Joseph Urie

Nickname: Joe

Assignment: Final Project

Enviroment/Compiler: VS 2015

Date: May 4th, 2018

References: None

Interactions:
The goal of the game is to have zero cards in your hand.
You can only play one card a turn, and it must match either the color, or the value of the card on the table.
Wildcards can be played whenever, as long as it's your turn.
End your turn by playing a card. You must draw until you can play a card.
Press 'R' to reset the entire program. Make sure to reset before starting a new game.
Press 't' to see a close up view of the table.
The rest of the controls can be accessed through the sub-menus.

*/

#include <iostream>
#include <cmath>
#include <fstream>

#ifdef __APPLE__
#  include <GLUT/glut.h>
#  include <OpenGL/glext.h>
#else
#  include <GL/glut.h>
#endif

using namespace std;

// Globals.
static float xPos = 0.0, yPos = 7.0, zPos = 10.0; //Default Camera Position.
static int view = 1, tableView = 0; //Camera View Animation.
static int players = 0; //Amount of Players.
static float xCard = 0.5, yCard = 1.0, zCard = 0.75; //Default Card Position.
static int deckSize = 108, discardSize = 1; //Size of draw pile.
static int onePlayerGame = 0, twoPlayerGame = 0, threePlayerGame = 0, fourPlayerGame = 0; //How many players?
static int playerOneHand = 0, playerTwoHand = 0, playerThreeHand = 0, playerFourHand = 0; //Handsize for each player. First to reach 0 again wins!
static int lighting = 0; //Light on or off.
static int card = 0; //What texture is loaded onto the card.
static unsigned int texture[4]; //Array of texture indices.

//Struct of bitmap file.
struct BitMapFile
{
	int sizeX;
	int sizeY;
	unsigned char *data;
};

//Routine to read a bitmap file.
//Works only for uncompressed bmp files of 24-bit color.
BitMapFile *getBMPData(string filename)
{
	BitMapFile *bmp = new BitMapFile;
	unsigned int size, offset, headerSize;

	//Read input file name.
	ifstream infile(filename.c_str(), ios::binary);

	//Get the starting point of the image data.
	infile.seekg(10);
	infile.read((char *)&offset, 4);

	//Get the header size of the bitmap.
	infile.read((char *)&headerSize, 4);

	//Get width and height values in the bitmap header.
	infile.seekg(18);
	infile.read((char *)&bmp->sizeX, 4);
	infile.read((char *)&bmp->sizeY, 4);

	//Allocate buffer for the image.
	size = bmp->sizeX * bmp->sizeY * 24;
	bmp->data = new unsigned char[size];

	//Read bitmap data.
	infile.seekg(offset);
	infile.read((char *)bmp->data, size);

	//Reverse color from bgr to rgb.
	int temp;
	for (int i = 0; i < size; i += 3)
	{
		temp = bmp->data[i];
		bmp->data[i] = bmp->data[i + 2];
		bmp->data[i + 2] = temp;
	}

	return bmp;
}

//Load external textures.
void loadExternalTextures()
{
	//Local storage for bmp image data.
	BitMapFile *image[4];

	//Load the textures.
	image[0] = getBMPData("Textures/wildcard.bmp");
	image[1] = getBMPData("Textures/skip.bmp");
	image[2] = getBMPData("Textures/reverse.bmp");
	image[3] = getBMPData("Textures/uno.bmp");

	//Bind image to texture index[0]. 
	glBindTexture(GL_TEXTURE_2D, texture[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image[0]->sizeX, image[0]->sizeY, 0,
		GL_RGB, GL_UNSIGNED_BYTE, image[0]->data);

	//Bind image to texture index[1]
	glBindTexture(GL_TEXTURE_2D, texture[1]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image[1]->sizeX, image[1]->sizeY, 0,
		GL_RGB, GL_UNSIGNED_BYTE, image[1]->data);

	//Bind image to texture index[2]
	glBindTexture(GL_TEXTURE_2D, texture[2]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image[2]->sizeX, image[2]->sizeY, 0,
		GL_RGB, GL_UNSIGNED_BYTE, image[2]->data);

	//Bind image to texture index[3]. 
	glBindTexture(GL_TEXTURE_2D, texture[3]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image[3]->sizeX, image[3]->sizeY, 0,
		GL_RGB, GL_UNSIGNED_BYTE, image[3]->data);
}

//Timers for Animation
void myViewTimer(int)
{
	//Switching View
	if (view == 1) {
		if ((zPos < 10.0) && (xPos < 0.0)) { zPos += 0.5; xPos += 0.5; }
	}

	if (view == 2) {
		if ((xPos < 10.0) && (zPos > 0.0)) { xPos += 0.5; zPos -= 0.5; }
	}

	if (view == 3) {
		if ((zPos > -10.0) && (xPos > 0.0)) { zPos -= 0.5; xPos -= 0.5; }
	}

	if (view == 4) {
		if ((xPos > -10.0) && (zPos < 0.0)) { xPos -= 0.5; zPos += 0.5; }
	}

	glutPostRedisplay();
	glutTimerFunc(2000 / 60, myViewTimer, 0);
}

void myDealTimer(int)
{
	//Dealing Cards
	if (onePlayerGame == 1) {
		if (deckSize > 101) {
			--deckSize; ++playerOneHand;
		}
	}

	if (twoPlayerGame == 1) {
		if (deckSize > 94) {
			deckSize -= 2; ++playerOneHand; ++playerTwoHand;
		}
	}

	if (threePlayerGame == 1) {
		if (deckSize > 87) {
			deckSize -= 3; ++playerOneHand; ++playerTwoHand; ++playerThreeHand;
		}
	}

	if (fourPlayerGame == 1) {
		if (deckSize > 80) {
			deckSize -= 4; ++playerOneHand; ++playerTwoHand; ++playerThreeHand; ++playerFourHand;
		}
	}

	glutPostRedisplay();
	glutTimerFunc(10000 / 60, myDealTimer, 0);
}

//Drawing Methods
void drawTable()
{
	glPushMatrix();
	glColor3f(1.15, 0.35, 0.2);
	//Table Top
	glPushMatrix();
	glScalef(10.0, 1.0, 10.0);
	glutSolidCube(1.0);
	glPopMatrix();
	//Bottom Left Leg
	glPushMatrix();
	glTranslatef(-4.5, -5.0, 4.5);
	glScalef(1.0, 10.0, 1.0);
	glutSolidCube(1.0);
	glPopMatrix();
	//Bottom Right Leg
	glPushMatrix();
	glTranslatef(4.5, -5.0, 4.5);
	glScalef(1.0, 10.0, 1.0);
	glutSolidCube(1.0);
	glPopMatrix();
	//Top Left Leg
	glPushMatrix();
	glTranslatef(-4.5, -5.0, -4.5);
	glScalef(1.0, 10.0, 1.0);
	glutSolidCube(1.0);
	glPopMatrix();
	//Top Right Leg
	glPushMatrix();
	glTranslatef(4.5, -5.0, -4.5);
	glScalef(1.0, 10.0, 1.0);
	glutSolidCube(1.0);
	glPopMatrix();
	glPopMatrix();
}

void drawPerson()
{
	glPushMatrix();
	//Player 1
	glPushMatrix();
	glColor3f(0.0, 0.0, 0.0);
	glTranslatef(0.0, 7.0, 10.0);
	glutSolidSphere(1.0, 20, 20); //Head
	glTranslatef(0.0, -10.0, 0.0);
	glRotatef(-90.0, 1.0, 0.0, 0.0);
	glutSolidCone(2.0, 11.0, 10, 10); //Body
	glRotatef(90.0, 1.0, 0.0, 0.0);
	glRotatef(180.0, 0.0, 1.0, 0.0);
	glLineWidth(150.0);
	glBegin(GL_LINE_STRIP);
	glVertex3f(0.0, 6.0, 0.0);
	glVertex3f(2.0, 3.0, 2.5);
	glVertex3f(1.0, 4.5, 4.0);
	glEnd(); //Left Arm
	glPushMatrix();
	glTranslatef(1.0, 4.5, 4.0);
	glutSolidSphere(0.5, 20, 20); //Left Hand
	glPopMatrix();
	glBegin(GL_LINE_STRIP);
	glVertex3f(0.0, 6.0, 0.0);
	glVertex3f(-2.0, 3.0, 2.5);
	glVertex3f(-1.0, 4.5, 4.0);
	glEnd(); //Right Arm
	glPushMatrix();
	glTranslatef(-1.0, 4.5, 4.0);
	glutSolidSphere(0.5, 20, 20); //Right Hand
	glPopMatrix();
	glLineWidth(1.0);
	glPopMatrix();
	//Player 2
	glPushMatrix();
	glColor3f(1.0, 0.0, 0.0);
	glTranslatef(10.0, 7.0, 0.0);
	glutSolidSphere(1.0, 20, 20); //Head
	glTranslatef(0.0, -10.0, 0.0);
	glRotatef(-90.0, 1.0, 0.0, 0.0);
	glutSolidCone(2.0, 11.0, 10, 10); //Body
	glRotatef(90.0, 1.0, 0.0, 0.0);
	glRotatef(-90.0, 0.0, 1.0, 0.0);
	glLineWidth(150.0);
	glBegin(GL_LINE_STRIP);
	glVertex3f(0.0, 6.0, 0.0);
	glVertex3f(2.0, 3.0, 2.5);
	glVertex3f(1.0, 4.5, 4.0);
	glEnd(); //Left Arm
	glPushMatrix();
	glTranslatef(1.0, 4.5, 4.0);
	glutSolidSphere(0.5, 20, 20); //Left Hand
	glPopMatrix();
	glBegin(GL_LINE_STRIP);
	glVertex3f(0.0, 6.0, 0.0);
	glVertex3f(-2.0, 3.0, 2.5);
	glVertex3f(-1.0, 4.5, 4.0);
	glEnd(); //Right Arm
	glPushMatrix();
	glTranslatef(-1.0, 4.5, 4.0);
	glutSolidSphere(0.5, 20, 20); //Right Hand
	glPopMatrix();
	glLineWidth(1.0);
	glPopMatrix();
	//Player 3
	glPushMatrix();
	glColor3f(0.0, 1.0, 0.0); //Color
	glTranslatef(0.0, 7.0, -10.0);
	glutSolidSphere(1.0, 20, 20); //Head
	glTranslatef(0.0, -10.0, 0.0);
	glRotatef(-90.0, 1.0, 0.0, 0.0);
	glutSolidCone(2.0, 11.0, 10, 10); //Body
	glRotatef(90.0, 1.0, 0.0, 0.0);
	glLineWidth(150.0);
	glBegin(GL_LINE_STRIP);
	glVertex3f(0.0, 6.0, 0.0);
	glVertex3f(2.0, 3.0, 2.5);
	glVertex3f(1.0, 4.5, 4.0);
	glEnd(); //Left Arm
	glPushMatrix();
	glTranslatef(1.0, 4.5, 4.0);
	glutSolidSphere(0.5, 20, 20); //Left Hand
	glPopMatrix();
	glBegin(GL_LINE_STRIP);
	glVertex3f(0.0, 6.0, 0.0);
	glVertex3f(-2.0, 3.0, 2.5);
	glVertex3f(-1.0, 4.5, 4.0);
	glEnd(); //Right Arm
	glPushMatrix();
	glTranslatef(-1.0, 4.5, 4.0);
	glutSolidSphere(0.5, 20, 20); //Right Hand
	glPopMatrix();
	glLineWidth(1.0);
	glPopMatrix();
	//Player 4
	glPushMatrix();
	glColor3f(0.0, 0.0, 1.0);
	glTranslatef(-10.0, 7.0, 0.0);
	glutSolidSphere(1.0, 20, 20); //Head
	glTranslatef(0.0, -10.0, 0.0);
	glRotatef(-90.0, 1.0, 0.0, 0.0);
	glutSolidCone(2.0, 11.0, 10, 10); //Body
	glRotatef(90.0, 1.0, 0.0, 0.0);
	glRotatef(90.0, 0.0, 1.0, 0.0);
	glLineWidth(150.0);
	glBegin(GL_LINE_STRIP);
	glVertex3f(0.0, 6.0, 0.0);
	glVertex3f(2.0, 3.0, 2.5);
	glVertex3f(1.0, 4.5, 4.0);
	glEnd(); //Left Arm
	glPushMatrix();
	glTranslatef(1.0, 4.5, 4.0);
	glutSolidSphere(0.5, 20, 20); //Left Hand
	glPopMatrix();
	glBegin(GL_LINE_STRIP);
	glVertex3f(0.0, 6.0, 0.0);
	glVertex3f(-2.0, 3.0, 2.5);
	glVertex3f(-1.0, 4.5, 4.0);
	glEnd(); //Right Arm
	glPushMatrix();
	glTranslatef(-1.0, 4.5, 4.0);
	glutSolidSphere(0.5, 20, 20); //Right Hand
	glPopMatrix();
	glLineWidth(1.0);
	glPopMatrix();
	glPopMatrix();
}

void drawCard(float x, float y, float z, int id)
{
	glBindTexture(GL_TEXTURE_2D, texture[id]);
	glBegin(GL_POLYGON);
	glTexCoord2f(0.0, 0.0); glVertex3f(-x, y, z);
	glTexCoord2f(1.0, 0.0); glVertex3f(x, y, z);
	glTexCoord2f(1.0, 1.0); glVertex3f(x, y, -z);
	glTexCoord2f(0.0, 1.0); glVertex3f(-x, y, -z);
	glEnd();
	glBindTexture(GL_TEXTURE_2D, texture[3]);
	glBegin(GL_POLYGON);
	glTexCoord2f(1.0, 0.0); glVertex3f(-x, y - 0.01, z);
	glTexCoord2f(0.0, 0.0); glVertex3f(x, y - 0.01, z);
	glTexCoord2f(0.0, 1.0); glVertex3f(x, y - 0.01, -z);
	glTexCoord2f(1.0, 1.0); glVertex3f(-x, y - 0.01, -z);
	glEnd();
}

void drawHand1(int size)
{
	float handX = -1.5, handY = 2.0, handZ = 5.0;
	float balance = 0.0;
	if (size > 7) balance = (size % 7);
	handX -= (balance / 4);
	for (int i = 0; i < size; ++i) { //Creating the deck
		glPushMatrix();
		glTranslatef(handX, handY, handZ);
		glRotatef(60.0, 1.0, 0.0, 0.0);
		drawCard(xCard, yCard, zCard, card);
		handX += 0.5; handY += 0.002; 
		glPopMatrix();
	}
	handX = (-1.5 - (balance / 4)); handY = 2.0;
}

void drawHand2(int size)
{
	float handX = -1.5, handY = 2.0, handZ = 5.0;
	float balance = 0.0;
	if (size > 7) balance = (size % 7);
	handX -= (balance / 4);
	for (int i = 0; i < size; ++i) { //Creating the deck
		glPushMatrix();
		glRotatef(90.0, 0.0, 1.0, 0.0);
		glTranslatef(handX, handY, handZ);
		glRotatef(60.0, 1.0, 0.0, 0.0);
		drawCard(xCard, yCard, zCard, card);
		handX += 0.5; handY += 0.002;
		glPopMatrix();
	}
	handX = (-1.5 - (balance / 4)); handY = 2.0;
}

void drawHand3(int size)
{
	float handX = -1.5, handY = 2.0, handZ = 5.0;
	float balance = 0.0;
	if (size > 7) balance = (size % 7);
	handX -= (balance / 4);
	for (int i = 0; i < size; ++i) { //Creating the deck
		glPushMatrix();
		glRotatef(180.0, 0.0, 1.0, 0.0);
		glTranslatef(handX, handY, handZ);
		glRotatef(60.0, 1.0, 0.0, 0.0);
		drawCard(xCard, yCard, zCard, card);
		handX += 0.5; handY += 0.002;
		glPopMatrix();
	}
	handX = (-1.5 - (balance / 4)); handY = 2.0;
}

void drawHand4(int size)
{
	float handX = -1.5, handY = 2.0, handZ = 5.0;
	float balance = 0.0;
	if (size > 7) balance = (size % 7);
	handX -= (balance / 4);
	for (int i = 0; i < size; ++i) { //Creating the deck
		glPushMatrix();
		glRotatef(270.0, 0.0, 1.0, 0.0);
		glTranslatef(handX, handY, handZ);
		glRotatef(60.0, 1.0, 0.0, 0.0);
		drawCard(xCard, yCard, zCard, card);
		handX += 0.5; handY += 0.002;
		glPopMatrix();
	}
	handX = (-1.5 - (balance / 4)); handY = 2.0;
}

//Initialization routine.
void setup(void)
{
	//Background Color 
	glClearColor(1.0, 1.0, 1.0, 0.0);
	glEnable(GL_DEPTH_TEST);

	//Create texture index array.
	glGenTextures(2, texture);

	//Load external textures.
	loadExternalTextures();

	//Specify how texture values combine with current surface color values.
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	//Turn on OpenGL lighting.
	glEnable(GL_LIGHTING);

	//Enable color material mode:
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT);
}

//Drawing routine.
void drawScene()
{
	//Light Property Vectors
	float lightAmb[] = { 0.0, 0.0, 0.0, 1.0 };
	float lightDifAndSpec0[] = { 0.2, 0.2, 0.2, 0.0 };
	float lightPos0[] = { 0.0, 25.0, 0.0, 1.0 };
	float globAmb[] = { 0.6, 0.6, 0.6, 1.0 };

	//Light0 properties.
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmb);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDifAndSpec0);
	glLightfv(GL_LIGHT0, GL_SPECULAR, lightDifAndSpec0);

	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globAmb); //Global ambient light.
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, 1); //Enable local viewpoint

	//Turn lights on/off
	if (lighting == 1) glEnable(GL_LIGHT0); else glDisable(GL_LIGHT0);
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();

	//Viewing Position
	if (tableView == 1) gluLookAt(0.0, 10.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -1.0); else	gluLookAt(xPos, yPos, zPos, 0.0, asin(1.0), 0.0, 0.0, 1.0, 0.0);

	//Draw objects.
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);
	drawTable();
	drawPerson();
	glEnable(GL_TEXTURE_2D);

	//Draw Cards and Hands
	glPushMatrix();
	glTranslatef(-1.0, 0.0, 0.0);
	glPushMatrix();
	glTranslatef(0.0, 2.0, 0.0);
	glRotatef(180.0, 0.0, 0.0, 1.0);
	if (deckSize == 0) { deckSize = discardSize; discardSize = 1; }
	for (int i = 0; i < deckSize-1; ++i) { //Creating the deck
		drawCard(xCard, yCard, zCard, card);
		yCard-=0.02;
	}
	xCard = 0.5, yCard = 1.0, zCard = 0.75;
	glPopMatrix();
	glTranslatef(2.0, 0.0, 0.0);
	if (onePlayerGame == 1 || twoPlayerGame == 1 || threePlayerGame == 1 || fourPlayerGame == 1) {
		//Discard pile starts at 1 because of first card of game//
		for (int i = 0; i < discardSize; ++i) { //Discard Pile
			drawCard(xCard, yCard, zCard, card);
			yCard += 0.02;
		}
	}
	xCard = 0.5, yCard = 1.0, zCard = 0.75;
	glPopMatrix();

	glDisable(GL_LIGHTING);
	if (onePlayerGame == 1) drawHand1(playerOneHand);
	if (twoPlayerGame == 1) { drawHand1(playerOneHand); drawHand2(playerTwoHand); }
	if (threePlayerGame == 1) { drawHand1(playerOneHand); drawHand2(playerTwoHand); drawHand3(playerThreeHand); }
	if (fourPlayerGame == 1) { drawHand1(playerOneHand); drawHand2(playerTwoHand); drawHand3(playerThreeHand); drawHand4(playerFourHand); }
	glEnable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);

	glutSwapBuffers();
}

//OpenGL window reshape routine.
void resize(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, (float)w / (float)h, 1.0, 50.0);
	glMatrixMode(GL_MODELVIEW);
}

//Keyboard input processing routine.
void keyInput(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 27:
		exit(0);
		break;
	case '1': //Player 1 View
		view = 1;
		//xPos = 0.0, yPos = 7.0, zPos = 12.0; 
		glutPostRedisplay();
		break;
	case '2': //Player 2 View
		view = 2;
		//xPos = 12.0, yPos = 7.0, zPos = 0.0;
		glutPostRedisplay();
		break;
	case '3': //Player 3 View
		view = 3;
		//xPos = 0.0, yPos = 7.0, zPos = -12.0;
		glutPostRedisplay();
		break;
	case '4': //Player 4 View
		view = 4;
		//xPos = -12.0, yPos = 7.0, zPos = 0.0;
		glutPostRedisplay();
		break;
	case 'R': //Reset
		xPos = 0.0, yPos = 7.0, zPos = 10.0; //Default Camera Position
		view = 1, tableView = 0; //Camera View Animation
		players = 0; //Amount of Players
		xCard = 0.5, yCard = 1.0, zCard = 0.75; //Default Card Position
		deckSize = 108; //Size of draw pile
		onePlayerGame = 0, twoPlayerGame = 0, threePlayerGame = 0, fourPlayerGame = 0; //How many players?
		playerOneHand = 0, playerTwoHand = 0, playerThreeHand = 0, playerFourHand = 0; //Handsize for each player. First to reach 0 again wins!
		lighting = 0; //Day or Night
		glutPostRedisplay();
		break;
	case 't': //Table View
		if (tableView == 0) tableView = 1;
		else tableView = 0;
		glutPostRedisplay();
		break;
	case 'c':
		if (card < 2) ++card; 
		else card = 0; 
		glutPostRedisplay();
		break;
	default:
		break;
	}
}

//Routine to output interaction instructions to the C++ window.
void printInteraction(void)
{
	cout << "CIS 425 Final Project \"Uno\" by Joseph Urie" << endl;
	cout << "The goal of the game is to have zero cards in your hand." << endl
		<< "You can only play one card a turn, and it must match either the color, or the value of the card on the table." << endl
		<< "Wildcards can be played whenever, as long as it's your turn." << endl
		<< "End your turn by playing a card. You must draw until you can play a card.\n" << endl;
	cout << "Press 'R' to reset the entire program. Make sure to reset before starting a new game." << endl;
	cout << "Press 't' to see a close up view of the table.\n" << endl; 
	cout << "The rest of the controls can be accessed through the sub-menus." << endl;
}

//The top menu callback function.
void top_menu(int id)
{
	if (id == 1) exit(0);
	if (onePlayerGame == 1 || twoPlayerGame == 1 || threePlayerGame == 1 || fourPlayerGame == 1) {
		if (id == 2) { //Draw Card
			if (view == 1) { ++playerOneHand; --deckSize; }
			if (view == 2) { ++playerTwoHand; --deckSize; }
			if (view == 3) { ++playerThreeHand; --deckSize; }
			if (view == 4) { ++playerFourHand; --deckSize; }
		}
		if (id == 3) { //Play Card
			if (view == 1) { --playerOneHand; ++discardSize; }
			if (view == 2) { --playerTwoHand; ++discardSize; }
			if (view == 3) { --playerThreeHand; ++discardSize; }
			if (view == 4) { --playerFourHand; ++discardSize; }
			if (view < 4) ++view; else view = 1; //Ends Turn
		}
	}
}

//The sub-menu callback function.
void new_game_menu(int id)
{
	if (id == 1)
	{
		xPos = 0.0, yPos = 7.0, zPos = 10.0;
		view = 1; tableView = 0;
		players = 1; onePlayerGame = 1; //One Human Player
		twoPlayerGame = 0, threePlayerGame = 0, fourPlayerGame = 0;
		playerOneHand = 0, playerTwoHand = 0, playerThreeHand = 0, playerFourHand = 0;
	}
	if (id == 2)
	{
		xPos = 0.0, yPos = 7.0, zPos = 10.0;
		view = 1; tableView = 0;
		players = 2; twoPlayerGame = 1; //Two Human Players
		onePlayerGame = 0, threePlayerGame = 0, fourPlayerGame = 0;
		playerOneHand = 0, playerTwoHand = 0, playerThreeHand = 0, playerFourHand = 0;
	}
	if (id == 3)
	{
		xPos = 0.0, yPos = 7.0, zPos = 10.0;
		view = 1; tableView = 0;
		players = 3; threePlayerGame = 1; //Three Human Players
		onePlayerGame = 0, twoPlayerGame = 0, fourPlayerGame = 0;
		playerOneHand = 0, playerTwoHand = 0, playerThreeHand = 0, playerFourHand = 0;
	}
	if (id == 4)
	{
		xPos = 0.0, yPos = 7.0, zPos = 10.0;
		view = 1; tableView = 0;
		players = 4; fourPlayerGame = 1; //Four (All) Human Players
		onePlayerGame = 0, twoPlayerGame = 0, threePlayerGame = 0;
		playerOneHand = 0, playerTwoHand = 0, playerThreeHand = 0, playerFourHand = 0;
	}
	glutPostRedisplay();
}

void options_menu(int id)
{
	if (id == 1) //Light
	{
		if (lighting == 0) lighting = 1; else lighting = 0;
	}

	glutPostRedisplay();
}

//Routine to make the menu.
void makeMenu(void)
{
	int sub_menu;
	sub_menu = glutCreateMenu(new_game_menu);
	glutAddMenuEntry("1 Player", 1);
	glutAddMenuEntry("2 Players", 2);
	glutAddMenuEntry("3 Players", 3);
	glutAddMenuEntry("4 Players", 4);

	int sub_menu_3;
	sub_menu_3 = glutCreateMenu(options_menu);
	glutAddMenuEntry("Light[Off]", 1);
	glutAddMenuEntry("Light[On]", 1);

	glutCreateMenu(top_menu);
	glutAddSubMenu("New Game", sub_menu);
	glutAddMenuEntry("Draw Card", 2);
	glutAddMenuEntry("Play Card", 3);
	glutAddSubMenu("Options", sub_menu_3);
	glutAddMenuEntry("Quit", 1);

	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

//Main routine.
int main(int argc, char **argv)
{
	printInteraction();
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(10, 10);
	glutCreateWindow("Uno.cpp");
	setup();
	glutDisplayFunc(drawScene);
	glutReshapeFunc(resize);
	glutKeyboardFunc(keyInput);
	myViewTimer(1);
	myDealTimer(1);
	makeMenu();
	glutMainLoop();
	return 0;
}

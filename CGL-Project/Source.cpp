/*
*		This Code Was Created By Jeff Molofee 2000
*		A HUGE Thanks To Fredric Echols For Cleaning Up
*		And Optimizing This Code, Making It More Flexible!
*		If You've Found This Code Useful, Please Let Me Know.
*		Visit My Site At nehe.gamedev.net
*/

#include <windows.h>		// Header File For Windows
#include <gl\gl.h>			// Header File For The OpenGL32 Library
#include <gl\glu.h>			// Header File For The GLu32 Library
#include <GL\glut.h>
#include <irrKlang\irrKlang.h>
#include <stdio.h>			// Header File For Standard Input/Output
#include <stdarg.h>			// Header File For Variable Argument Routines
#include <string>
#include <ctime>
#include <cstdlib>
#include <iostream>
#include <queue>
#include <thread>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "Textures.h"
#include "Map.h"
#include "ObjectsListIndices.h"
#include "Snake.h"
#define PI acos(-1)
#define JUMP 2
#define TILESIZE 2
#define SKYHEIGHT 800
#define LENGTH 1600
#define FLOORSIZE 8

#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"glu32.lib")

unsigned int base;				// Base Display List For The Font Set
unsigned int Index;

double Theta = 0;
double CoinAngle = 0;
int X = 1, Y = 1, Z = 1;

Textures TexturesList = {};
int CameraPosition = 5;
int RotationCoefficient = 0;
int Score = 0;
int CurrentLevel = 2;
int RemainingLives = 4;

int SnakeSpeed = 1000;

Map WorldMap = NULL;
irrklang::ISoundEngine *se = irrklang::createIrrKlangDevice();
ObjectsListIndices Objects;

bool IsSoundEnabled = false;
bool IsMipmap = true;

bool IsRotating = false;
bool IsMoving = false;
bool GameStarted = false;
bool Fullscreen = false;
bool Debug = false;
bool CoinEaten = true;
bool GameOver = false;
bool GamePaused = false;
bool ShouldGenerateLevel = true;
bool ShouldGenerateRandomVector = true;
bool HasWonGame = false;
bool StartFromTheBeginning = true;

static std::vector<Point> ObstaclesRandomPoints;
Point CoinCoordinates;

Snake GameSnake;

void GLPrint(const char *str, double x, double y, double z) {					// Custom GL "Print" Routine
	int len = (int)strlen(str);
	glRasterPos3d(x, y, z);
	for (int i = 0; i < len; i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, (int)str[i]);
	}
}

void DrawFloor(int size = 1) {
	int step = 2;
	glBindTexture(GL_TEXTURE_2D, TexturesList.Grass);
	for (int i = 0; i < 2 * size; i += step)
	{
		for (int j = 0; j < 2 * size; j += step)
		{
			glPushMatrix();
			glTranslated(i, 0, j);
			glCallList(Objects.Tile);
			glPopMatrix();
		}
	}
}

unsigned int LoadTexture(const char imagename[], bool mipmap) {
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	// load and generate the texture
	int width, height, nrChannels;
	unsigned char *data = stbi_load(imagename, &width, &height, &nrChannels, 0);
	if (data)
	{
		if (mipmap) {
			gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, width, height, GL_RGB, GL_UNSIGNED_BYTE, data);
		}
		else {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		}
	}
	else
	{
		MessageBox(NULL, "Failed to load texture", "SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
	}
	stbi_image_free(data);

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	//GL_ADD, GL_MODULATE, GL_DECAL, GL_BLEND, GL_REPLACE.

	// set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);//GL_CLAMP
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	if (mipmap) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	}
	else {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}

	return textureID;
}

void DrawSkybox() {

	glPushMatrix();

	//	Bottom
	
	glBindTexture(GL_TEXTURE_2D, TexturesList.Water);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);		glVertex3d(-LENGTH, -SKYHEIGHT, LENGTH);
	glTexCoord2f(1, 0);		glVertex3d(-LENGTH, -SKYHEIGHT, -LENGTH);
	glTexCoord2f(1, 1);		glVertex3d(LENGTH, -SKYHEIGHT, -LENGTH);
	glTexCoord2f(0, 1);		glVertex3d(LENGTH, -SKYHEIGHT, LENGTH);
	glEnd();

	//	Top
	glBindTexture(GL_TEXTURE_2D, TexturesList.Sky);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0); glVertex3d(-LENGTH, SKYHEIGHT, LENGTH);
	glTexCoord2f(1, 0); glVertex3d(-LENGTH, SKYHEIGHT, -LENGTH);
	glTexCoord2f(1, 1); glVertex3d(LENGTH, SKYHEIGHT, -LENGTH);
	glTexCoord2f(0, 1); glVertex3d(LENGTH, SKYHEIGHT, LENGTH);
	glEnd();

	//	Left
	glBindTexture(GL_TEXTURE_2D, TexturesList.Sky);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 1); glVertex3d(-LENGTH, -LENGTH, LENGTH);
	glTexCoord2f(1, 1); glVertex3d(-LENGTH, -LENGTH, -LENGTH);
	glTexCoord2f(1, 0); glVertex3d(-LENGTH, LENGTH, -LENGTH);
	glTexCoord2f(0, 0); glVertex3d(-LENGTH, LENGTH, LENGTH);
	glEnd();

	//	Right
	glBindTexture(GL_TEXTURE_2D, TexturesList.Sky);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 1); glVertex3d(LENGTH, -LENGTH, LENGTH);
	glTexCoord2f(1, 1); glVertex3d(LENGTH, -LENGTH, -LENGTH);
	glTexCoord2f(1, 0); glVertex3d(LENGTH, LENGTH, -LENGTH);
	glTexCoord2f(0, 0); glVertex3d(LENGTH, LENGTH, LENGTH);
	glEnd();

	//	Front
	glBindTexture(GL_TEXTURE_2D, TexturesList.Sky);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 1); glVertex3d(-LENGTH, -LENGTH, LENGTH);
	glTexCoord2f(1, 1); glVertex3d(LENGTH, -LENGTH, LENGTH);
	glTexCoord2f(1, 0); glVertex3d(LENGTH, LENGTH, LENGTH);
	glTexCoord2f(0, 0); glVertex3d(-LENGTH, LENGTH, LENGTH);
	glEnd();

	//	Back
	glBindTexture(GL_TEXTURE_2D, TexturesList.Sky);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 1); glVertex3d(-LENGTH, -LENGTH, -LENGTH);
	glTexCoord2f(1, 1); glVertex3d(LENGTH, -LENGTH, -LENGTH);
	glTexCoord2f(1, 0); glVertex3d(LENGTH, LENGTH, -LENGTH);
	glTexCoord2f(0, 0); glVertex3d(-LENGTH, LENGTH, -LENGTH);
	glEnd();
	glPopMatrix();
}

void GenerateObstacles(int count) {
	Point P;
	glPushMatrix();

	for (int i = 0; i < count;) {
		P = ObstaclesRandomPoints[i];
		if (WorldMap(P.X, P.Z) == Map::EMPTY) {
			WorldMap(P.X, P.Z) = Map::OBSTACLE;
			glPushMatrix();
			glTranslated(P.X - 1, 0, P.Z - 1);
			glCallList(Objects.Obstacle);
			glPopMatrix();
			i++;
		}
		else {
			continue;
		}
	}	
	
	glPopMatrix();
}

void GenerateCoin() {
	if (CoinEaten && GameStarted) {
		bool flag = false;
		Point Temp;
		while (!flag) {
			Temp.X = rand() % (CurrentLevel * FLOORSIZE * 2);
			if (Temp.X % 2 == 0) {
				continue;
			}

			Temp.Y = 1;

			Temp.Z = rand() % (CurrentLevel * FLOORSIZE * 2);
			if (Temp.Z % 2 == 0) {
				continue;
			}

			if (WorldMap(Temp.X, Temp.Z) == Map::OBSTACLE) {
				continue;
			}

			flag = true;
		}
		CoinEaten = false;
		CoinCoordinates = Temp;
	}

	glPushMatrix();
	glTranslated(CoinCoordinates.X, 0.5, CoinCoordinates.Z);
	glRotated(CoinAngle, 0, 1, 0);
	glCallList(Objects.Coin);
	glPopMatrix();

	WorldMap(CoinCoordinates.X, CoinCoordinates.Z) = Map::COIN;
}

bool GoingToCollide(int x, int z) {
	if (x > CurrentLevel * FLOORSIZE * 2 - 1 || x < 0 ||
		z > CurrentLevel * FLOORSIZE * 2 - 1 || z < 0) {
		return true;
	}

	if (WorldMap(x, z) == Map::OBSTACLE) {
		return true;
	}

	return false;
}

bool GoingToCoin(int x, int z) {
	if (WorldMap(x, z) == Map::COIN) {
		return true;
	}

	return false;
}

void SetInside() {
	if (X >= 2 * CurrentLevel * FLOORSIZE - 1) {
		X = CurrentLevel * FLOORSIZE * 2 - 1;
	}

	if (Y >= 2 * CurrentLevel * FLOORSIZE - 1) {
		Y = CurrentLevel * FLOORSIZE * 2 - 1;
	}

	if (Z >= 2 * CurrentLevel * FLOORSIZE - 1) {
		Z = CurrentLevel * FLOORSIZE * 2 - 1;
	}

	if (X < 1) {
		X = 1;
	}

	if (Y < 1) {
		Y = 1;
	}

	if (Z < 1) {
		Z = 1;
	}
}

void UpdateSnake() {
	glPushMatrix();
	GLUquadric *head = gluNewQuadric();
	glTranslated(X, 0.25, Z);
	glBindTexture(GL_TEXTURE_2D, TexturesList.SnakeHead);
	gluQuadricTexture(head, true);
	gluSphere(head, 0.5, 100, 40);
	
	GLUquadric *body = gluNewQuadric();
	for (int i = 0; i < Score; i++) {
		glTranslated(-round(cos(Theta)), 0.25, -round(sin(Theta)));
		glBindTexture(GL_TEXTURE_2D, TexturesList.SnakeBody);
		gluQuadricTexture(body, true);
		gluSphere(body, 0.5, 100, 40);
	}
	glPopMatrix();
}

void Move(int useless) {
	if (GoingToCollide(X + JUMP * cos(Theta), Z + JUMP * sin(Theta)) && IsMoving) {
		se->play2D("Music/Solid.wav", false);
		GameOver = true;
		IsMoving = false;
		RemainingLives--;
	}
	else {
		if (IsMoving) {
			if (GoingToCoin(X, Z)) {
				Score++;
				WorldMap(X, Z) = Map::EMPTY;
				CoinEaten = true;
				GenerateCoin();
				se->play2D("Music/Bleep.mp3");
				WorldMap(CoinCoordinates.X, CoinCoordinates.Z) = Map::COIN;
				GameSnake.InsertBody(X, Z);
			}
			UpdateSnake();
			//WorldMap(X, Z) = Map::EMPTY;
			X += JUMP * cos(Theta);
			//Y += SPEED * sin(angle);
			Z += JUMP * sin(Theta);
			//WorldMap(X, Z) = Map::SNAKE;
		}
		glutTimerFunc(SnakeSpeed, Move, 0);
	}
	glutPostRedisplay();
}

void ChangeCameraAngle(int useless) {
	double value = RotationCoefficient * PI / 2;
	if (Theta < value) {
		if (Theta + 0.1 >= value) {
			Theta = RotationCoefficient * PI / 2;
			IsMoving = true;
			IsRotating = false;
		}
		else {
			IsMoving = false;
			IsRotating = true;
			Theta += 0.4;
			glutTimerFunc(10, ChangeCameraAngle, 0);
		}
	}
	else if (Theta > value) {
		if (Theta - 0.1 <= value) {
			Theta = RotationCoefficient * PI / 2;
			IsMoving = true;
			IsRotating = false;
		}
		else {
			IsMoving = false;
			IsRotating = true;
			Theta -= 0.4;
			glutTimerFunc(10, ChangeCameraAngle, 0);
		}
	}
	glutPostRedisplay();
}

void RotateCoin(int useless) {
	CoinAngle += 1;
	glutTimerFunc(100, RotateCoin, 0);
	glutPostRedisplay();
}

void GenerateRandomVector() {
	ObstaclesRandomPoints.clear();
	srand(time(0));
	int c = 0;
	while (c < pow(CurrentLevel * FLOORSIZE, 2)) {
		Point Temp;
		Temp.X = rand() % (CurrentLevel * FLOORSIZE + 1);
		if ((int)Temp.X % 2 == 0) {
			continue;
		}

		Temp.Y = 1;

		Temp.Z = rand() % (CurrentLevel * FLOORSIZE + 1);
		if ((int)Temp.Z % 2 == 0) {
			continue;
		}

		ObstaclesRandomPoints.push_back(Temp);
		c++;
	}
}

void GenerateLevel() {
	if (ShouldGenerateRandomVector) {
		GenerateRandomVector();
		Score = 0;
		ShouldGenerateRandomVector = false;
	}
	GameSnake = Snake(1, 1);
	WorldMap = Map(CurrentLevel * FLOORSIZE);
	WorldMap(1, 1) = Map::SNAKE;
	DrawFloor(CurrentLevel * FLOORSIZE);
	GenerateObstacles(CurrentLevel * 4);
}

void ReSizeGLScene(int width, int height)		// Resize And Initialize The GL Window
{
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//gluOrtho2D(-10, 10, -10, 10); //2D prespective (for WEEK 2)
	gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 4000.0f); //3D prespective (since WEEK 3)
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();									// Reset The Modelview Matrix
}

void Keyboard(unsigned char key, int x, int y) {
	if (Debug) {
		char buffer[128];
		sprintf(buffer, "Key '%c' was pressed", key);
		MessageBox(NULL, buffer, "Info", 0x00000000L);
	}

	if (key == VK_ESCAPE) {
		exit(EXIT_SUCCESS);
	}
	else if (key == 'q') {
		IsRotating = true;
		IsMoving = false;
		RotationCoefficient--;
		ChangeCameraAngle(0);
	}
	else if (key == 'e') {
		IsRotating = true;
		IsMoving = false;
		RotationCoefficient++;
		ChangeCameraAngle(0);
	}
	else if (key == 'w') {
		if (!GameStarted) {
			GameSnake = Snake(1, 1);
			WorldMap(1, 1) = Map::SNAKE;
			GameOver = false;
			GameStarted = true;
			IsRotating = false;
			IsMoving = true;
			CoinEaten = true;
			Move(0);
		}
	}
	else if (key == 'n') {
		CurrentLevel++;
		GenerateLevel();
		GameStarted = false;
		HasWonGame = false;
	}
	else if (key == 'i') {
		char buffer[1024];
		sprintf(buffer, "Current Coordinates:\nX: %f\nY: %f\nZ: %f\n\nLooking At:\nX: %f\nY: %f\nZ: %f\nTheta: %f", X, Y, Z, X + cos(Theta), Y, Z + sin(Theta), Theta);
		MessageBox(NULL, buffer, "Coordinates", 0x00000000L);
	}
	else if (key == '+') {
		CameraPosition++;
		CameraPosition = CameraPosition % 6;
	}
	else if (key == '-') {
		ShouldGenerateLevel = !ShouldGenerateLevel;
		ShouldGenerateRandomVector = true;
	}
	else if (key == 'p') {
		IsMoving = !IsMoving;
		GamePaused = !GamePaused;
	}
	else if (key == 'r' && GameOver && RemainingLives > 1) {
		ShouldGenerateRandomVector = true;
		GenerateLevel();
		GameStarted = false;
		GameOver = false;
		if (StartFromTheBeginning) {
			CurrentLevel = 1;
			StartFromTheBeginning = false;
		}
	}
}

void SpecialKey(int key, int x, int y) {
	if (key == GLUT_KEY_F1) {
		if (Fullscreen) {
			glutReshapeWindow(1024, 768);
		}
		else {
			glutFullScreen();
		}
		Fullscreen = !Fullscreen;
	}
	else if (key == GLUT_KEY_F2) {
		IsSoundEnabled = !IsSoundEnabled;
		if (!IsSoundEnabled) {
			se->stopAllSounds();
		}
		else {
			se->play2D("Music/Gameplay.mp3", true);
		}
	}
	else if (key == GLUT_KEY_F3) {
		MessageBox(NULL, WorldMap.ToString().c_str(), "World Map", 0x00000000L);
	}
	else if (key == GLUT_KEY_F4) {
		Debug = !Debug;
	}
	else if (key == GLUT_KEY_F11) {
		ObstaclesRandomPoints.clear();
		GenerateRandomVector();
		CurrentLevel++;
		GenerateLevel();
	}
	else if (key == GLUT_KEY_F12) {
		ObstaclesRandomPoints.clear();
		GenerateRandomVector();
		CurrentLevel--;
		GenerateLevel();
	}
}

void Animate(void) {
	glutPostRedisplay();
}

int InitGL(void)										// All Setup For OpenGL Goes Here
{
	if (IsSoundEnabled) {
		se->play2D("Music/Gameplay.mp3", true);
	}

	glEnable(GL_TEXTURE_2D);
	glShadeModel(GL_SMOOTH);							// Enable Smooth Shading
	glClearColor(0.0f, 0.0f, 0.0f, 0.5f);				// Black Background
	glClearDepth(1.0f);									// Depth Buffer Setup
	glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
	glDepthFunc(GL_LEQUAL);								// The Type Of Depth Testing To Do
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations

	TexturesList.Sky = LoadTexture("Images/Sky-Seamless.jpg", IsMipmap);
	TexturesList.Grass = LoadTexture("Images/Grass.jpg", IsMipmap);
	TexturesList.Wood = LoadTexture("Images/Wood.jpg", IsMipmap);
	TexturesList.Coin = LoadTexture("Images/Coin.jpg", IsMipmap);
	TexturesList.SnakeHead = LoadTexture("Images/SnakeHead.jpg", IsMipmap);
	TexturesList.SnakeBody = LoadTexture("Images/SnakeBody.png", IsMipmap);
	TexturesList.Water = LoadTexture("Images/Water.jpg", IsMipmap);


	int index = glGenLists(3);
	Objects.Obstacle = index;
	Objects.Coin = index + 1;
	Objects.Tile = index + 2;

	
#pragma region Obstacle

	glNewList(Objects.Obstacle, GL_COMPILE);
	glPushMatrix();

	//	Top
	glBindTexture(GL_TEXTURE_2D, TexturesList.Wood);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0); glVertex3d(0, TILESIZE, TILESIZE);
	glTexCoord2f(1, 0); glVertex3d(0, TILESIZE, 0);
	glTexCoord2f(1, 1); glVertex3d(TILESIZE, TILESIZE, 0);
	glTexCoord2f(0, 1); glVertex3d(TILESIZE, TILESIZE, TILESIZE);
	glEnd();

	//	Bottom
	glBindTexture(GL_TEXTURE_2D, TexturesList.Wood);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0); glVertex3d(0, 0, TILESIZE);
	glTexCoord2f(1, 0); glVertex3d(0, 0, 0);
	glTexCoord2f(1, 1); glVertex3d(TILESIZE, 0, 0);
	glTexCoord2f(0, 1); glVertex3d(TILESIZE, 0, TILESIZE);
	glEnd();

	//	Left
	glBindTexture(GL_TEXTURE_2D, TexturesList.Wood);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 1); glVertex3d(0, 0, TILESIZE);
	glTexCoord2f(1, 1); glVertex3d(0, 0, 0);
	glTexCoord2f(1, 0); glVertex3d(0, TILESIZE, 0);
	glTexCoord2f(0, 0); glVertex3d(0, TILESIZE, TILESIZE);
	glEnd();

	//	Right
	glBindTexture(GL_TEXTURE_2D, TexturesList.Wood);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 1); glVertex3d(TILESIZE, 0, TILESIZE);
	glTexCoord2f(1, 1); glVertex3d(TILESIZE, 0, 0);
	glTexCoord2f(1, 0); glVertex3d(TILESIZE, TILESIZE, 0);
	glTexCoord2f(0, 0); glVertex3d(TILESIZE, TILESIZE, TILESIZE);
	glEnd();

	//	Front
	glBindTexture(GL_TEXTURE_2D, TexturesList.Wood);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 1); glVertex3d(0, 0, TILESIZE);
	glTexCoord2f(1, 1); glVertex3d(TILESIZE, 0, TILESIZE);
	glTexCoord2f(1, 0); glVertex3d(TILESIZE, TILESIZE, TILESIZE);
	glTexCoord2f(0, 0); glVertex3d(0, TILESIZE, TILESIZE);
	glEnd();

	//	Back
	glBindTexture(GL_TEXTURE_2D, TexturesList.Wood);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 1); glVertex3d(0, 0, 0);
	glTexCoord2f(1, 1); glVertex3d(TILESIZE, 0, 0);
	glTexCoord2f(1, 0); glVertex3d(TILESIZE, TILESIZE, 0);
	glTexCoord2f(0, 0); glVertex3d(0, TILESIZE, 0);
	glEnd();
	glPopMatrix();
	glEndList();

#pragma endregion

#pragma region Coin
	glNewList(Objects.Coin, GL_COMPILE);
	glPushMatrix();
	glTranslated(0, 0, 0);
	glBindTexture(GL_TEXTURE_2D, TexturesList.Coin);
	GLUquadric *coin = gluNewQuadric();
	gluQuadricTexture(coin, true);
	gluDisk(coin, 0, 0.5, 100, 32);
	glPopMatrix();
	glEndList();
#pragma endregion

#pragma region Tile
	glNewList(Objects.Tile, GL_COMPILE);
	int step = 2;
	glPushMatrix();
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0); glVertex3i(0, 0, 0);
	glTexCoord2f(1, 0); glVertex3i(step, 0, 0);
	glTexCoord2f(1, 1); glVertex3i(step, 0, step);
	glTexCoord2f(0, 1); glVertex3i(0, 0, step);
	glEnd();
	glPopMatrix();
	glEndList();
#pragma endregion

	RotateCoin(0);
	GenerateRandomVector();

	return true;										// Initialization Went OK
}

void DrawGLScene(void)									// Here's Where We Do All The Drawing
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear Screen And Depth Buffer
	glLoadIdentity();									// Reset The Current Modelview Matrix

	if (GameOver) {
		glDisable(GL_TEXTURE_2D);
		glPushMatrix();
		GLPrint("GAME OVER. HIT 'R' TO TRY AGAIN", 0, 0, -1);
		glPopMatrix();
		glEnable(GL_TEXTURE_2D);
	}

	if (GamePaused) {
		glDisable(GL_TEXTURE_2D);
		glPushMatrix();
		GLPrint("PAUSED. PRESS 'P' TO RESUME", 0, 0, -1);
		glPopMatrix();
		glEnable(GL_TEXTURE_2D);
	}

	if (HasWonGame) {
		glDisable(GL_TEXTURE_2D);
		glPushMatrix();
		GLPrint("YOU WON. PRESS 'N' TO GO TO NEXT STAGE", 0, 0, -1);
		glPopMatrix();
		glEnable(GL_TEXTURE_2D);
		IsMoving = false;
		IsRotating = false;
	}

	if (RemainingLives == 0) {
		StartFromTheBeginning = true;
	}

	glDisable(GL_TEXTURE_2D);
	char buffer[1024];

	sprintf(buffer, "X: %d", X);
	GLPrint(buffer, -0.5, 0.38, -1);

	sprintf(buffer, "Z: %d", Z);
	GLPrint(buffer, -0.5, 0.35, -1);

	sprintf(buffer, "Theta: %lf", Theta);
	GLPrint(buffer, -0.5, 0.32, -1);

	sprintf(buffer, "Rotation Coefficient: %d", RotationCoefficient);
	GLPrint(buffer, -0.5, 0.29, -1);

	sprintf(buffer, "Is Rotating: %d", IsRotating);
	GLPrint(buffer, -0.5, 0.26, -1);

	sprintf(buffer, "Is Moving: %d", IsMoving);
	GLPrint(buffer, -0.5, 0.23, -1);

	sprintf(buffer, "Coin coordinates: X: %d, Z: %d", CoinCoordinates.X, CoinCoordinates.Z);
	GLPrint(buffer, -0.5, 0.20, -1);

	sprintf(buffer, "Score: %d", Score);
	GLPrint(buffer, -0.5, 0.17, -1);

	sprintf(buffer, "Remaining lives: %d", RemainingLives);
	GLPrint(buffer, -0.5, 0.14, -1);
	glEnable(GL_TEXTURE_2D);

	glPushMatrix();
	SetInside();

	// The following line is causing camera speed issues
	if (CameraPosition == 0) {
		gluLookAt(X, Y, Z, X + JUMP * cos(Theta), Y/* + sin(angle)*/, Z + JUMP * sin(Theta), 0, 1, 0);
	}
	else if (CameraPosition == 1) {
		gluLookAt(CurrentLevel * FLOORSIZE * 2, CurrentLevel * FLOORSIZE * 2, CurrentLevel * FLOORSIZE * 2, 0, 0, 0, 0, 1, 0);
	}
	else if (CameraPosition == 2) {
		gluLookAt(CurrentLevel * FLOORSIZE * 2, CurrentLevel * FLOORSIZE * 2, 0, 0, 0, CurrentLevel * FLOORSIZE * 2, 0, 1, 0);
	}
	else if (CameraPosition == 3) {
		gluLookAt(0, CurrentLevel * FLOORSIZE * 2, CurrentLevel * FLOORSIZE * 2, CurrentLevel * FLOORSIZE * 2, 0, 0, 0, 1, 0);
	}
	else if (CameraPosition == 4) {
		gluLookAt(0, CurrentLevel * FLOORSIZE * 2, 0, CurrentLevel * FLOORSIZE * 2, 0, CurrentLevel * FLOORSIZE * 2, 0, 1, 0);
	}
	else if (CameraPosition == 5) {
		gluLookAt(CurrentLevel * FLOORSIZE * 2, 50, CurrentLevel * FLOORSIZE * 2, CurrentLevel * FLOORSIZE / 2, 0, CurrentLevel * FLOORSIZE / 2, 0, 1, 0);
	}

	DrawSkybox();

	if (ShouldGenerateLevel) {
		GenerateLevel();
	}

	if (Score == CurrentLevel * 2) {
		HasWonGame = true;
	}

	if (RemainingLives == -1) {
		GameOver = true;
	}

	GenerateCoin();
	UpdateSnake(); 

	glPopMatrix();
	glutSwapBuffers();
}

int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE); // Use single display buffer.
	glutInitWindowSize(1024, 768);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("CGL Project");
	InitGL();
	glutKeyboardFunc(Keyboard);
	glutSpecialFunc(SpecialKey);
	glutReshapeFunc(ReSizeGLScene);
	glutDisplayFunc(DrawGLScene);
	glutIdleFunc(Animate);

	glutMainLoop();
	return 0;
}
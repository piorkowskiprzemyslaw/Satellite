/*
 * Projekt OpenGL.
 * autor : Przemysław Piórkowski
 */
#include <Windows.h>
#include <GL\glew.h>
#include <GL\freeglut.h>
#include <glm\glm.hpp>
#include <glm\gtx\transform.hpp>
#include <IL\il.h>
#include <IL\ilu.h>
#include <IL\ilut.h>
#include <iostream>
#include <cmath>
#include <time.h>
#include "ParticleEngine.h"

#define PI 3.14159265
#define TIMER_MS 25

char * earthLocation = "earth.jpg";
char * moonLocation = "moon.jpg";
char * cloudsLocation = "clouds.jpg";
char * modulLocation = "modul.jpg";
char * modul1Location = "modul1.jpg";
char * modul2Location = "modul2.jpg";
char * modul3Location = "modul3.jpg";
char * engineLocation = "engine.jpg";
char * satellite1Location = "satellite1.jpg";
char * satellite2Location = "satellite2.jpg";
char * satellite3Location = "satellite3.jpg";
char * satellite4Location = "satellite4.jpg";
char * satellite5Location = "satellite5.jpg";
char * solarpanelLocation = "solarpanel.jpg";
char * skymapLocation = "skymap.jpg";

GLsizei earthImageHeight, earthImageWidth;
GLsizei moonImageHeight, moonImageWidth;
GLsizei cloudsImageHeight, cloudsImageWidth;
GLsizei modulImageHeight, modulImageWidth;
GLsizei modul1ImageHeight, modul1ImageWidth;
GLsizei modul2ImageHeight, modul2ImageWidth;
GLsizei modul3ImageHeight, modul3ImageWidth;
GLsizei engineImageHeight, engineImageWidth;
GLsizei satellite1ImageHeight, satellite1ImageWidth;
GLsizei satellite2ImageHeight, satellite2ImageWidth;
GLsizei satellite3ImageHeight, satellite3ImageWidth;
GLsizei satellite4ImageHeight, satellite4ImageWidth;
GLsizei satellite5ImageHeight, satellite5ImageWidth;
GLsizei solarpanelImageHeight, solarpanelImageWidth;
GLsizei skymapImageHeight, skymapImageWidth;

static GLuint earth;
static GLuint moon;
static GLuint clouds;
static GLuint modul;
static GLuint modul1;
static GLuint modul2;
static GLuint modul3;
static GLuint engine;
static GLuint satellite1;
static GLuint satellite2;
static GLuint satellite3;
static GLuint satellite4;
static GLuint satellite5;
static GLuint solarpanel;
static GLuint skymap;

static ILuint ilEarth;
static ILuint ilMoon;
static ILuint ilClouds;
static ILuint ilModul;
static ILuint ilModul1;
static ILuint ilModul2;
static ILuint ilModul3;
static ILuint ilEngine;
static ILuint ilSatellite1;
static ILuint ilSatellite2;
static ILuint ilSatellite3;
static ILuint ilSatellite4;
static ILuint ilSatellite5;
static ILuint ilSolarpanel;
static ILuint ilSkymap;

ILubyte * earthData;
ILubyte * moonData;
ILubyte * cloudsData;
ILubyte * moduleData;
ILubyte * module1Data;
ILubyte * module2Data;
ILubyte * module3Data;
ILubyte * engineData;
ILubyte * satellite1Data;
ILubyte * satellite2Data;
ILubyte * satellite3Data;
ILubyte * satellite4Data;
ILubyte * satellite5Data;
ILubyte * solarpanelData;
ILubyte * skymapData;

GLUquadricObj* skymapQuadricObject = gluNewQuadric( );
GLUquadricObj* earthQuadricObject = gluNewQuadric( );
GLUquadricObj* moonQuadricObject = gluNewQuadric( );
GLUquadricObj* cloudsQuadricObject = gluNewQuadric( );
GLUquadricObj* qdo = gluNewQuadric( );
GLUquadricObj* moduleQuadricObject = gluNewQuadric( );
GLUquadricObj* satelliteQuadricObject = gluNewQuadric( );

static float fov = 45.0f;
static float frame_no = 0.0f;
static int animation = 0;
static int satelliteAnimation = 0;

static float horizontalAngle = PI;
static float verticalAngle = 0.0f;
static float speed = 0.05f;
static float mouseSpeed = 0.005f;

static glm::vec3 eye( 0.0f, 0.0f, 4.0f );
static glm::vec3 point( 0.0f, 0.0f, 0.0f );
static glm::vec3 up( 0.0f, 1.0f, 0.0f );
static glm::vec3 right( 1.0f, 0.0f, 0.0f );
static glm::vec3 direction( 0.0f, 0.0f, 1.0f );

static int windowWidht = 0;
static int windowHeight = 0;
static int lastX = 0;
static int lastY = 0;

static GLfloat light_position[] = { 10.0f, 0.0f, 5.0f, 1.0f };

ParticleEngine * pe;

clock_t lastTime;

/**
 * Handler dla openGL'a do obslugi klawiszy z klawiatury.
 */
void handleKeypress( unsigned char key, int x, int y )
{
	switch ( key ) 
	{
		case 'r': // reset pozycji kamery
				fov		= 45.0f;
				eye		= glm::vec3( 0.0f, 0.0f, 4.0f );
				point	= glm::vec3( 0.0f, 0.0f, 0.0f );
				up		= glm::vec3( 0.0f, 1.0f, 0.0f );
				right	= glm::vec3( 1.0f, 0.0f, 0.0f );
				horizontalAngle = PI;
				verticalAngle = 0.0f;
				windowHeight = glutGet( GLUT_WINDOW_HEIGHT );
				windowWidht = glutGet( GLUT_WINDOW_WIDTH );
				lastX = windowWidht / 2;
				lastY = windowHeight / 2;
				glutWarpPointer( lastX, lastY );
				break;
		case 27: // Wyjscie z programu
				exit( 0 );
	}
}

/**
 * Obsluga klawiszy specjalnych.
 */
void handleSpecialKeys( int key, int x, int y )
{
	switch ( key ) {
		case GLUT_KEY_DOWN:
			eye -= direction * speed;
			break;
		case GLUT_KEY_UP:
			eye += direction * speed;
			break;
		case GLUT_KEY_LEFT:
			eye -= right * speed;
			break;
		case GLUT_KEY_RIGHT:
			eye += right * speed;
			break;
		default:
			break;
	}

	point = eye + direction;
}

/**
 * Odczytanie z myszki jej polozenia. Funkcja musi byc tak zbudowana, poniewaz kazdorazowe wywolywanie
 * glutWrapPointer() powoduje nieskonczona petle - bug biblioteki glut... 
 * More info here : http://stackoverflow.com/questions/728049/glutpassivemotionfunc-and-glutwarpmousepointer
 */
void computeFromMouse( int x, int y )
{
	int deltaX = x - lastX;
	int deltaY = y - lastY;

	lastX = x;
	lastY = y;

	if ( deltaX == 0 && deltaY == 0 ) return;

	int windowX = glutGet( GLUT_WINDOW_X );
	int windowY = glutGet( GLUT_WINDOW_Y );

	int screenLeft = -windowX;
	int screenTop = -windowY;
	int screenRight = windowWidht - windowX;
	int screenBottom = windowHeight - windowY;

	if ( x <= screenLeft + 10 || ( y ) <= screenTop + 10 || x >= screenRight - 10 || y >= screenBottom - 10 ) {
		lastX = windowWidht / 2;
		lastY = windowHeight / 2;
		glutWarpPointer( lastX, lastY );
	}

	horizontalAngle	+= mouseSpeed * float( deltaX );
	verticalAngle	+= mouseSpeed * float( deltaY );
	direction = glm::vec3( cos( verticalAngle ) * sin( horizontalAngle ), sin( verticalAngle ), cos( verticalAngle ) * cos( horizontalAngle ) );
	point = eye + direction;
	right = glm::vec3( sin( horizontalAngle - 3.14f / 2.0f ), 0.0f, cos( horizontalAngle - 3.14f / 2.0f ) );
	up = glm::cross( right, direction );
}

/**
 * Inicjalizacja DevIL'a, pobieranie tekstur z plikow, etc, etc...
 */
void initDevIL( )
{
	ilInit( );
	iluInit( );
	ilutInit( );
	ilutRenderer( ILUT_OPENGL );

	/* Wczytanie textury ziemi z pliku. */
	ilGenImages( 1, &ilEarth );
	ilBindImage( ilEarth );
	ilutGLBindTexImage( );
	ilLoadImage( earthLocation );
	earthImageHeight = ilGetInteger( IL_IMAGE_HEIGHT );
	earthImageWidth = ilGetInteger( IL_IMAGE_WIDTH );
	earthData = ilGetData( );
	/* Wczytanie textury ksiezyca z pliku. */
	ilGenImages( 1, &ilMoon );
	ilBindImage( ilMoon );
	ilutGLBindTexImage( );
	ilLoadImage( moonLocation );
	moonImageHeight = ilGetInteger( IL_IMAGE_HEIGHT );
	moonImageWidth = ilGetInteger( IL_IMAGE_WIDTH );
	moonData = ilGetData( );
	/* Wczytanie textury chmur z pliku. */
	ilGenImages( 1, &ilClouds );
	ilBindImage( ilClouds );
	ilutGLBindTexImage( );
	ilLoadImage( cloudsLocation );
	cloudsImageHeight = ilGetInteger( IL_IMAGE_HEIGHT );
	cloudsImageWidth = ilGetInteger( IL_IMAGE_WIDTH );
	cloudsData = ilGetData( );
	/* Wczytanie tekstury modulu z pliku. */
	ilGenImages( 1, &ilModul );
	ilBindImage( ilModul );
	ilutGLBindTexImage( );
	ilLoadImage( modulLocation );
	modulImageHeight = ilGetInteger( IL_IMAGE_HEIGHT );
	modulImageWidth = ilGetInteger( IL_IMAGE_WIDTH );
	moduleData = ilGetData( );
	/* Wczytanie tekstury modulu1 z pliku. */
	ilGenImages( 1, &ilModul1 );
	ilBindImage( ilModul1 );
	ilutGLBindTexImage( );
	ilLoadImage( modul1Location );
	modul1ImageHeight = ilGetInteger( IL_IMAGE_HEIGHT );
	modul1ImageWidth = ilGetInteger( IL_IMAGE_WIDTH );
	module1Data = ilGetData( );
	/* Wczytanie tekstury modulu2 z pliku. */
	ilGenImages( 1, &ilModul2 );
	ilBindImage( ilModul2 );
	ilutGLBindTexImage( );
	ilLoadImage( modul2Location );
	modul2ImageHeight = ilGetInteger( IL_IMAGE_HEIGHT );
	modul2ImageWidth = ilGetInteger( IL_IMAGE_WIDTH );
	module2Data = ilGetData( );
	/* Wczytanie tekstury modulu3 z pliku. */
	ilGenImages( 1, &ilModul3 );
	ilBindImage( ilModul3 );
	ilutGLBindTexImage( );
	ilLoadImage( modul3Location );
	modul3ImageHeight = ilGetInteger( IL_IMAGE_HEIGHT );
	modul3ImageWidth = ilGetInteger( IL_IMAGE_WIDTH );
	module3Data = ilGetData( );
	/* Wczytanie tekstury silnika z pliku. */
	ilGenImages( 1, &ilEngine );
	ilBindImage( ilEngine );
	ilutGLBindTexImage( );
	ilLoadImage( engineLocation );
	engineImageHeight = ilGetInteger( IL_IMAGE_HEIGHT );
	engineImageWidth = ilGetInteger( IL_IMAGE_WIDTH );
	engineData = ilGetData( );
	/* Wczytanie tekstury satelity1 z pliku. */
	ilGenImages( 1, &ilSatellite1 );
	ilBindImage( ilSatellite1 );
	ilutGLBindTexImage( );
	ilLoadImage( satellite1Location );
	satellite1ImageHeight = ilGetInteger( IL_IMAGE_HEIGHT );
	satellite1ImageWidth = ilGetInteger( IL_IMAGE_WIDTH );
	satellite1Data = ilGetData( );
	/* Wczytanie tekstury satelity2 z pliku. */
	ilGenImages( 1, &ilSatellite2 );
	ilBindImage( ilSatellite2 );
	ilutGLBindTexImage( );
	ilLoadImage( satellite2Location );
	satellite2ImageHeight = ilGetInteger( IL_IMAGE_HEIGHT );
	satellite2ImageWidth = ilGetInteger( IL_IMAGE_WIDTH );
	satellite2Data = ilGetData( );
	/* Wczytanie tekstury satelity3 z pliku. */
	ilGenImages( 1, &ilSatellite3 );
	ilBindImage( ilSatellite3 );
	ilutGLBindTexImage( );
	ilLoadImage( satellite3Location );
	satellite3ImageHeight = ilGetInteger( IL_IMAGE_HEIGHT );
	satellite3ImageWidth = ilGetInteger( IL_IMAGE_WIDTH );
	satellite3Data = ilGetData( );
	/* Wczytanie tekstury satelity4 z pliku. */
	ilGenImages( 1, &ilSatellite4 );
	ilBindImage( ilSatellite4 );
	ilutGLBindTexImage( );
	ilLoadImage( satellite4Location );
	satellite4ImageHeight = ilGetInteger( IL_IMAGE_HEIGHT );
	satellite4ImageWidth = ilGetInteger( IL_IMAGE_WIDTH );
	satellite4Data = ilGetData( );
	/* Wczytanie tekstury satelity5 z pliku. */
	ilGenImages( 1, &ilSatellite5 );
	ilBindImage( ilSatellite5 );
	ilutGLBindTexImage( );
	ilLoadImage( satellite5Location );
	satellite5ImageHeight = ilGetInteger( IL_IMAGE_HEIGHT );
	satellite5ImageWidth = ilGetInteger( IL_IMAGE_WIDTH );
	satellite5Data = ilGetData( );
	/* Wczytanie tekstury solarpanel z pliku. */
	ilGenImages( 1, &ilSolarpanel );
	ilBindImage( ilSolarpanel );
	ilutGLBindTexImage( );
	ilLoadImage( solarpanelLocation );
	solarpanelImageHeight = ilGetInteger( IL_IMAGE_HEIGHT );
	solarpanelImageWidth = ilGetInteger( IL_IMAGE_WIDTH );
	solarpanelData = ilGetData( );
	/* Wczytanie tekstury skymap z pliku. */
	ilGenImages( 1, &ilSkymap );
	ilBindImage( ilSkymap );
	ilutGLBindTexImage( );
	ilLoadImage( skymapLocation );
	skymapImageHeight = ilGetInteger( IL_IMAGE_HEIGHT );
	skymapImageWidth = ilGetInteger( IL_IMAGE_WIDTH );
	skymapData = ilGetData( );
}

/**
 * Inicjalizacja OpenGL'a.
 */
void init( )
{
	//Okreslenie wlasciwosci materialow.
	GLfloat mat_ambient[] = { 0.3f, 0.3f, 0.3f, 1.0f };
	
	glMaterialfv( GL_FRONT, GL_AMBIENT, mat_ambient );
	
	//Okreslenie swiatel, otoczenia, glownego...
	GLfloat lm_ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f };

	glLightfv( GL_LIGHT0, GL_POSITION, light_position );
	glLightModelfv( GL_LIGHT_MODEL_AMBIENT, lm_ambient );
	glEnable( GL_LIGHTING );
	glEnable( GL_LIGHT0 );
	

	glEnable( GL_TEXTURE_2D );
	glShadeModel( GL_SMOOTH );
	glDepthFunc( GL_LESS );
	glEnable( GL_DEPTH_TEST );
	glEnable( GL_NORMALIZE );
	glEnable( GL_CULL_FACE );

	gluQuadricNormals( earthQuadricObject, GLU_SMOOTH );
	gluQuadricNormals( moonQuadricObject, GLU_SMOOTH );
	gluQuadricNormals( cloudsQuadricObject, GLU_SMOOTH );
	gluQuadricNormals( qdo, GLU_SMOOTH );
	gluQuadricNormals( moduleQuadricObject, GLU_SMOOTH );

	gluQuadricDrawStyle( earthQuadricObject, GLU_FILL );
	gluQuadricDrawStyle( moonQuadricObject, GLU_FILL );
	gluQuadricDrawStyle( cloudsQuadricObject, GLU_FILL );
	gluQuadricDrawStyle( qdo, GLU_FILL );
	gluQuadricDrawStyle( moduleQuadricObject, GLU_FILL );

	initDevIL( );

	glGenTextures( 1, &earth );
	ilBindImage( ilEarth );
	glBindTexture( GL_TEXTURE_2D, earth );
	gluQuadricTexture( earthQuadricObject, GL_TRUE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	ilutGLTexImage( 0 );

	glGenTextures( 1, &moon );
	ilBindImage( ilMoon );
	glBindTexture( GL_TEXTURE_2D, moon );
	gluQuadricTexture( moonQuadricObject, GL_TRUE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	ilutGLTexImage( 0 );

	glGenTextures( 1, &clouds );
	ilBindImage( ilClouds );
	glBindTexture( GL_TEXTURE_2D, clouds );
	gluQuadricTexture( cloudsQuadricObject, GL_TRUE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	ilutGLTexImage( 0 );

	glGenTextures( 1, &modul );
	ilBindImage( ilModul );
	glBindTexture( GL_TEXTURE_2D, modul );
	gluQuadricTexture( moduleQuadricObject, GL_TRUE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	ilutGLTexImage( 0 );

	glGenTextures( 1, &modul1 );
	ilBindImage( ilModul1 );
	glBindTexture( GL_TEXTURE_2D, modul1 );
	gluQuadricTexture( moduleQuadricObject, GL_TRUE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	ilutGLTexImage( 0 );

	glGenTextures( 1, &modul2 );
	ilBindImage( ilModul2 );
	glBindTexture( GL_TEXTURE_2D, modul2 );
	gluQuadricTexture( moduleQuadricObject, GL_TRUE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	ilutGLTexImage( 0 );

	glGenTextures( 1, &modul3 );
	ilBindImage( ilModul3 );
	glBindTexture( GL_TEXTURE_2D, modul3 );
	gluQuadricTexture( moduleQuadricObject, GL_TRUE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	ilutGLTexImage( 0 );

	glGenTextures( 1, &engine );
	ilBindImage( ilEngine );
	glBindTexture( GL_TEXTURE_2D, engine );
	gluQuadricTexture( moduleQuadricObject, GL_TRUE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	ilutGLTexImage( 0 );

	glGenTextures( 1, &satellite1 );
	ilBindImage( ilSatellite1 );
	glBindTexture( GL_TEXTURE_2D, satellite1 );
	gluQuadricTexture( satelliteQuadricObject, GL_TRUE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	ilutGLTexImage( 0 );

	glGenTextures( 1, &satellite2 );
	ilBindImage( ilSatellite2 );
	glBindTexture( GL_TEXTURE_2D, satellite2 );
	gluQuadricTexture( satelliteQuadricObject, GL_TRUE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	ilutGLTexImage( 0 );

	glGenTextures( 1, &satellite3 );
	ilBindImage( ilSatellite3 );
	glBindTexture( GL_TEXTURE_2D, satellite3 );
	gluQuadricTexture( satelliteQuadricObject, GL_TRUE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	ilutGLTexImage( 0 );

	glGenTextures( 1, &satellite4 );
	ilBindImage( ilSatellite4 );
	glBindTexture( GL_TEXTURE_2D, satellite4 );
	gluQuadricTexture( satelliteQuadricObject, GL_TRUE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	ilutGLTexImage( 0 );

	glGenTextures( 1, &satellite5 );
	ilBindImage( ilSatellite5 );
	glBindTexture( GL_TEXTURE_2D, satellite5 );
	gluQuadricTexture( satelliteQuadricObject, GL_TRUE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	ilutGLTexImage( 0 );

	glGenTextures( 1, &solarpanel );
	ilBindImage( ilSolarpanel );
	glBindTexture( GL_TEXTURE_2D, solarpanel );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	ilutGLTexImage( 0 );

	glGenTextures( 1, &skymap );
	ilBindImage( ilSkymap );
	glBindTexture( GL_TEXTURE_2D, skymap );
	gluQuadricTexture( skymapQuadricObject, GL_TRUE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	ilutGLTexImage( 0 );

	glutSetCursor( GLUT_CURSOR_NONE );

	lastTime = clock( );
}

/**
 * Wyswietlanie elementow otoczenia - planet.
 */
void displayEnviroment( float angle )
{
	GLfloat earth_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat moon_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat clouds_blend[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	GLfloat clouds_shinines[] = { 0.0f };
	GLfloat clouds_specular[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	GLfloat clouds_emission[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	GLfloat clouds_diffuse[] = { 0.25f, 0.25f, 0.25f, 1.0f };

	glPushMatrix( );

		glPushMatrix( );
		//Wyswietlenie otoczenia.
		glDisable( GL_CULL_FACE );
		glDepthMask( GL_FALSE );
		glDisable( GL_LIGHTING );

		glRotatef( 90.0f, 1.0f, 0.0f, 0.0f );
		glBindTexture( GL_TEXTURE_2D, skymap );
		gluSphere( skymapQuadricObject, 14.0f, 1000, 1000 );

		glEnable( GL_LIGHTING );
		glDepthMask( GL_TRUE );
		glEnable( GL_CULL_FACE );
		//wyswietlenia otoczenia.
		glPopMatrix( );

	glTranslatef( 0.0f, 0.0f, -6.0f );
	glRotatef( 2 *angle / 90.0f, 0.0f, 1.0f, 0.0f );
	glLightfv( GL_LIGHT0, GL_POSITION, light_position );

	// ZIEMIA
			glPushMatrix( );
	
				glRotatef( 90.0f, 1.0f, 0.0f, 0.0f );
				
				glPushMatrix( );
				glRotatef( -( angle / 720.0f + 90.0f ), 0.0f, 0.0f, 1.0f );
					glBindTexture( GL_TEXTURE_2D, earth );
					glMaterialfv( GL_FRONT, GL_DIFFUSE, earth_diffuse );
					gluSphere( earthQuadricObject, 4.5, 100, 100 );
				
				glPopMatrix( );
	// CHMURY
				glPushMatrix( );
					glEnable( GL_BLEND );
					glBlendFunc( GL_ONE, GL_ONE );
					glRotatef( angle / 90, 0.0f, 0.0f, 1.0f );
					glMaterialfv( GL_FRONT, GL_SHININESS, clouds_shinines);
					glMaterialfv( GL_FRONT, GL_SPECULAR, clouds_specular );
					glMaterialfv( GL_FRONT, GL_EMISSION, clouds_emission );
					glMaterialfv( GL_FRONT, GL_DIFFUSE, clouds_diffuse );
					glTexEnvfv( GL_TEXTURE_ENV, GL_BLEND, clouds_blend );
					glBindTexture( GL_TEXTURE_2D, clouds );
					gluSphere( cloudsQuadricObject, 4.51, 1000, 1000 );
					glDisable( GL_BLEND );
				glPopMatrix( );
				
	// KSIEZYC
				glPushMatrix( );
					glRotatef( -angle / 360.0f - 45.0f, 0.0f, 0.0f, 1.0f );
					glTranslatef( 40.0f, 0.0f, 0.0f );
					glRotatef( angle / 720 - 90, 0.0f, 0.0f, 1.0f );
					glBindTexture( GL_TEXTURE_2D, moon );
					glMaterialfv( GL_FRONT, GL_DIFFUSE, moon_diffuse );
					gluSphere( moonQuadricObject, 1.23, 100, 100 );
				glPopMatrix( );
			glPopMatrix( );

	glPopMatrix( );
	glFlush( );
}

/**
 * Tutaj wyswietlanie rakiety, etc...
 */
void displayRocket( float frame_no )
{
	GLfloat diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLdouble plane_eq0[] = { 0.0f, 0.0f, -1.0f, 0.0f };
	GLdouble plane_eq1[] = { 1.0f, 0.0f, 0.0f, 0.0f };
	GLdouble plane_eq2[] = { -1.0f, 0.0f, 0.0f, 0.f };

	glDisable( GL_LIGHTING );
	glDisable( GL_CULL_FACE );

	glPushMatrix( );

	glScalef( 0.2f, 0.2f, 0.2f );
	glRotatef( -90.0f, 0.0f, 1.0f, 0.0f );
	//glTranslatef( 0.0f, 0.0f, 7.05f );

		// caly model rakiery
		glPushMatrix( );

				glPushMatrix( );
				// modul napedowy

					//oderwanie modulu napedowego.
					if ( frame_no >= 360.0 ) {
						glTranslatef( -6.0f * ( 1 / 0.2f), 0.0f, 0.0f );
						glRotatef( 2 * frame_no / 360.0f, 0.0f, 1.0f, 0.0f );
						glTranslatef( 6.0f * ( 1 / 0.2f ), 0.0f, 0.0f );
					}

					glTranslatef( 0.0f, 0.0f, -7.05f );

					glPushMatrix( );
					glColor3f( ( float ) ( 98.0 / 255.0 ), ( float ) ( 98.0 / 255.0 ), ( float ) ( 98.0 / 255.0 ) );
						gluDisk( qdo, 0.0f, 0.5f, 30, 20 );
						glColor3f( 1.0f, 1.0f, 1.0f );
					glPopMatrix( );

					glPushMatrix( );
						glTranslatef( 0.0f, 0.0f, 2.0f );
						glColor3f( ( float ) ( 98.0 / 255.0 ), ( float ) ( 98.0 / 255.0 ), ( float ) ( 98.0 / 255.0 ) );
						gluDisk( qdo, 0.0f, 0.5f, 30, 20 );
						glColor3f( 1.0f, 1.0f, 1.0f );
					glPopMatrix( );


					glPushMatrix( );
						glBindTexture( GL_TEXTURE_2D, modul2 );
						gluCylinder( moduleQuadricObject, 0.5f, 0.5f, 2.0f, 30, 30 );
					glPopMatrix( );

					//silnik gora
					glPushMatrix( );
						glTranslatef( 0.0f, 0.65f, 0.0f );
						glColor3f( ( float ) ( 98.0 / 255.0 ), ( float ) ( 98.0 / 255.0 ), ( float ) ( 98.0 / 255.0 ) );
						gluDisk( qdo, 0.0f, 0.15f, 30, 10 );
						glColor3f( 1.0f, 1.0f, 1.0f );
						glBindTexture( GL_TEXTURE_2D, engine );
						gluCylinder( moduleQuadricObject, 0.15f, 0.15f, 1.5f, 30, 30 );
						glPushMatrix( );
							glTranslatef( 0.0f, 0.0f, 1.5f );
							gluDisk( qdo, 0.0f, 0.15f, 30, 10 );
						glPopMatrix( );
					glPopMatrix( );

					// silnik prawo gora
					glPushMatrix( );
						glTranslatef( 0.468f, 0.468f, 0.0f );
						glColor3f( ( float ) ( 98.0 / 255.0 ), ( float ) ( 98.0 / 255.0 ), ( float ) ( 98.0 / 255.0 ) );
						gluDisk( qdo, 0.0f, 0.15f, 30, 10 );
						glColor3f( 1.0f, 1.0f, 1.0f );
						glBindTexture( GL_TEXTURE_2D, engine );
						gluCylinder( moduleQuadricObject, 0.15f, 0.15f, 1.5f, 30, 30 );
						glPushMatrix( );
							glTranslatef( 0.0f, 0.0f, 1.5f );
							gluDisk( qdo, 0.0f, 0.15f, 30, 10 );
						glPopMatrix( );
					glPopMatrix( );

					// silnik lewo gora
					glPushMatrix( );
						glTranslatef( -0.468f, 0.468f, 0.0f );
						glColor3f( ( float ) ( 98.0 / 255.0 ), ( float ) ( 98.0 / 255.0 ), ( float ) ( 98.0 / 255.0 ) );
						gluDisk( qdo, 0.0f, 0.15f, 30, 10 );
						glColor3f( 1.0f, 1.0f, 1.0f );
						glBindTexture( GL_TEXTURE_2D, engine );
						gluCylinder( moduleQuadricObject, 0.15f, 0.15f, 1.5f, 30, 30 );
						glPushMatrix( );
							glTranslatef( 0.0f, 0.0f, 1.5f );
							gluDisk( qdo, 0.0f, 0.15f, 30, 10 );
						glPopMatrix( );
					glPopMatrix( );

					//silnik dol
					glPushMatrix( );
						glTranslatef( 0.0f, -0.65f, 0.0f );
						glColor3f( ( float ) ( 98.0 / 255.0 ), ( float ) ( 98.0 / 255.0 ), ( float ) ( 98.0 / 255.0 ) );
						gluDisk( qdo, 0.0f, 0.15f, 30, 10 );
						glColor3f( 1.0f, 1.0f, 1.0f );
						glBindTexture( GL_TEXTURE_2D, engine );
						gluCylinder( moduleQuadricObject, 0.15f, 0.15f, 1.5f, 30, 30 );
						glPushMatrix( );
							glTranslatef( 0.0f, 0.0f, 1.5f );
							gluDisk( qdo, 0.0f, 0.15f, 30, 10 );
						glPopMatrix( );
					glPopMatrix( );

					// silnik prawo dol
					glPushMatrix( );
						glTranslatef( 0.468f, -0.468f, 0.0f );
						glColor3f( ( float ) ( 98.0 / 255.0 ), ( float ) ( 98.0 / 255.0 ), ( float ) ( 98.0 / 255.0 ) );
						gluDisk( qdo, 0.0f, 0.15f, 30, 10 );
						glColor3f( 1.0f, 1.0f, 1.0f );
						glBindTexture( GL_TEXTURE_2D, engine );
						gluCylinder( moduleQuadricObject, 0.15f, 0.15f, 1.5f, 30, 30 );
						glPushMatrix( );
							glTranslatef( 0.0f, 0.0f, 1.5f );
							gluDisk( qdo, 0.0f, 0.15f, 30, 10 );
						glPopMatrix( );
					glPopMatrix( );

					// silnik lewo dol
					glPushMatrix( );
						glTranslatef( -0.468f, -0.468f, 0.0f );
						glColor3f( ( float ) ( 98.0 / 255.0 ), ( float ) ( 98.0 / 255.0 ), ( float ) ( 98.0 / 255.0 ) );
						gluDisk( qdo, 0.0f, 0.15f, 30, 10 );
						glColor3f( 1.0f, 1.0f, 1.0f );
						glBindTexture( GL_TEXTURE_2D, engine );
						gluCylinder( moduleQuadricObject, 0.15f, 0.15f, 1.5f, 30, 30 );
						glPushMatrix( );
							glTranslatef( 0.0f, 0.0f, 1.5f );
							gluDisk( qdo, 0.0f, 0.15f, 30, 10 );
						glPopMatrix( );
					glPopMatrix( );

					// silnik prawo
					glPushMatrix( );
						glTranslatef( 0.65f, 0.0f, 0.0f );
						glColor3f( ( float ) ( 98.0 / 255.0 ), ( float ) ( 98.0 / 255.0 ), ( float ) ( 98.0 / 255.0 ) );
						gluDisk( qdo, 0.0f, 0.15f, 30, 10 );
						glColor3f( 1.0f, 1.0f, 1.0f );
						glBindTexture( GL_TEXTURE_2D, engine );
						gluCylinder( moduleQuadricObject, 0.15f, 0.15f, 1.5f, 30, 30 );
						glPushMatrix( );
							glTranslatef( 0.0f, 0.0f, 1.5f );
							gluDisk( qdo, 0.0f, 0.15f, 30, 10 );
						glPopMatrix( );
					glPopMatrix( );

					//silnik lewo
					glPushMatrix( );
						glTranslatef( -0.65f, 0.0f, 0.0f );
						glColor3f( ( float ) ( 98.0 / 255.0 ), ( float ) ( 98.0 / 255.0 ), ( float ) ( 98.0 / 255.0 ) );
						gluDisk( qdo, 0.0f, 0.15f, 30, 10 );
						glColor3f( 1.0f, 1.0f, 1.0f );
						glBindTexture( GL_TEXTURE_2D, engine );
						gluCylinder( moduleQuadricObject, 0.15f, 0.15f, 1.5f, 30, 30 );
						glPushMatrix( );
							glTranslatef( 0.0f, 0.0f, 1.5f );
							gluDisk( qdo, 0.0f, 0.15f, 30, 10 );
						glPopMatrix( );
					glPopMatrix( );
				
				// modul silnikowy.
				glPopMatrix( );


				glPushMatrix( );
				// modul satelity + czubek.

				// oddalenie sie modulu
				if ( frame_no > 1500 ) {
					if ( animation <= 190 ) {
						glTranslatef( -( animation - 90.0f ) / 100.0f, 0.0f, 0.0f );
						glTranslatef( 0.0f, 0.0f, -4.65f );
						glRotatef( -( animation - 90 ) / 7.5f, 0.0f, 1.0f, 0.0f );
						glTranslatef( 0.0f, 0.0f, 4.65f );
						++animation;
					} else {
						glTranslatef( -1.0f, 0.0f, 0.0f );
						glTranslatef( 0.0f, 0.0f, -4.65f );
						glRotatef( -( 100.0f ) / 7.5f, 0.0f, 1.0f, 0.0f );
						glTranslatef( 0.0f, 0.0f, 4.65f );
					}

					glTranslatef( -5.0f * ( 1 / 0.2f ), 0.0f, 0.0f );
					glRotatef( 2 * (frame_no - 1500) / 360.0f, 0.0f, 1.0f, 0.0f );
					glTranslatef( 5.0f * ( 1 / 0.2f ), 0.0f, 0.0f );

				}

					glPushMatrix( );
					// modul z satelita.

						glPushMatrix( );
						// lacznik. bialy.
							glTranslatef( 0.0f, 0.0f, -4.65f );						
							glPushMatrix( );
								// zatyczka w wezszym koncu
								glColor4f( ( float ) ( 98.0 / 255.0 ), ( float ) ( 98.0 / 255.0 ), ( float ) ( 98.0 / 255.0 ), 1.0f );
								gluDisk( qdo, 0.0f, 0.4f, 30, 20 );
								glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );

								glClipPlane( GL_CLIP_PLANE0, plane_eq0 );
								glEnable( GL_CLIP_PLANE0 );
								glTranslatef( 0.0f, 0.0f, -0.4f );
								glBindTexture( GL_TEXTURE_2D, modul1 );
								gluCylinder( moduleQuadricObject, 0.5f, 0.0f, 2.0f, 30, 20 );
								glDisable( GL_CLIP_PLANE0 );

								//zatyczka w szerszym koncu.
								glColor4f( ( float ) ( 98.0 / 255.0 ), ( float ) ( 98.0 / 255.0 ), ( float ) ( 98.0 / 255.0 ), 1.0f );
								gluDisk( qdo, 0.0f, 0.5f, 30, 20 );
								glColor4f( 1.0f, 1.0f, 1.0f , 1.0f );

								if ( frame_no > 360 && frame_no < 1500 ) {
									glPushMatrix( );
										glEnable( GL_BLEND );
										//glDisable( GL_LIGHTING );
										glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
										pe->draw( );
										//glEnable( GL_LIGHTING );
										glDisable( GL_BLEND );
									glPopMatrix( );
								}

							glPopMatrix( );
						// lacznik. bialy.
						glPopMatrix( );

						glPushMatrix( );
						// modul z satelita lewa czesc.
						
						
						// otwieranie pokrywy z satelita.
						if ( frame_no > 1000 ) {
							if ( animation <= 91 ) {
								glTranslatef( 0.0f, -0.4f, 0.0f );
								glRotatef( -( float ) animation, 0.0f, 0.0f, 1.0f );
								glTranslatef( 0.0f, 0.4f, 0.0f );
								++animation;
							} else {
								glTranslatef( 0.0f, -0.4f, 0.0f );
								glRotatef( -90.0f, 0.0f, 0.0f, 1.0f );
								glTranslatef( 0.0f, 0.4f, 0.0f );
							}
						
						}

							glClipPlane( GL_CLIP_PLANE1, plane_eq1 );
							glEnable( GL_CLIP_PLANE1 );
							glTranslatef( 0.0f, 0.0f, -4.65f );
							glBindTexture( GL_TEXTURE_2D, modul );
							gluCylinder( moduleQuadricObject, 0.4f, 0.4f, 3.0f, 30, 30 );
							glColor4f( ( float ) ( 30.0 / 255.0 ), ( float ) ( 30.0 / 255.0 ), ( float ) ( 30.0 / 255.0 ), 1.0f );
							gluCylinder( moduleQuadricObject, 0.39f, 0.39f, 3.0f, 30, 30 );
							glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );
							glDisable( GL_CLIP_PLANE1 );
						// modul z satelita lewa czesc.
						glPopMatrix( );

						glPushMatrix( );
						// modul z satelita prawa czesc.
							glClipPlane( GL_CLIP_PLANE2, plane_eq2 );
							glEnable( GL_CLIP_PLANE2 );
							glTranslatef( 0.0f, 0.0f, -4.65f );
							glBindTexture( GL_TEXTURE_2D, modul );
							gluCylinder( moduleQuadricObject, 0.4f, 0.4f, 3.0f, 30, 30 );
							glColor4f( ( float ) ( 30.0 / 255.0 ), ( float ) ( 30.0 / 255.0 ), ( float ) ( 30.0 / 255.0 ), 1.0f );
							gluCylinder( moduleQuadricObject, 0.39f, 0.39f, 3.0f, 30, 30 );
							glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );
							glDisable( GL_CLIP_PLANE2 );
						// modul z satelita prawa czesc.
						glPopMatrix( );

					//modul z satelita.
					glPopMatrix( );

					glPushMatrix( );
					// modul czubka.
						glTranslatef( 0.0f, 0.0f, -1.4f );
						glPushMatrix( );
							glClipPlane( GL_CLIP_PLANE0, plane_eq0 );
							glEnable( GL_CLIP_PLANE0 );
							glTranslatef( 0.0f, 0.0f, -0.25f );
							glBindTexture( GL_TEXTURE_2D, modul1 );
							gluCylinder( moduleQuadricObject, 0.4f, 0.0f, 0.75f, 30, 20 );
							glDisable( GL_CLIP_PLANE0 );
							glColor4f( ( float ) ( 98.0 / 255.0 ), ( float ) ( 98.0 / 255.0 ), ( float ) ( 98.0 / 255.0 ), 1.0f );
							gluDisk( qdo, 0.0f, 0.4f, 30, 20 );
							glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );
						glPopMatrix( );
					// modul czubka
					glPopMatrix( );

					glPushMatrix( );
					// modul czubka.
						glTranslatef( 0.0f, 0.0f, -1.4f );
						glBindTexture( GL_TEXTURE_2D, modul2 );
						gluCylinder( moduleQuadricObject, 0.265f, 0.265f, 0.5f, 30, 10 );
					// modul czubka.
					glPopMatrix( );

					glPushMatrix( );
					// modul czubka.
						glTranslatef( 0.0f, 0.0f, -0.7f );
						glPushMatrix( );
							glClipPlane( GL_CLIP_PLANE0, plane_eq0 );
							glEnable( GL_CLIP_PLANE0 );
							glTranslatef( 0.0f, 0.0f, -0.2f );
							glBindTexture( GL_TEXTURE_2D, modul1 );
							gluCylinder( moduleQuadricObject, 0.265f, 0.0f, 0.3f, 30, 10 );
							glDisable( GL_CLIP_PLANE0 );
						glPopMatrix( );
					// modul czubka.
					glPopMatrix( );

					glPushMatrix( );
					// czubek .
						glTranslatef( 0.0f, 0.0f, -0.7f );
						glBindTexture( GL_TEXTURE_2D, modul2 );
						gluCylinder( moduleQuadricObject, 0.088f, 0.088f, 0.3f, 30, 10 );
					// czubek .
					glPopMatrix( );

					glPushMatrix( );
					// ostrze czubka.
						glTranslatef( 0.0f, 0.0f, -0.4f );
						glBindTexture( GL_TEXTURE_2D, modul3 );
						gluCylinder( moduleQuadricObject, 0.088f, 0.0f, 0.4f, 30, 10 );
					// ostrze czubka.
					glPopMatrix( );

				// modul satelity + czubek.
				glPopMatrix( );

		// caly model rakiety.
		glPopMatrix( );


	glPopMatrix( );

	glEnable( GL_CULL_FACE );
	glEnable( GL_LIGHTING );
	glFlush( );
}

/**
 * Wyswietlanie satelity.
 */
void displaySatellite( float frame_no )
{
	GLdouble plane_eq0[] = { 0.0, 0.0, -1.0, 0.0 };
	GLdouble plane_eq1[] = { 0.0, 0.0, 1.0, 0.0 };

	glDisable( GL_CULL_FACE );
	glDisable( GL_LIGHTING );

	glPushMatrix( );
	//satelita

	/* diagnostyczny punkt w 0,0,0*/
	//glPointSize( 5.0f );
	//glBegin( GL_POINTS );
	//glVertex2f( 0.0f, 0.0f );
	//glEnd( );


	// pomniejszenie satelity
	glScalef( 0.2f, 0.2f, 0.2f );
	//umieszczenie satelity w rakiecie.
	glRotatef( -90.0f, 0.0f, 1.0f, 0.0f );
	glTranslatef( 0.0f, 0.0f, -3.0f );

		glPushMatrix( );
		// stozek u dolu satelity
			glColor3f( 0.12f, 0.12f, 0.12f );
			glTranslatef( 0.0f, 0.0f, -0.7f );
			glPushMatrix( );
				glClipPlane( GL_CLIP_PLANE0, plane_eq0 );
				glEnable( GL_CLIP_PLANE0 );
				glTranslatef( 0.0f, 0.0f, -0.2f );
				gluDisk( qdo, 0.0f, 0.3f, 30, 15 );
				glColor3f( 1.0f, 1.0f, 1.0f );
				glBindTexture( GL_TEXTURE_2D, satellite1 );
				gluCylinder( satelliteQuadricObject, 0.3f, 0.0f, 1.2f, 30, 20 );
				glDisable( GL_CLIP_PLANE0 );
			glPopMatrix( );
		// stozek u dolu satelity
		glPopMatrix( );


		glPushMatrix( );
		//walec 
			glColor3f( 0.12f, 0.12f, 0.12f );
			glTranslatef( 0.0f, 0.0f, -0.4f );
			gluDisk( qdo, 0.22f, 0.25f, 30, 10 );
			glColor3f( 1.0f, 1.0f, 1.0f );
			glTranslatef( 0.0f, 0.0f, -0.3f );
			glBindTexture( GL_TEXTURE_2D, satellite2 );
			gluCylinder( satelliteQuadricObject, 0.25f, 0.25f, 0.3f, 30, 20 );
		//walec 
		glPopMatrix( );

		glPushMatrix( );
		// walec
			glTranslatef( 0.0f, 0.0f, -0.4f );
			glBindTexture( GL_TEXTURE_2D, satellite3 );
			gluCylinder( satelliteQuadricObject, 0.22f, 0.22f, 0.1f, 30, 10 );
		//walec
		glPopMatrix( );

		glPushMatrix( );
		// stozek sciety
			glTranslatef( 0.0f, 0.0f, -0.2f );
			glPushMatrix( );
				glClipPlane( GL_CLIP_PLANE0, plane_eq0 );
				glEnable( GL_CLIP_PLANE0 );
				glTranslatef( 0.0f, 0.0f, -0.1f );
				glBindTexture( GL_TEXTURE_2D, satellite4 );
				gluCylinder( satelliteQuadricObject, 0.22f, 0.0f, 1.1f, 30, 20 );
				glDisable( GL_CLIP_PLANE0 );
			glPopMatrix( );
		// stozek sciety
		glPopMatrix( );

		glPushMatrix( );
		// Scieta kolpula.
			glTranslatef( 0.0f, 0.0f, -0.2f );
			glPushMatrix( );
				glClipPlane( GL_CLIP_PLANE1, plane_eq1 );
				glEnable( GL_CLIP_PLANE1 );
				glBindTexture( GL_TEXTURE_2D, satellite5 );
				gluSphere( satelliteQuadricObject, 0.2f, 30, 30 );
				glDisable( GL_CLIP_PLANE1 );
			glPopMatrix( );
		// Scieta kopula.
		glPopMatrix( );


		glPushMatrix( );
		// zestaw paneli prawa strona

			glRotatef( 90.0f, 1.0f, 0.0f, 0.0f );
			glTranslatef( -0.25f, -0.65f, 0.2f );
			glRotatef( 90.0f, 0.0f, 0.0f, 1.0f );

			if ( frame_no > 4000 ) {
				if ( satelliteAnimation <= 1800 ) {
					glRotatef( satelliteAnimation/ 20.0f , 0.0f, 0.0f, 1.0f );
					//++satelliteAnimation;
				} else {
					glRotatef( 90, 0.0f, 0.0f, 1.0f );
				}
			}

			glPushMatrix( );
			// obydwa panele i laczniki.

				glBindTexture( GL_TEXTURE_2D, solarpanel );
					glBegin( GL_QUADS );

						//gora panelu
						glNormal3f( 0.0f, 1.0f, 0.0f );
						glTexCoord2f( 0.0f, 0.0f );
						glVertex3f( 0.0f, 0.02f, 0.0f );
						glTexCoord2f( 1.0f, 0.0f );
						glVertex3f( 0.55f, 0.02f, 0.0f );
						glTexCoord2f( 1.0f, 1.0f );
						glVertex3f( 0.55f, 0.02f, -0.4f );
						glTexCoord2f( 0.0f, 1.0f );
						glVertex3f( 0.0f, 0.02f, -0.4f );

						// dol panelu
						glNormal3f( 0.0f, -1.0f, 0.0f );
						glTexCoord2d( 0.0f, 0.0f );
						glVertex3f( 0.0f, 0.0f, 0.0f );
						glTexCoord2d( 1.0f, 0.0f );
						glVertex3f( 0.55f, 0.0f, 0.0f );
						glTexCoord2d( 1.0f, 1.0f );
						glVertex3f( 0.55f, 0.0f, -0.4f );
						glTexCoord2d( 0.0f, 1.0f );
						glVertex3f( 0.0f, 0.0f, -0.4f );
		
						// front panelu
						glColor3f( 0.12f, 0.12f, 0.12f );
						glNormal3f( 0.0f, 0.0f, 1.0f );
						glVertex3f( 0.0f, 0.0f, 0.0f );
						glVertex3f( 0.55f, 0.0f, 0.0f );
						glVertex3f( 0.55f, 0.02f, 0.0f );
						glVertex3f( 0.0f, 0.02f, 0.0f );

						// tyl panelu
						glNormal3f( 0.0f, 0.0f, -1.0f );
						glVertex3f( 0.0f, 0.0f, -0.4f );
						glVertex3f( 0.55f, 0.0f, -0.4f );
						glVertex3f( 0.55f, 0.02f, -0.4f );
						glVertex3f( 0.0f, 0.02f, -0.4f );

						// lewa czesc panelu
						glNormal3f( -1.0f, 0.0f, 0.0f );
						glVertex3f( 0.0f, 0.0f, 0.0f );
						glVertex3f( 0.0f, 0.0f, -0.4f );
						glVertex3f( 0.0f, 0.02f, -0.4f );
						glVertex3f( 0.0f, 0.02f, 0.0f );

						// prawa czesc panelu
						glNormal3f( 1.0f, 0.0f, 0.0f );
						glVertex3f( 0.55f, 0.0f, 0.0f );
						glVertex3f( 0.55f, 0.0f, -0.4f );
						glVertex3f( 0.55f, 0.02f, -0.4f );
						glVertex3f( 0.55f, 0.02f, 0.0f );

					glEnd( );
				
					glPushMatrix( );
					// walec robiacy za laczenie z satelita
						gluDisk( qdo, 0.0f, 0.01f , 10, 10);
						glTranslatef( 0.0f, 0.0f, -0.4f );
						gluCylinder( qdo, 0.01f, 0.01f, 0.4f, 10, 10 );
						gluDisk( qdo, 0.0f, 0.01f, 10, 10 );
					// walec robiacy za laczenie z satelita
					glPopMatrix( );

					glPushMatrix( );
					// walec robiacy za laczenie z drugim panelem
						glTranslatef( 0.55f, 0.02f, -0.4f );
						gluDisk( qdo, 0.0f, 0.015f, 10, 10 );
						gluCylinder( qdo, 0.015f, 0.015f, 0.4f, 10, 10 );
						glTranslatef( 0.0f, 0.0f, 0.4f );
						gluDisk( qdo, 0.0f, 0.015f, 10, 10 );
					// walec robiacy za laczenie z drugim panelem
					glPopMatrix( );

					glPushMatrix( );
					// drugi panel sloneczny.

					glRotatef( 180.0f, 0.0f, 1.0f, 0.0f );
					glTranslatef( -0.55f, 0.03f, 0.4f );

					if ( frame_no > 4000 ) {
						if ( satelliteAnimation <= 1800 ) {
							glRotatef( satelliteAnimation / 10.0f, 0.0f, 0.0f, 1.0f );
							//++satelliteAnimation;
						} else {
							glRotatef( 180.0f, 0.0f, 0.0f, 1.0f );
						}
					}

						glColor3f( 1.0f, 1.0f, 1.0f );
					
						glBegin( GL_QUADS );

						//gora panelu
						glNormal3f( 0.0f, 1.0f, 0.0f );
						glTexCoord2f( 0.0f, 0.0f );
						glVertex3f( 0.0f, 0.02f, 0.0f );
						glTexCoord2f( 1.0f, 0.0f );
						glVertex3f( 0.55f, 0.02f, 0.0f );
						glTexCoord2f( 1.0f, 1.0f );
						glVertex3f( 0.55f, 0.02f, -0.4f );
						glTexCoord2f( 0.0f, 1.0f );
						glVertex3f( 0.0f, 0.02f, -0.4f );

						// dol panelu
						glNormal3f( 0.0f, -1.0f, 0.0f );
						glTexCoord2d( 0.0f, 0.0f );
						glVertex3f( 0.0f, 0.0f, 0.0f );
						glTexCoord2d( 1.0f, 0.0f );
						glVertex3f( 0.55f, 0.0f, 0.0f );
						glTexCoord2d( 1.0f, 1.0f );
						glVertex3f( 0.55f, 0.0f, -0.4f );
						glTexCoord2d( 0.0f, 1.0f );
						glVertex3f( 0.0f, 0.0f, -0.4f );

						// front panelu
						glColor3f( 0.12f, 0.12f, 0.12f );
						glNormal3f( 0.0f, 0.0f, 1.0f );
						glVertex3f( 0.0f, 0.0f, 0.0f );
						glVertex3f( 0.55f, 0.0f, 0.0f );
						glVertex3f( 0.55f, 0.02f, 0.0f );
						glVertex3f( 0.0f, 0.02f, 0.0f );

						// tyl panelu
						glNormal3f( 0.0f, 0.0f, -1.0f );
						glVertex3f( 0.0f, 0.0f, -0.4f );
						glVertex3f( 0.55f, 0.0f, -0.4f );
						glVertex3f( 0.55f, 0.02f, -0.4f );
						glVertex3f( 0.0f, 0.02f, -0.4f );

						// lewa czesc panelu
						glNormal3f( -1.0f, 0.0f, 0.0f );
						glVertex3f( 0.0f, 0.0f, 0.0f );
						glVertex3f( 0.0f, 0.0f, -0.4f );
						glVertex3f( 0.0f, 0.02f, -0.4f );
						glVertex3f( 0.0f, 0.02f, 0.0f );

						// prawa czesc panelu
						glNormal3f( 1.0f, 0.0f, 0.0f );
						glVertex3f( 0.55f, 0.0f, 0.0f );
						glVertex3f( 0.55f, 0.0f, -0.4f );
						glVertex3f( 0.55f, 0.02f, -0.4f );
						glVertex3f( 0.55f, 0.02f, 0.0f );

						glEnd( );


					// drugi panel sloneczny.
					glPopMatrix( );

				// obydwa panele i lacznik.
				glPopMatrix( );

				glColor3f( 1.0f, 1.0f, 1.0f );
		
		// panel sloneczny prawa strona	
		glPopMatrix( );
	
		glPushMatrix( );
		// zestaw paneli lewa strona

			glRotatef( -90.0f, 1.0f, 0.0f, 0.0f );
			glTranslatef( 0.25f, 0.65f, 0.2f );
			glRotatef( -90.0f, 0.0f, 0.0f, 1.0f );

			if ( frame_no > 4000 ) {
				if ( satelliteAnimation <= 1800 ) {
					glRotatef( satelliteAnimation / 20.0f, 0.0f, 0.0f, 1.0f );
				} else {
					glRotatef( 90, 0.0f, 0.0f, 1.0f );
				}
			}

				glPushMatrix( );
				// obydwa panele i laczniki.

				glBindTexture( GL_TEXTURE_2D, solarpanel );
				glBegin( GL_QUADS );

				//gora panelu
				glNormal3f( 0.0f, 1.0f, 0.0f );
				glTexCoord2f( 0.0f, 0.0f );
				glVertex3f( 0.0f, 0.02f, 0.0f );
				glTexCoord2f( 1.0f, 0.0f );
				glVertex3f( 0.55f, 0.02f, 0.0f );
				glTexCoord2f( 1.0f, 1.0f );
				glVertex3f( 0.55f, 0.02f, -0.4f );
				glTexCoord2f( 0.0f, 1.0f );
				glVertex3f( 0.0f, 0.02f, -0.4f );

				// dol panelu
				glNormal3f( 0.0f, -1.0f, 0.0f );
				glTexCoord2d( 0.0f, 0.0f );
				glVertex3f( 0.0f, 0.0f, 0.0f );
				glTexCoord2d( 1.0f, 0.0f );
				glVertex3f( 0.55f, 0.0f, 0.0f );
				glTexCoord2d( 1.0f, 1.0f );
				glVertex3f( 0.55f, 0.0f, -0.4f );
				glTexCoord2d( 0.0f, 1.0f );
				glVertex3f( 0.0f, 0.0f, -0.4f );

				// front panelu
				glColor3f( 0.12f, 0.12f, 0.12f );
				glNormal3f( 0.0f, 0.0f, 1.0f );
				glVertex3f( 0.0f, 0.0f, 0.0f );
				glVertex3f( 0.55f, 0.0f, 0.0f );
				glVertex3f( 0.55f, 0.02f, 0.0f );
				glVertex3f( 0.0f, 0.02f, 0.0f );

				// tyl panelu
				glNormal3f( 0.0f, 0.0f, -1.0f );
				glVertex3f( 0.0f, 0.0f, -0.4f );
				glVertex3f( 0.55f, 0.0f, -0.4f );
				glVertex3f( 0.55f, 0.02f, -0.4f );
				glVertex3f( 0.0f, 0.02f, -0.4f );

				// lewa czesc panelu
				glNormal3f( -1.0f, 0.0f, 0.0f );
				glVertex3f( 0.0f, 0.0f, 0.0f );
				glVertex3f( 0.0f, 0.0f, -0.4f );
				glVertex3f( 0.0f, 0.02f, -0.4f );
				glVertex3f( 0.0f, 0.02f, 0.0f );

				// prawa czesc panelu
				glNormal3f( 1.0f, 0.0f, 0.0f );
				glVertex3f( 0.55f, 0.0f, 0.0f );
				glVertex3f( 0.55f, 0.0f, -0.4f );
				glVertex3f( 0.55f, 0.02f, -0.4f );
				glVertex3f( 0.55f, 0.02f, 0.0f );

				glEnd( );

				glPushMatrix( );
				// walec robiacy za laczenie z satelita
				gluDisk( qdo, 0.0f, 0.01f, 10, 10 );
				glTranslatef( 0.0f, 0.0f, -0.4f );
				gluCylinder( qdo, 0.01f, 0.01f, 0.4f, 10, 10 );
				gluDisk( qdo, 0.0f, 0.01f, 10, 10 );
				// walec robiacy za laczenie z satelita
				glPopMatrix( );

				glPushMatrix( );
				// walec robiacy za laczenie z drugim panelem
				glTranslatef( 0.55f, 0.02f, -0.4f );
				gluDisk( qdo, 0.0f, 0.015f, 10, 10 );
				gluCylinder( qdo, 0.015f, 0.015f, 0.4f, 10, 10 );
				glTranslatef( 0.0f, 0.0f, 0.4f );
				gluDisk( qdo, 0.0f, 0.015f, 10, 10 );
				// walec robiacy za laczenie z drugim panelem
				glPopMatrix( );

					glPushMatrix( );
					// drugi panel sloneczny.

					glRotatef( 180.0f, 0.0f, 1.0f, 0.0f );
					glTranslatef( -0.55f, 0.03f, 0.4f );

					if ( frame_no > 4000 ) {
						if ( satelliteAnimation <= 1800 ) {
							glRotatef( satelliteAnimation / 10.0f, 0.0f, 0.0f, 1.0f );
							++satelliteAnimation;
						} else {
							glRotatef( 180.0f, 0.0f, 0.0f, 1.0f );
						}
					}

					glColor3f( 1.0f, 1.0f, 1.0f );

					glBegin( GL_QUADS );

					//gora panelu
					glNormal3f( 0.0f, 1.0f, 0.0f );
					glTexCoord2f( 0.0f, 0.0f );
					glVertex3f( 0.0f, 0.02f, 0.0f );
					glTexCoord2f( 1.0f, 0.0f );
					glVertex3f( 0.55f, 0.02f, 0.0f );
					glTexCoord2f( 1.0f, 1.0f );
					glVertex3f( 0.55f, 0.02f, -0.4f );
					glTexCoord2f( 0.0f, 1.0f );
					glVertex3f( 0.0f, 0.02f, -0.4f );

					// dol panelu
					glNormal3f( 0.0f, -1.0f, 0.0f );
					glTexCoord2d( 0.0f, 0.0f );
					glVertex3f( 0.0f, 0.0f, 0.0f );
					glTexCoord2d( 1.0f, 0.0f );
					glVertex3f( 0.55f, 0.0f, 0.0f );
					glTexCoord2d( 1.0f, 1.0f );
					glVertex3f( 0.55f, 0.0f, -0.4f );
					glTexCoord2d( 0.0f, 1.0f );
					glVertex3f( 0.0f, 0.0f, -0.4f );

					// front panelu
					glColor3f( 0.12f, 0.12f, 0.12f );
					glNormal3f( 0.0f, 0.0f, 1.0f );
					glVertex3f( 0.0f, 0.0f, 0.0f );
					glVertex3f( 0.55f, 0.0f, 0.0f );
					glVertex3f( 0.55f, 0.02f, 0.0f );
					glVertex3f( 0.0f, 0.02f, 0.0f );

					// tyl panelu
					glNormal3f( 0.0f, 0.0f, -1.0f );
					glVertex3f( 0.0f, 0.0f, -0.4f );
					glVertex3f( 0.55f, 0.0f, -0.4f );
					glVertex3f( 0.55f, 0.02f, -0.4f );
					glVertex3f( 0.0f, 0.02f, -0.4f );

					// lewa czesc panelu
					glNormal3f( -1.0f, 0.0f, 0.0f );
					glVertex3f( 0.0f, 0.0f, 0.0f );
					glVertex3f( 0.0f, 0.0f, -0.4f );
					glVertex3f( 0.0f, 0.02f, -0.4f );
					glVertex3f( 0.0f, 0.02f, 0.0f );

					// prawa czesc panelu
					glNormal3f( 1.0f, 0.0f, 0.0f );
					glVertex3f( 0.55f, 0.0f, 0.0f );
					glVertex3f( 0.55f, 0.0f, -0.4f );
					glVertex3f( 0.55f, 0.02f, -0.4f );
					glVertex3f( 0.55f, 0.02f, 0.0f );

					glEnd( );


					// drugi panel sloneczny.
					glPopMatrix( );

				// obydwa panele i lacznik.
				glPopMatrix( );

			glColor3f( 1.0f, 1.0f, 1.0f );

		// panel sloneczny lewa strona
		glPopMatrix( );

	// satelita
	glPopMatrix( );

	glEnable( GL_LIGHTING );
	glEnable( GL_CULL_FACE );

	glFlush( );
}

/**
 * Wyswietlanie wszystkiego...
 */
void display( )
{
	if ( frame_no < 30000 ) frame_no+= 1; 
	else {
		frame_no = 0;
		satelliteAnimation = 0;
		animation = 0;
	}
	
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	glLoadIdentity( );

	/* Transformacje widoku... */
	gluLookAt( eye.x, eye.y, eye.z, point.x, point.y, point.z, up.x, up.y, up.z );

	displayEnviroment( frame_no );
	displayRocket( frame_no );
	displaySatellite( frame_no );

	glutSwapBuffers( );
}

/**
 * Funkcja wywolywana podczas zmiany rozmiaru okna...
 */
void reshape( GLsizei w, GLsizei h )
{
	// Uaktualnienie pozycji kursora...
	windowHeight = glutGet( GLUT_WINDOW_HEIGHT );
	windowWidht = glutGet( GLUT_WINDOW_WIDTH );
	lastX = windowWidht / 2;
	lastY = windowHeight / 2;
	glutWarpPointer( lastX, lastY );
	
	if ( h > 0 && w > 0 ) {
		glViewport( 0, 0, w, h );
		glMatrixMode( GL_PROJECTION );
		glLoadIdentity( );
		if ( w <= h ) {
			gluPerspective( fov, ( double ) h / ( double ) w, 0.01f, 100.0f );
		} else {
			gluPerspective( fov, ( double ) w / ( double ) h, 0.01f, 100.0f );
		}
		glMatrixMode( GL_MODELVIEW );
	}
}

/**
 * uaktualnienie pozycji czasteczek.
 */
void update( int value )
{
	pe->advance( TIMER_MS / 1000.0f );
	if ( frame_no < 1500 )
		glutTimerFunc( TIMER_MS, update, 0 );
}

/**
 * Rozpoczecie wykonania...
 */
int main( int argc, char** argv )
{
	srand( ( unsigned int ) time( NULL ) );
	pe = new ParticleEngine( 1500, 0.01, 0.06 );

	glutInit( &argc, argv );
	glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH );
	glutInitWindowPosition( 0, 0 );
	glutInitWindowSize( 800, 600 );
	glutCreateWindow( "Satellite" );
	glutFullScreen( );
	
	//uaktualnienie pozycji kursora...
	windowHeight = glutGet( GLUT_WINDOW_HEIGHT );
	windowWidht = glutGet( GLUT_WINDOW_WIDTH );
	lastX = windowWidht / 2;
	lastY = windowHeight / 2;
	glutWarpPointer( lastX, lastY );

	init( );

	glutDisplayFunc( display );
	glutReshapeFunc( reshape );
	glutIdleFunc( display );
	glutKeyboardFunc( handleKeypress );
	glutSpecialFunc( handleSpecialKeys );
	glutPassiveMotionFunc( computeFromMouse );

	glutTimerFunc( TIMER_MS, update, 0 );

	glutMainLoop( );

	return 0;
}
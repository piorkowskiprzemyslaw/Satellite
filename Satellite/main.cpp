/*
 * Projekt OpenGL.
 * autor : Przemys³aw Piórkowski
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

char * earthLocation = "earth.jpg";
char * moonLocation = "moon.jpg";
char * cloudsLocation = "clouds.jpg";

GLsizei earthImageHeight, earthImageWidth;
GLsizei moonImageHeight, moonImageWidth;
GLsizei cloudsImageHeight, cloudsImageWidth;

static GLuint earth;
static GLuint moon;
static GLuint clouds;

static ILuint ilEarth;
static ILuint ilMoon;
static ILuint ilClouds;

ILubyte * earthData;
ILubyte * moonData;
ILubyte * cloudsData;

GLUquadricObj* earthQuadricObject = gluNewQuadric( );
GLUquadricObj* moonQuadricObject = gluNewQuadric( );
GLUquadricObj* cloudsQuadricObject = gluNewQuadric( );

/**
 * Handler dla openGL'a do obslugi klawiszy z klawiatury.
 */
void handleKeypress( unsigned char key, int x, int y )
{    //The current mouse coordinates
	switch ( key ) 
	{
		case 27: //Escape key
			exit( 0 ); //Exit the program
	}
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
}

/**
 * Inicjalizacja OpenGL'a.
 */
void init( )
{
	//Okreslenie wlasciwosci materialow.
	GLfloat mat_ambient[] = { 0.3, 0.3, 0.3, 1.0 };
	GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	
	glMaterialfv( GL_FRONT, GL_AMBIENT, mat_ambient );
	glMaterialfv( GL_FRONT, GL_SPECULAR, mat_specular );
	glMaterialf( GL_FRONT, GL_SHININESS, 50.0 );
	
	//Okreslenie swiatel, otoczenia, glownego...
	GLfloat light_position[] = { 5.0, 1.0, 5.0, 1.0 };
	GLfloat lm_ambient[] = { 0.2, 0.2, 0.2, 1.0f };

	glLightfv( GL_LIGHT0, GL_POSITION, light_position );
	glLightModelfv( GL_LIGHT_MODEL_AMBIENT, lm_ambient );
	
	
	glShadeModel( GL_SMOOTH );

	glEnable( GL_LIGHTING );
	glEnable( GL_LIGHT0 );

	glDepthFunc( GL_LESS );
	glEnable( GL_DEPTH_TEST );
	glEnable( GL_NORMALIZE );
	glEnable( GL_CULL_FACE );
	glEnable( GL_COLOR );

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
}

void displayObjects( float angle )
{
	GLfloat earth_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat moon_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat clouds_blend[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	GLfloat clouds_shinines[] = { 0.0f };
	GLfloat clouds_specular[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	GLfloat clouds_emission[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	GLfloat clouds_diffuse[] = { 0.3f, 0.3f, 0.3f, 1.0f };

	glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
	glEnable( GL_TEXTURE_2D );
	glPushMatrix( );

	// ZIEMIA
			glPushMatrix( );
				glTranslatef( -3.0, -0.8, 0.0 );
				glRotatef( 90, 1.0, 0.0, 0.0 );
				
				glPushMatrix( );
					glRotatef( -( angle/720 + 90), 0.0, 0.0, 1.0 );
					//glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_BLEND );
					glBindTexture( GL_TEXTURE_2D, earth );
					glMaterialfv( GL_FRONT, GL_DIFFUSE, earth_diffuse ); // ???
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
					glRotatef( -angle/90 , 0.0, 0.0, 1.0 );
					glTranslatef( 15.0f, 0.0, 0.0 );
					glRotatef( angle / 720 - 90, 0.0f, 0.0f, 1.0f );
					glBindTexture( GL_TEXTURE_2D, moon );
					glMaterialfv( GL_FRONT, GL_DIFFUSE, moon_diffuse );
					gluSphere( moonQuadricObject, 1.23, 100, 100 );
				glPopMatrix( );
			glPopMatrix( );

	glPopMatrix( );
	glFlush( );
	glDisable( GL_TEXTURE_2D );
}

void display( )
{
	static int frame_no = 0;
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	if ( frame_no < 720*720 ) frame_no++; else frame_no = 0;
	glMatrixMode( GL_PROJECTION );
	glPushMatrix( );
	//glRotatef( ( float ) frame_no, 0.0, 1.0, 0.0 );
		glMatrixMode( GL_MODELVIEW );
		displayObjects( frame_no );
		glMatrixMode( GL_PROJECTION );
	glPopMatrix( );
	glFlush( );
	glutSwapBuffers( );
}

void reshape( GLsizei w, GLsizei h )
{
	if ( h > 0 && w > 0 ) {
		glViewport( 0, 0, w, h );
		glMatrixMode( GL_PROJECTION );
		glLoadIdentity( );
		if ( w <= h ) {
			//gluPerspective( 45.0, ( double ) h / ( double ) w, 11.0, 18.0 );
			glFrustum( -2.25, 2.25, -2.25*h / w, 2.25*h / w, 10.0, 100.0);
			//glOrtho( -2.25, 2.25, -2.25*h/w, 2.25*h/w, -10.0, 10.0 );
		} else {
			//gluPerspective( 45.0, ( double ) w / ( double ) h, 11.0, 18.0 );
			glFrustum( -2.25*w / h, 2.25*w / h, -2.25, 2.25, 10.0, 100.0 );
			//glOrtho( -2.25*w/h, 2.25*w/h, -2.25, 2.25, -10.0, 10.0 );
		}
		glTranslatef( 0.0, 0.0, -16.0 );
		glMatrixMode( GL_MODELVIEW );
	}
}

int main( int argc, char** argv )
{
	glutInit( &argc, argv );
	glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH );
	glutInitWindowPosition( 100, 50 );
	glutInitWindowSize( 800, 600 );
	glutCreateWindow( "Satellite" );
	glutFullScreen( );
	glutDisplayFunc( display );
	glutReshapeFunc( reshape );
	glutIdleFunc( display );
	glutKeyboardFunc( handleKeypress );

	init( );
	glutMainLoop( );

	return 0;
}
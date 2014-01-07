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
GLUquadricObj* qdo = gluNewQuadric( );

static float fov = 45.0f;
static float frame_no = 0;

static float horizontalAngle = 3.14f;
static float verticalAngle = 0.0f;
static float speed = 0.05f;
static float mouseSpeed = 0.005f;

static glm::vec3 eye( 0.0f, 0.0f, 8.0f );
static glm::vec3 point( 0.0f, 0.0f, 0.0f );
static glm::vec3 up( 0.0f, 1.0f, 0.0f );
static glm::vec3 right( 1.0f, 0.0f, 0.0f );
static glm::vec3 direction( 0.0f, 0.0f, 1.0f );

static int windowWidht = 0;
static int windowHeight = 0;
static int lastX = 0;
static int lastY = 0;

static GLfloat light_position[] = { 100.0f, 0.0f, 50.0f, 1.0f };

/**
 * Handler dla openGL'a do obslugi klawiszy z klawiatury.
 */
void handleKeypress( unsigned char key, int x, int y )
{
	switch ( key ) 
	{
		case 'r': // reset pozycji kamery
				fov		= 45.0f;
				eye		= glm::vec3( 0.0f, 0.0f, 8.0f );
				point	= glm::vec3( 0.0f, 0.0f, 0.0f );
				up		= glm::vec3( 0.0f, 1.0f, 0.0f );
				right	= glm::vec3( 1.0f, 0.0f, 0.0f );
				horizontalAngle = 3.14f;
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
}

/**
 * Inicjalizacja OpenGL'a.
 */
void init( )
{
	//Okreslenie wlasciwosci materialow.
	GLfloat mat_ambient[] = { 0.3f, 0.3f, 0.3f, 1.0f };
	GLfloat mat_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	
	glMaterialfv( GL_FRONT, GL_AMBIENT, mat_ambient );
	glMaterialfv( GL_FRONT, GL_SPECULAR, mat_specular );
	glMaterialf( GL_FRONT, GL_SHININESS, 50.0 );
	
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

	glutSetCursor( GLUT_CURSOR_NONE );
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

	glTranslatef( 0.0f, 0.0f, -6.0f );
	glRotatef( 4 *angle / 90.0f, 0.0f, 1.0f, 0.0f );

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
					glRotatef( -angle / 360.0f - 55.0f, 0.0f, 0.0f, 1.0f );
					glTranslatef( 40.0f, 0.0f, 0.0f );
					glRotatef( angle / 720 - 90, 0.0f, 0.0f, 1.0f );
					glBindTexture( GL_TEXTURE_2D, moon );
					glMaterialfv( GL_FRONT, GL_DIFFUSE, moon_diffuse );
					gluSphere( moonQuadricObject, 1.23, 100, 100 );
				glPopMatrix( );
			glPopMatrix( );

	glPopMatrix( );
}

/**
 * Tutaj wyswietlanie rakiety, etc...
 */
void displayObjects( float frame_no )
{
	GLfloat diffuse[] = { 1.0f, 1.0f, 0.0f, 1.0f };
	GLdouble plane_eq0[] = { 0.0f, 0.0f, -1.0f, 0.0f };


	glPushMatrix( );

	//glDisable( GL_LIGHTING );
	glDisable( GL_CULL_FACE );

	glScalef( 0.2f, 0.2f, 0.2f );
	glRotatef( -90.0f, 0.0f, 1.0f, 0.0f );

		// caly model rakiery
		glPushMatrix( );

				glPushMatrix( );
				// modl napedowy. CZERWONY
					glColor3f( 1.0f, 0.0f, 0.0f );
					glTranslatef( 0.0f, 0.0f, -7.05f );
					gluDisk( qdo, 0.0f, 0.5f, 30, 20 );
					//gluCylinder( qdo, 0.5f, 0.5f, 2.0f, 30, 30 );
					glutWireCylinder( 0.5f, 2.0f, 30, 30 );

					//silnik gora
					glPushMatrix( );
						glTranslatef( 0.0f, 0.65f, 0.0f );
						//gluCylinder( qdo, 0.1f, 0.1f, 1.5f, 30, 30 );
						gluDisk( qdo, 0.0f, 0.15f, 30, 10 );
						glutWireCylinder( 0.15f, 1.5f, 30, 30 );
					glPopMatrix( );

					// silnik prawo gora
					glPushMatrix( );
						glTranslatef( 0.468f, 0.468f, 0.0f );
						//gluCylinder( qdo, 0.1f, 0.1f, 1.5f, 30, 30 );
						gluDisk( qdo, 0.0f, 0.15f, 30, 10 );
						glutWireCylinder( 0.15f, 1.5f, 30, 30 );
					glPopMatrix( );

					// silnik lewo gora
					glPushMatrix( );
						glTranslatef( -0.468f, 0.468f, 0.0f );
						//gluCylinder( qdo, 0.1f, 0.1f, 1.5f, 30, 30 );
						gluDisk( qdo, 0.0f, 0.15f, 30, 10 );
						glutWireCylinder( 0.15f, 1.5f, 30, 30 );
					glPopMatrix( );

					//silnik dol
					glPushMatrix( );
						glTranslatef( 0.0f, -0.65f, 0.0f );
						//gluCylinder( qdo, 0.1f, 0.1f, 1.5f, 30, 30 );
						gluDisk( qdo, 0.0f, 0.15f, 30, 10 );
						glutWireCylinder( 0.15f, 1.5f, 30, 30 );
					glPopMatrix( );

					// silnik prawo dol
					glPushMatrix( );
						glTranslatef( 0.468f, -0.468f, 0.0f );
						//gluCylinder( qdo, 0.1f, 0.1f, 1.5f, 30, 30 );
						gluDisk( qdo, 0.0f, 0.15f, 30, 10 );
						glutWireCylinder( 0.15f, 1.5f, 30, 30 );
					glPopMatrix( );

					// silnik lewo dol
					glPushMatrix( );
						glTranslatef( -0.468f, -0.468f, 0.0f );
						//gluCylinder( qdo, 0.1f, 0.1f, 1.5f, 30, 30 );
						gluDisk( qdo, 0.0f, 0.15f, 30, 10 );
						glutWireCylinder( 0.15f, 1.5f, 30, 30 );
					glPopMatrix( );

					// silnik prawo
					glPushMatrix( );
						glTranslatef( 0.65f, 0.0f, 0.0f );
						//gluCylinder( qdo, 0.1f, 0.1f, 1.5f, 30, 30 );
						gluDisk( qdo, 0.0f, 0.15f, 30, 10 );
						glutWireCylinder( 0.15f, 1.5f, 30, 30 );
					glPopMatrix( );

					//silnik lewo
					glPushMatrix( );
						glTranslatef( -0.65f, 0.0f, 0.0f );
						//gluCylinder( qdo, 0.1f, 0.1f, 1.5f, 30, 30 );
						gluDisk( qdo, 0.0f, 0.15f, 30, 10 );
						glutWireCylinder( 0.15f, 1.5f, 30, 30 );
					glPopMatrix( );
				
				// modul silnikowy. czerwony.
				glPopMatrix( );

				glPushMatrix( );
				// lacznik. bialy.
					glTranslatef( 0.0f, 0.0f, -4.65f );
					glPushMatrix( );
						glColor3f( 1.0f, 1.0f, 1.0f );
						glClipPlane( GL_CLIP_PLANE0, plane_eq0 );
						glEnable( GL_CLIP_PLANE0 );
						glTranslatef( 0.0f, 0.0f, -0.4f );
						glutWireCone( 0.5f, 2.0f, 30, 20 );
						glDisable( GL_CLIP_PLANE0 );
					glPopMatrix( );
				// lacznik. bialy.
				glPopMatrix( );

				glPushMatrix( );
				// modul paliwa. Zielony

					glTranslatef( 0.0f, 0.0f, -4.65f );
					glColor3f( 0.0f, 1.0f, 0.0f );
					glutWireCylinder( 0.4f, 3.0f, 30, 30 );

				// modul paliwa. Zielony
				glPopMatrix( );

				glPushMatrix( );
				// modul satelity. Niebieski

				glTranslatef( 0.0f, 0.0f, -1.4f );
					glPushMatrix( );
						glColor3f( 0.0f, 0.0f, 1.0f );
						glClipPlane( GL_CLIP_PLANE0, plane_eq0 );
						glEnable( GL_CLIP_PLANE0 );
						glTranslatef( 0.0f, 0.0f, -0.25f );
						glutWireCone( 0.4f, 0.75f, 30, 20 );
						glDisable( GL_CLIP_PLANE0 );
					glPopMatrix( );

				// modul satelity
				glPopMatrix( );

				glPushMatrix( );
				// modul satelity. Czerwony

					glTranslatef( 0.0f, 0.0f, -1.4f );
					glColor3f( 1.0f, 0.0f, 0.0f );
					glutWireCylinder( 0.265f, 0.5f, 30, 10 );
		
				// modul satelity. Czerwony
				glPopMatrix( );

				glPushMatrix( );
				// modul satelity. Zielony.
				
					glTranslatef( 0.0f, 0.0f, -0.7f );
					glPushMatrix( );
						glColor3f( 0.0f, 1.0f, 0.0f );
						glClipPlane( GL_CLIP_PLANE0, plane_eq0 );
						glEnable( GL_CLIP_PLANE0 );
						glTranslatef( 0.0f, 0.0f, -0.2f );
						glutWireCone( 0.265f, 0.3f, 30, 10 );
						glDisable( GL_CLIP_PLANE0 );
					glPopMatrix( );

				// modul satelity. Zielony.
				glPopMatrix( );

				glPushMatrix( );
				// czubek rakiety. niebieski.
		
					glTranslatef( 0.0f, 0.0f, -0.7f );
					glColor3f( 0.0f, 0.0f, 1.0f );
					glutWireCylinder( 0.088f, 0.3f, 30, 10 );
		
				// czubek rakiety. niebieski.
				glPopMatrix( );

				glPushMatrix( );
				// ostrze czubka. Czerwony.
				
					glTranslatef( 0.0f, 0.0f, -0.4f );
					glColor3f( 1.0f, 0.0f, 0.0f );
					glutWireCone( 0.088f, 0.4f, 30, 10 );

				// ostrze czubka. Czerwony.
				glPopMatrix( );

		// caly model rakiety.
		glPopMatrix( );



	glEnable( GL_CULL_FACE );
	//glEnable( GL_LIGHTING );

	glPopMatrix( );
}

/**
 * Wyswietlanie wszystkiego...
 */
void display( )
{
	if ( frame_no < 720*720 ) frame_no+= 1; else frame_no = 0;
	
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	glLoadIdentity( );

	/* Transformacje widoku... */
	gluLookAt( eye.x, eye.y, eye.z, point.x, point.y, point.z, up.x, up.y, up.z );
	glLightfv( GL_LIGHT0, GL_POSITION, light_position );

	displayEnviroment( (float)frame_no );
	displayObjects( (float) frame_no );

	glFlush( );
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
 * Rozpoczecie wykonania...
 */
int main( int argc, char** argv )
{
	glutInit( &argc, argv );
	glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH );
	glutInitWindowPosition( 100, 50 );
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
	
	glutMainLoop( );

	return 0;
}
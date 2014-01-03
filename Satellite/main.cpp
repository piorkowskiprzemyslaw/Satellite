#include <Windows.h>
#include <GL\glew.h>
#include <GL\freeglut.h>
#include <glm\glm.hpp>
#include <glm\gtx\transform.hpp>
#include <iostream>

//Called when a key is pressed
void handleKeypress( unsigned char key, int x, int y )
{    //The current mouse coordinates
	switch ( key ) 
	{
		case 27: //Escape key
			exit( 0 ); //Exit the program
	}
}

void init( )
{
	GLfloat mat_ambient[] = { 0.5, 0.5, 0.5, 0.5 };
	GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light_position[] = { 0.0, 0.0, 10.0, 1.0 };
	GLfloat lm_ambient[] = { 0.0, 0.0, 0.0, 0.0 };

	glMaterialfv( GL_FRONT, GL_AMBIENT, mat_ambient );
	glMaterialfv( GL_FRONT, GL_SPECULAR, mat_specular );
	glMaterialf( GL_FRONT, GL_SHININESS, 50.0 );
	glLightfv( GL_LIGHT0, GL_POSITION, light_position );
	glLightModelfv( GL_LIGHT_MODEL_AMBIENT, lm_ambient );

	glShadeModel( GL_SMOOTH );

	glEnable( GL_LIGHTING );
	glEnable( GL_LIGHT0 );

	glDepthFunc( GL_LESS );
	glEnable( GL_DEPTH_TEST );
	glEnable( GL_NORMALIZE );
	glEnable( GL_CULL_FACE );
}

void displayObjects( float angle )
{
	GLfloat sphere_diffuse[] = { 1.0, 0.0, 0.0, 1.0 };


	glPushMatrix( );

			glPushMatrix( );
				//glTranslatef( 0.75, 0.60, 0.0 );
				glTranslatef( -1.0, -0.4, 0.0 );
				glRotatef( 90, 1.0, 0.0, 0.0 );
				glRotatef( angle/720, 0.0, 0.0, 1.0 );
				glMaterialfv( GL_FRONT, GL_DIFFUSE, sphere_diffuse );
				glutWireSphere( 2.5, 100, 100 );
			glPopMatrix( );

	glPopMatrix( );
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
			glFrustum( -2.25, 2.25, -2.25*h / w, 2.25*h / w, 10.0, 18.0 );
			//glOrtho( -2.25, 2.25, -2.25*h/w, 2.25*h/w, -10.0, 10.0 );
		} else {
			//gluPerspective( 45.0, ( double ) w / ( double ) h, 11.0, 18.0 );
			glFrustum( -2.25*w / h, 2.25*w / h, -2.25, 2.25, 10.0, 18.0 );
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

	
	glutCreateWindow( "GPOB: OpenGL" );
	glutFullScreen( );

	glutDisplayFunc( display );
	glutReshapeFunc( reshape );
	glutIdleFunc( display );
	glutKeyboardFunc( handleKeypress );

	init( );

	glutMainLoop( );

	system( "PASUE" );
	return 0;
}
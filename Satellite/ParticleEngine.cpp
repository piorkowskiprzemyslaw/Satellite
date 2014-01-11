#include "ParticleEngine.h"

ParticleEngine::ParticleEngine( const unsigned numberOfParticles, const float stepTime, const float particleSize ) : numberOfParticles( numberOfParticles ), stepTime( stepTime ), particleSize(particleSize)
{
	particles = new Particle[numberOfParticles];
	timeUntilNextStep = 0;
	colorTime = 0;
	for ( int i = 0; i < numberOfParticles; i++ ) {
		createParticle( particles + i );
	}

	/*for ( int i = 0; i < 5 / stepTime; i++ ) {
		step( );
	}*/
}

ParticleEngine::~ParticleEngine( )
{
	delete( particles );
}

void ParticleEngine::advance( float dt )
{
	while ( dt > 0 ) {
		if ( timeUntilNextStep < dt ) {
			dt -= timeUntilNextStep;
			step( );
			timeUntilNextStep = stepTime;
		} else {
			timeUntilNextStep -= dt;
			dt = 0;
		}
	}
}

float ParticleEngine::randomFloat( )
{
	return ( float ) rand( ) / ( ( float ) RAND_MAX + 1 );
}

vec3 ParticleEngine::currentVelocity( )
{
	return vec3( 0.0f, 0.0f, - 0.2f );
}

vec3 ParticleEngine::currentColor( )
{
	if ( colorTime < 12 ) {
		++colorTime;
	} else {
		colorTime = 0;
	}

	return vec3( colorsFire[colorTime][0] / 255.0, colorsFire[colorTime][1] / 255.0, colorsFire[colorTime][2] / 255.0 );
}

void ParticleEngine::createParticle( Particle * p )
{
	p->position = vec3( 0.0f, 0.0f, 0.0f );
	p->velocity = currentVelocity( ) - vec3( 0.3f * randomFloat( ) - 0.15f,
											 0.3f * randomFloat( ) - 0.15f,
											 0.3f * randomFloat( ) - 0.15f );
	p->color = currentColor( );
	p->timeAlive = 0;
	p->lifespan = randomFloat( ) + 1;
}

void ParticleEngine::draw( )
{
	for ( unsigned int  i = 0; i < numberOfParticles; i++ ) {
		glColor4f( particles[i].color.x, particles[i].color.y, particles[i].color.z, ( 1 - particles[i].timeAlive / particles[i].lifespan ) );
		float size = particleSize / 2;

		glPushMatrix( );
		glTranslatef( particles[i].position.x, particles[i].position.y, particles[i].position.z );
		
		/*glPointSize( 2.0f );
		glBegin( GL_POINTS );
		glVertex3f( particles[i].position.x, particles[i].position.y, particles[i].position.z );
		glEnd( );*/
		glutSolidSphere( size, 3, 3 );
		glPopMatrix( );
	}
	glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );
}

void ParticleEngine::step( )
{
	for ( int i = 0; i < numberOfParticles; ++i ) {
		Particle* p = particles + i;
		p->position += p->velocity * stepTime;
		p->timeAlive += stepTime;
		if ( p->timeAlive > p->lifespan ) {
			createParticle( p );
		}
	}
}
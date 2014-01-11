#ifndef PARTICLEENGINE_H
#define PARTICLEENGINE_H

#include <Windows.h>
#include <GL\freeglut.h>
#include <glm\glm.hpp>
#include <cstdlib>
#include <iostream>

using namespace glm;

static short colorsFire[][3] = {
	{ 255, 0, 0 },
	{ 232, 0, 0 },
	{ 250, 27, 27 },
	{ 255, 83, 0 },
	{ 215, 70, 0 },
	{ 230, 249, 0 },
	{ 232, 44, 17 },
	{ 231, 130, 18 },
	{ 233, 16, 21 },
	{ 217, 15, 20 },
	{ 236, 214, 55 },
	{ 255, 21, 9 }
};


struct Particle
{
	vec3 position;
	vec3 velocity;
	vec3 color;
	float timeAlive;
	float lifespan;
};

class ParticleEngine
{
private:
	const unsigned int numberOfParticles;
	const float stepTime;
	const float particleSize;
	Particle * particles;
	float timeUntilNextStep;
	int colorTime;

	float randomFloat( );
	vec3 currentColor( );
	vec3 currentVelocity( );
	void createParticle( Particle * p );
	void step( );
public:
	ParticleEngine( const unsigned int numberOfParticles, const float stepTime, const float particleSize );
	~ParticleEngine( );
	void advance( float dt );
	void draw( );
};

#endif
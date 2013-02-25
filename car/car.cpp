#include "car.hpp"

// units in meters, seconds

// hardcoded for now. It's a Honda Civic
Car::Car(const sf::Vector2f& p)
	: rect(sf::Vector2f(4.55676, 1.7526))
{
	vel = 0;
	trg_vel = 0;
	brk_acc = 0;
	brk_max = -37.17112; // 60-0 in 127 feet
	max_vel = 53.6448; // 120mph
	max_acc = 3.7778; // 0-60 in 7.1 seconds

	rect.setPosition(p);
	rect.setFillColor(sf::Color(50, 170, 90));
}

void Car::set_gas(float g)
{
	brk_acc = 0;
	trg_vel = g * max_vel;
}

void Car::set_brake(float b)
{
	trg_vel = 0;
	brk_acc = b * brk_max;
}

void Car::step(float time)
{
	float acc = 0;

	// if not going as fast as you want to, set acceleration proportional to the difference
	if (vel < trg_vel)
		acc = ((max_vel - vel) / (float)max_vel) * max_acc;

	if (brk_acc < 0)
		acc = brk_acc;

	// TODO air resistance for coasting

	vel += acc * time;

	rect.move(vel * time, 0);
}

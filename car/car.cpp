#include "car.hpp"

// units in meters, seconds, kg

// hardcoded for now. It's a Honda Civic
Car::Car(const sf::Vector2f& p)
	: rect(sf::Vector2f(4.55676, 1.7526))
{
	vel = 0;
	trg_vel = 0;
	brk_acc = 0;
	brk_max = -37.17112; // 60-0 in 127 feet
	max_vel = 53.6448; // 120mph
	max_acc = 5.509; // 0-60 in 7.1 seconds (accounting for drag and such)
    mass = 1242.84; // 2740 lb
    weight = mass * 9.81; // kg*m/s^2
    roll_r = 0.015 * 9.81; // maybe not big enough?
	drag_m = (0.5 * 2.51516 * 0.45 * 1.225) / mass; // (1/2 * area of front of car * drag coefficient * density of air) / mass
    time_r = 0.5; // reaction time, proxy for now

    control = MANUAL; // Which self-driving algorithm

	rect.setPosition(p);
	rect.setFillColor(sf::Color(std::rand() % 256, std::rand() % 256, std::rand() % 256));
}

void Car::set_gas(float g)
{
//	brk_acc = 0;
	trg_vel = g * max_vel;
}

void Car::set_brake(float b)
{
	trg_vel = 0;
	brk_acc = b * brk_max;
}

void Car::set_control(unsigned char c)
{
	control = c;
}

// Get the distance between my front bumper and the leader's back
float Car::get_headway(Car leader)
{
    float my_front;
    float lead_front;

    my_front = get_pos() + get_size()/2;
    lead_front = leader.get_pos() + DELTA * leader.get_size()/2;
    return lead_front - my_front;
}

// Get the safe stopping distance, front to front
float Car::get_stop_d(Car leader)
{
    return time_r * max_vel * PHI / 2 + leader.get_size() * DELTA;
}

// Get what the target gas setting of the car should be
// given the car it is following.
float Car::get_auto_vel(Car leader)
{
    float new_vel;
    float headway;
    float stop_d;
    /* TODO Put self-driving algorithms into a nice array
     * algs[control] is the control scheme function.
     * No changes for MANUAL
     * Complain if control >= NUM_CONTROL_TYPES
     * Default to coasting
     */
    switch (control)
    {
        case MANUAL:
            new_vel = trg_vel;
            break;
        case AUTOMATIC1:
            // Follow the leader exactly
            new_vel = leader.get_vel();
            break;
        case AUTOMATIC2:
            /* Go fast until we get close, then follow
             * TODO still collides.  stopping calcs may be off
             * Still problems, but making progess
             */
            headway = get_headway(leader);
            stop_d = get_stop_d(leader);
            if (headway > stop_d)
            {
                new_vel = max_vel;
            }
            else
            {
                new_vel = std::min(leader.vel,
                    2 * (headway - DELTA * leader.get_size()) / (PHI * time_r));
            }
            break;
        default:
            new_vel = leader.get_vel(); //Try not to crash
    }

    return new_vel / (float)max_vel;
}

void Car::step(float time)
{
	acc = 0;


	// if not going the speed you want to, set acceleration proportional to the difference
    // TODO Change to accept an acc and compute those before step
	if (vel < trg_vel)
    {
		acc = ((max_vel - vel) / (float)max_vel) * max_acc;
    }
    /* slow down in proportion to trg_vel
     * Careful with epsilon so we don't divide by zero
     */
    else if (vel > trg_vel)
    {
		acc = ((trg_vel - vel) / ( (float)trg_vel + 
               FLT_EPSILON)) * brk_acc;
    }

    // if set to fixed braking, ignore the trg_vel
	if (brk_acc < 0 )
		acc = brk_acc;

    // we desperately need to stop, so override braking
    // TODO there's probably a better way, but alg was naive
    if (trg_vel < FLT_EPSILON)
        acc = brk_max;

	// deceleration from drag
	drag = drag_m * vel * vel;

	vel += (acc - drag - roll_r) * time;

	// clamp velocity
	if (vel < 0)
		vel = 0;

	if (vel > max_vel)
		vel = max_vel;

	rect.move(vel * time, 0);
}

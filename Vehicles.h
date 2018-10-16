#ifndef Vehicles_h
#define Vehicles_h
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <vector>

#include "Points.h"
#define WIDTH 20
#define HEIGHT 15
#define INFO_MAX 64
using namespace std;

enum types {
	MINER,
	RESCUE,
	EXPLORE,
	NONE = -1 // for virtual function
};

class Vehicles{
public:

	Vehicles(); // Constructor

	// returns the type of the vehicle
	// 0 -> miner
	// 1 -> rescue
	// 2 -> explore
	virtual types getType() const; 

	// returns information about the vehicle
	virtual void getInfo(char *) const;
	// execute the vehicle's role
	virtual void action(Points[WIDTH][HEIGHT]);
	// checks if the vehicle can move to a neighbor tile
	bool canMove(Points[WIDTH][HEIGHT]);
	// moves the vehicle
	void move(Points[WIDTH][HEIGHT]);
	// calculates the probability of the vehicle getting damaged during movement
	void failure(Points[WIDTH][HEIGHT]);

	// returns the speed of the vehicle
	int getSpeed() const;
	// returns the health / ability of the vehicle
	float getHealth() const;
	// returns true of the vehicle is not damaged
	bool isOK() const;
	// checks if the vehicle is doing something
	int isActive() const;

	// sets the x, y coordinates of the vehicle
	void setX(int);
	void setY(int);

	// returns the x, y coordinates of the vehicle
	int getX() const;
	int getY() const;

	// returns the number of failures of the vehicle
	int getFailures() const;
	// returns total number of failures
	static int getTotalFailures();

	// returns number of movements
	int getMoves() const;
	// teleport animation
	int getTeleport() const;
	// destroy animation / timer (-1, -2, -3 for the animation, -4 for the destruction)
	int getDestroy() const;

	// sets vehicle's speed
	void setSpeed(int);
	// sets vehicle's health / ability
	void setHealth(float);
	// sets vehicle's health state
	void setState(bool);
	// active > speed means that the vehicle cannot move
	void setActive(int);

	// teleport animation
	void setTeleport(int);
	// destroy animation
	void setDestroy(int);

private:
	float health;
	static int total_fails;
	int speed, active, x_position, y_position, moves, failures, teleport, destroy;
	bool state;

};


#endif

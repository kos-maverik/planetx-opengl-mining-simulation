#ifndef RobotRescue_h
#define RobotRescue_h
#include "Vehicles.h"

class RobotRescue : public Vehicles {

public:

	RobotRescue(); // Constructor

	// returns the type of the vehicle
	types getType() const;
	// returns information about the vehicle
	void getInfo(char *) const;
	// check for damaged vehicles in the neibhgor tiles and repair them
	void action(Points[WIDTH][HEIGHT]);
	
	// get number of repairs
	int getRepairs() const;
	// get number of total repairs by all vehicles
	static int getTotalRepairs();
	// increase number of total repairs
	static void addTotalRepairs(int);

private:
	int repairs;
	static int total_repairs;

};

#endif
#ifndef RobotExplore_h
#define RobotExplore_h
#include "Vehicles.h"

class RobotExplore : public Vehicles {

public:

	RobotExplore(); // Constructor

	// returns the type of the vehicle
	types getType() const;
	// returns information about the vehicle
	void getInfo(char *) const;
	// explores the neighbor tiles and set danger flags
	void action(Points[WIDTH][HEIGHT]);

	// returns number of flags set by the vehicle
	int getFlags() const;

	// returns number of flags set by all vehicles
	static int getTotalFlags();
	// increases the number of the flags set
	static void addTotalFlags(int);

private:
	int flags;
	static int total_flags;

};

#endif
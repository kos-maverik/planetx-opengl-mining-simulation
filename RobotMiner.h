#ifndef RobotMiner_h
#define RobotMiner_h
#include "Vehicles.h"

class RobotMiner : public Vehicles {

public:

	RobotMiner();

	int getPalladium() const;
	int getIridium() const;
	int getPlatinum() const;
	bool isFull() const;

	// returns the type of the vehicle
	types getType() const;
	// returns information about the vehicle
	void getInfo(char *) const;
	// mines the minerals at a specific map tile
	void action(Points[WIDTH][HEIGHT]);

	// returns total palladium mined by all vehicles
	static int getTotalPalladium();
	// returns total iridium mined by all vehicles
	static int getTotalIridium();
	// returns total platinum mined by all vehicles
	static int getTotalPlatinum();

	void setPalladium(int);
	void setIridium(int);
	void setPlatinum(int);

	void addPalladium(int);
	void addIridium(int);
	void addPlatinum(int);

private:
	int palladium, iridium, platinum, total_palladium, total_iridium, total_platinum;
	static const int max_weight = 50;
	static int all_palladium, all_iridium, all_platinum;
};

#endif
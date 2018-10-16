#ifndef Points_h
#define Points_h
#include <cstdlib>
class Vehicles;

class Points {

public:

	Points(); // Constructor

	void setPalladium(int);
	void setIridium(int);
	void setPlatinum(int);

	int getPalladium() const;
	int getIridium() const;
	int getPlatinum() const;

	// returns a Vehicle object if the tile is occupied
	// otherwise returns NULL
	Vehicles* isOccupied() const;

	// returns true if the tile is flagged as dangerous
	bool isFlagged() const;
	// sets the danger levels
	void setDanger(float);
	// sets whether the tile is occupied
	void setOccupied(Vehicles*);
	// sets the danger flag
	void setFlagged(bool);

	// returns the minerals of the tile
	int getContents() const;
	// returns the danger levels of the tile
	float getDanger() const;

private:
	bool flag;
	Vehicles* occupied;
	float danger;
	int palladium, iridium, platinum;
};

#endif
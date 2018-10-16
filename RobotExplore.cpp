#include "RobotExplore.h"

RobotExplore::RobotExplore()
{
	// flags set by the vehicle
	flags = 0;
}

// total flags set by all vehicles
int RobotExplore::total_flags = 0;

types RobotExplore::getType() const
{
	return EXPLORE;
}

// write number of flags in the variable c
void RobotExplore::getInfo(char *c) const
{
	snprintf(c, INFO_MAX, "Flags : %d", flags);
}

void RobotExplore::action(Points map[WIDTH][HEIGHT])
{
	int X = this->getX(), Y = this->getY(), dx = -1, dy = -1;
	bool done = false;
	if (this->isActive() != this->getSpeed()) {
		this->setActive(this->isActive() - 1);
	}
	else {
		this->setActive(0);
	}
	do {
		do {
			// checks the neighboring tiles, including diagonally, until it finds a dangerous tile
			// checks for the map's limits, whether the tile is dangerous and there is not a flag set
			// if all the above are true, sets a danger flag on the corresponding tile
			if ((dx || dy) && X + dx >= 0 && X + dx < WIDTH && Y + dy >= 0 && Y + dy < HEIGHT		
				&& map[X + dx][Y + dy].getDanger() > 0.5 && !map[X + dx][Y + dy].isFlagged()) {
				// flag set
				map[X + dx][Y + dy].setFlagged(true);
				done = true;
				++flags;
				++total_flags;
				// the vehicle is staying active for another round
				this->setActive(this->isActive() + 1);
			}
		} while (++dy < 2 && !done);
		// dx, dy are used to get the neighbor tiles in two while loops
		dy = -1;
	} while (++dx < 2 && !done);
}

int RobotExplore::getFlags() const
{
	return flags;
}

int RobotExplore::getTotalFlags()
{
	return total_flags;
}

void RobotExplore::addTotalFlags(int shm)
{
	total_flags += shm;
}

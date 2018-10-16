#include "RobotRescue.h"

RobotRescue::RobotRescue()
{
	repairs = 0;
}

// total repairs by all vehicles
int RobotRescue::total_repairs = 0;

types RobotRescue::getType() const
{
	return RESCUE;
}

// writes number of repairs in the variable c
void RobotRescue::getInfo(char *c) const
{
	snprintf(c, INFO_MAX, "Rescues : %d", repairs);
}

void RobotRescue::action(Points map[WIDTH][HEIGHT])
{
	int unsigned dx = -1, dy = -1, X = this->getX(), Y = this->getY();
	bool done = false;			

	if (this->isActive() != this->getSpeed()) {
		this->setActive(this->isActive() - 1);
	}
	else {
		this->setActive(0);
	}
	do {
		do {
			// checks the neighbor tiles within the map's limits for another vehicle
			if ((dx || dy) && X + dx >= 0 && X + dx < WIDTH && Y + dy >= 0 && Y + dy < HEIGHT
				&& map[X + dx][Y + dy].isOccupied()) {
				// get that vehicle
				Vehicles* veh = map[X + dx][Y + dy].isOccupied();
				// checks if the vehicle is damaged
				if (veh->getDestroy() > 0) {
					// sets the vehicle's state to healthy
					veh->setState(true);
					// the rescue vehicle will stay active for one more round
					this->setActive(this->isActive() + 1);
					// the repaired vehicle will stay active for one more round
					veh->setActive(veh->isActive() + 1);
					// destroy = 0 means that the vehicle is safe
					veh->setDestroy(0);
					++repairs;
					++total_repairs;
					// repair done
					done = true;
				}
			}
		} while (++dy < 2 && !done);
		dy = -1;
	} while (++dx < 2 && !done);
}

int RobotRescue::getRepairs() const
{
	return repairs;
}

int RobotRescue::getTotalRepairs()
{
	return total_repairs;
}

void RobotRescue::addTotalRepairs(int ep)
{
	total_repairs += ep;
}

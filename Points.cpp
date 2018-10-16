#include "Points.h"

Points::Points()
{
	// danger flag
	flag = false;
	// random initial mineral levels
	palladium = rand() % 10;
	iridium = rand() % 10;
	platinum = rand() % 10;
	// random danger levels
	danger = rand() % 15 / 10.0f;
	if (danger > 0.9) {
		danger -= 1;
	}
	// no vehicle by default
	occupied = NULL;
}

Vehicles* Points::isOccupied() const
{
	return occupied;
}

bool Points::isFlagged() const
{
	return flag;
}

void Points::setPalladium(int pal)
{
	palladium = pal;
}

void Points::setIridium(int ir)
{
	iridium = ir;
}

void Points::setPlatinum(int plat)
{
	platinum = plat;
}

void Points::setDanger(float ep)
{
	danger = ep;
}

void Points::setOccupied(Vehicles* veh)
{
	occupied = veh;
}

void Points::setFlagged(bool fl)
{
	flag = fl;
}

int Points::getPalladium() const
{
	return palladium;
}

int Points::getIridium() const
{
	return iridium;
}

int Points::getPlatinum() const
{
	return platinum;
}

int Points::getContents() const
{
	return palladium + iridium + platinum;
}

float Points::getDanger() const
{
	return danger;
}

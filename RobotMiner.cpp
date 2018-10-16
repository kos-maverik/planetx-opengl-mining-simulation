#include "RobotMiner.h"

RobotMiner::RobotMiner()
{
	// initialize everything with zero
	palladium = 0;
	iridium = 0;
	platinum = 0;
	total_iridium = 0;
	total_platinum = 0;
	total_palladium = 0;
}

int RobotMiner::all_palladium = 0, RobotMiner::all_iridium = 0, RobotMiner::all_platinum = 0;

types RobotMiner::getType() const
{
	return MINER;
}

// writes information about the minerals mined in the variable c
void RobotMiner::getInfo(char *c) const
{
	snprintf(c, INFO_MAX, "Plat / Irid / Pallad : %d / %d / %d", total_platinum, total_iridium, total_palladium);
}

void RobotMiner::action(Points map[WIDTH][HEIGHT])
{
	int X = this->getX(), Y = this->getY();
	// active decreases until it reaches speed's value
	// active increases when the vehicle is doing something
	if (this->isActive() != this->getSpeed()) {
		this->setActive(this->isActive() - 1);
	}
	else {
		this->setActive(0);
	}
	// if the map tile is high in minerals
	if (map[X][Y].getContents() > 3)
	{
		// start mining
		this->addIridium(map[X][Y].getIridium());
		this->addPlatinum(map[X][Y].getPlatinum());
		this->addPalladium(map[X][Y].getPalladium());
		// empty the tile of the minerals
		map[X][Y].setIridium(0);
		map[X][Y].setPlatinum(0);
		map[X][Y].setPalladium(0);
		this->setActive(this->isActive() + 1);
	}
}

int RobotMiner::getTotalPalladium()
{
	return all_palladium;
}

int RobotMiner::getTotalIridium()
{
	return all_iridium;
}

int RobotMiner::getTotalPlatinum()
{
	return all_platinum;
}

int RobotMiner::getPalladium() const
{
	return palladium;
}

int RobotMiner::getIridium() const
{
	return iridium;
}

int RobotMiner::getPlatinum() const
{
	return platinum;
}

bool RobotMiner::isFull() const
{
	return (palladium + iridium + platinum >= max_weight);
}

void RobotMiner::setPalladium(int pal)
{
	palladium = pal;
}

void RobotMiner::setIridium(int ir)
{
	iridium = ir;
}

void RobotMiner::setPlatinum(int plat)
{
	platinum = plat;
}

void RobotMiner::addPalladium(int pal)
{
	int _pal = palladium + pal > max_weight ? max_weight - palladium : pal;
	palladium += _pal;
	total_palladium += _pal;
	all_palladium += _pal;
}

void RobotMiner::addIridium(int ir)
{
	int _ir = iridium + ir > max_weight ? max_weight - iridium : ir;
	iridium += _ir;
	total_iridium += _ir;
	all_iridium += _ir;
}

void RobotMiner::addPlatinum(int plat)
{
	int _plat = platinum + plat > max_weight ? max_weight - platinum : plat;
	platinum += _plat;
	total_platinum += _plat;
	all_platinum += _plat;
}

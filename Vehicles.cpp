#include "Vehicles.h"

Vehicles::Vehicles()
{
	// initialization of vehicle's properties
	health = (rand() % 15 + 1) / 10.0f;
	if (health > 1) {
		health -= 0.5;
	}
	state = true;	
	// speed:
	// 0 -> fast
	// -1 -> normal
	// -2 -> slow
	speed = rand() % 3 - 2;
	active = 0;	
	moves = 0;
	failures = 0;
	teleport = 0;
	destroy = 0;
}

// total failures of all vehicles
int Vehicles::total_fails = 0;

types Vehicles::getType() const { return NONE; }
void Vehicles::getInfo(char *) const { std::cout << "ERROR"; }
void Vehicles::action(Points map[WIDTH][HEIGHT]) { std::cout << "ERROR"; }

// checks if the vehicle can move in a neighbor tile
bool Vehicles::canMove(Points map[WIDTH][HEIGHT])
{
	int dx = -1, dy = -1, X = this->getX(), Y = this->getY();
	bool ok = false;
	// checks in all neighbor tiles for other vehicles and flags
	do {
		do {
			if (X + dx >= 0 && X + dx < WIDTH && Y + dy >= 0 && Y + dy < HEIGHT &&
				!map[X + dx][Y + dy].isOccupied() && !map[X + dx][Y + dy].isFlagged()) {
				ok = true;
			}
		} while (!ok && ++dy < 2);
		dy = -1;
	} while (!ok && ++dx < 2);
	return ok;
}

void Vehicles::move(Points map[WIDTH][HEIGHT])
{
	int X = this->getX(), Y = this->getY(), dX, dY;
	// increase number of moves
	++moves;
	do {
		// random dx, dy in {-1, 0, 1}
		// moves in neighbor tile, including diagonally
		dX = (rand() % 3) - 1;
		dY = (rand() % 3) - 1;
	// find a tile that is within the map's limits, is not occupied and is not flagged
	} while (X + dX < 0 || X + dX >= WIDTH || Y + dY < 0 || Y + dY >= HEIGHT ||
		map[X + dX][Y + dY].isOccupied() || map[X + dX][Y + dY].isFlagged());
	// previous position is no longer occupied
	map[X][Y].setOccupied(NULL);
	// new position is occupied by the vehicle
	map[X + dX][Y + dY].setOccupied(this);
	// new coordinates
	this->setX(X + dX);
	this->setY(Y + dY);
}

void Vehicles::failure(Points map[WIDTH][HEIGHT])
{
	int X = this->getX(), Y = this->getY();
	// probability of getting damaged during movement
	int prob = rand() % 10;
	if (prob < 10 * map[X][Y].getDanger() * (1 - this->getHealth())) {
		// damaged
		this->setState(false);
		// lower the danger levels of the tile in half
		map[X][Y].setDanger(map[X][Y].getDanger() / 2);
	}
}

void Vehicles::setX(int x)
{
	x_position = x;
}

void Vehicles::setY(int y)
{
	y_position = y;
}

int Vehicles::getSpeed() const
{
	return speed;
}

float Vehicles::getHealth() const
{
	return health;
}

bool Vehicles::isOK() const
{
	return state;
}

int Vehicles::isActive() const
{
	return active;
}

int Vehicles::getX() const
{
	return x_position;
}

int Vehicles::getY() const
{
	return y_position;
}

int Vehicles::getFailures() const
{
	return failures;
}

int Vehicles::getTotalFailures()
{
	return total_fails;
}

int Vehicles::getMoves() const
{
	return moves;
}

int Vehicles::getTeleport() const
{
	return teleport;
}

int Vehicles::getDestroy() const
{
	return destroy;
}

void Vehicles::setSpeed(int sp)
{
	speed = sp;
}

void Vehicles::setHealth(float ab)
{
	health = ab;
}

void Vehicles::setState(bool st)
{
	state = st;
	if (st == false) {
		++failures;
		++total_fails;
	}
}

void Vehicles::setActive(int ac)
{
	active = ac;
}

void Vehicles::setTeleport(int tel)
{
	teleport = tel;
}

void Vehicles::setDestroy(int des)
{
	destroy = des;
}

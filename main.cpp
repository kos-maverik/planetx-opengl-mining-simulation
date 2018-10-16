#include <iostream>
#include <cstdlib>
#include <ctime>
#include <vector>

#ifdef __APPLE__
#include <GLUT/glut.h>	/* Open GL Util APPLE */
#else
#include <GL/glut.h>	/* Open GL Util OpenGL*/
#endif

#include "RobotExplore.h"
#include "RobotRescue.h"
#include "RobotMiner.h"
#include "imageloader.h"
// no console
// #pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")

using namespace std;

// Textures
GLuint Grass, Grass_h, Danger, Base, Options, Success, Fail;
GLuint Textures[18];
// menu
bool pause = false, info = false, VehiclesInfo = false,
MapInfo = false, editVeh = false, editMap = false;
// mouse
int press_x = -1, press_y = -1;
// user input
int option = 0, input = 0;

// menu states
enum states {
	RUNNING,
	ADD_VEHICLE,
	REMOVE_VEHICLE,
	TOGGLE_REPAIR_VEHICLE,
	EDIT_VEHICLE_ABILITY,
	EDITING_VEHICLE_ABILITY,
	EDIT_MINERAL_LEVELS,
	EDITING_MINERAL_LEVELS,
	EDIT_DANGER_LEVELS,
	EDITING_DANGER_LEVELS,
	RANDOMIZE_MAP_TILE,
	TOGGLE_DANGER_FLAG
};

// simulation state
states busy = RUNNING;

Points map[WIDTH][HEIGHT];	// map object
std::vector <Vehicles *> vehicles;	// all vehicles / robots

// minerals gathered in the spaceship
int base_palladium = 0, base_iridium = 0, base_platinum = 0;
int pixels_w = 1024, pixels_h = 768;

// menu consists of eight strings
// c1-c3 are the top three
// c4-c6 are the bottom three
char c1[64], c2[64], c3[64], c4[64], c5[64], c6[64];
// cmid is in the middle
// cmenu is on the bottom right
char cmid[64], cmenu[8];

// function prototypes
// teleport animation
void teleport1(int);
void teleport2(int);
void teleport3(int);
//destroy animation
void destroy1(int);
void destroy2(int);
void destroy3(int);

// prints a string on the window
void output(float x, float y, float z, float red, float green, float blue, void *font, char *string)
{
	// color
	glColor3f(red, green, blue);
	// position
	glRasterPos3f(x, y + 1, z);
	int len, i;
	len = (int)strlen(string);
	for (i = 0; i < len; i++) {
		glutBitmapCharacter(font, string[i]);
	}
}

// game ending
int finished()
{
	bool ok = false;
	unsigned int i = 0;
	if (base_iridium + base_platinum + base_palladium >= 1500) {
		// if the required minerals are gathered in the spaceship
		// return 1 and end the game
		return 1;
	}
	else {
		if (!vehicles.empty()) {
			do {
				if (vehicles[i]->getType() == MINER) {
					// if there are any miner robots
					ok = true;
				}
			} while (++i < vehicles.size() && !ok);
		}
		if (ok) {	
			// continue the simulation
			return 0;
		}
		// no miner robots are left, end the game
		else return 2;
	}
}

// keyboard listener
void handleKeypress(unsigned char key, int x, int y)
{
	switch (key) {
	case 'p':
		// 'p' stands for pause / resume
		if (pause) {
			info = false;
			VehiclesInfo = false;
			MapInfo = false;
			editVeh = false;
			editMap = false;
			busy = RUNNING;
			input = 0;
			option = 0;
		}
		pause = !pause;
		break;
	case 27:
		// 27 stands for escape and quit
		glutDestroyWindow(glutGetWindow());
		exit(0);
		break;
	}

	if (!finished() && !busy) {
		// inside the pause menu
		switch (key) { // huge switch statement
		case 'i':
			if (pause && !VehiclesInfo && !MapInfo && !editVeh && !editMap) {
				// toggles general info
				info = !info;
			}
			break;
		case 'v':
			if (pause && !info && !MapInfo && !editVeh && !editMap) {
				// toggles vehicle info
				VehiclesInfo = !VehiclesInfo;
			}
			break;
		case 'm':
			if (pause && !info && !VehiclesInfo && !editVeh && !editMap) {
				// toggles map info
				MapInfo = !MapInfo;
			}
			break;
		case 'e':
			if (pause && !info && !VehiclesInfo && !MapInfo && !editMap) {
				// toggles vehicle editing
				editVeh = !editVeh;
			}
			break;
		// inside the vehicle editing menu
		case 'z':
			if (editVeh) {
				// removes vehicle
				busy = REMOVE_VEHICLE;
			}
			break;
		case 'x':
			if (editVeh) {
				// repairs / breaks vehicle
				busy = TOGGLE_REPAIR_VEHICLE;
			}
			break;
		case 'c':
			if (editVeh) {
				// edits vehicle's ability
				busy = EDIT_VEHICLE_ABILITY;
			}
			break;
		// inside the map editing menu
		case 'r':
			if (pause && !info && !VehiclesInfo && !MapInfo && !editVeh) {
				// toggles map editing
				editMap = !editMap;
			}
			break;
		case 'd':
			if (editMap) {
				// edits danger levels
				busy = EDIT_DANGER_LEVELS;
			}
			break;
		case 'f':
			if (editMap) {
				// randomizes map tile's properties
				busy = RANDOMIZE_MAP_TILE;
			}
			break;
		case 'g':
			if (editMap) {
				// puts / removes danger flag
				busy = TOGGLE_DANGER_FLAG;
			}
			break;
		// inside the vehicle editing menu
		case '1':
		case '2':
		case '3':
			if (editVeh) {
				// converts character to number
				option = key - 48;
				// adds vehicle
				busy = ADD_VEHICLE;
			}
			break;
		case '4':
		case '5':
		case '6':
			if (editMap) {
				option = key - 48;
				// edits mineral levels
				busy = EDIT_MINERAL_LEVELS;
			}
			break;
		}
	}
	else if (  busy == EDIT_VEHICLE_ABILITY
			|| busy == EDIT_MINERAL_LEVELS
			|| busy == EDIT_DANGER_LEVELS  ) {
		if (key >= '0' && key <= '9') {
			busy = (states)((int)busy + 1);
			input = key - 48;
		}
	}
	glutPostRedisplay();
}

// mouse listener
void mouseClick(int button, int state, int x, int y)
{
	unsigned int i = 0;
	bool done = false;
	// mouse click x, y coordinates
	int _x = WIDTH * x / pixels_w;
	int _y = (HEIGHT + 2)* y / pixels_h;
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) { 
		if (pause) {
			// lights up the clicked tile
			press_x = _x;
			press_y = _y;
		}
		// check if clicked tile is in the map
		if (_y < HEIGHT) {
			// gets vehicle in clicked tile, or null if there is none
			Vehicles* vehicle = map[_x][_y].isOccupied();
			if (VehiclesInfo) {
				// inside the vehicles info menu
				if (vehicle) {
					// finds the vehicle's position in the vector
					do {
						if (vehicles[i] == vehicle) {
							done = true;
						}
					} while (++i < vehicles.size() && !done);

					// gets information about the vehicle
					vehicle->getInfo(c2);

					// prints useful info about the vehicle
					sprintf_s(c3, sizeof(c3), "Vehicle %d, Type: %s", --i,
						vehicle->getType() == MINER ? "Miner" :
						vehicle->getType() == RESCUE ? "Rescue" : "Exploration");
					sprintf_s(c4, sizeof(c4), "Moves: %d", vehicle->getMoves());
					sprintf_s(c5, sizeof(c5), "Mulfunctions: %d", vehicle->getFailures());
					sprintf_s(c6, sizeof(c6), "Ability (0.1 - 1.0): %.1f", vehicle->getHealth());
				}
			}
			else if (MapInfo) {
				// prints info about a map tile
				sprintf_s(c2, sizeof(c2), "Palladium: %d", map[_x][_y].getPalladium());
				sprintf_s(c4, sizeof(c4), "Iridium: %d", map[_x][_y].getIridium());
				sprintf_s(c6, sizeof(c6), "Platinum: %d", map[_x][_y].getPlatinum());
				sprintf_s(c3, sizeof(c3), "Tile [%d] [%d]", _x, _y);
				sprintf_s(c5, sizeof(c5), "Danger (0.0 - 0.9): %.1f", map[_x][_y].getDanger());
			}
			else if (busy == ADD_VEHICLE && !vehicle && !map[_x][_y].isFlagged()) {
				if (option == 1) {
					// adds an exploring vehicle
					RobotExplore *Veh1 = new RobotExplore();
					map[_x][_y].setOccupied(Veh1);
					Veh1->setX(_x);
					Veh1->setY(_y);
					vehicles.push_back(Veh1);
				}
				else if (option == 2) {
					// add a rescue vehicle
					RobotRescue *Veh2 = new RobotRescue();
					map[_x][_y].setOccupied(Veh2);
					Veh2->setX(_x);
					Veh2->setY(_y);
					vehicles.push_back(Veh2);
				}
				else if (option == 3) {
					// add a mining vehicle
					RobotMiner *Veh3 = new RobotMiner();
					map[_x][_y].setOccupied(Veh3);
					Veh3->setX(_x);
					Veh3->setY(_y);
					vehicles.push_back(Veh3);
				}
				busy = RUNNING;
				option = 0;
			}
			else if (busy == REMOVE_VEHICLE && vehicle) {
				// finds the vehicle in the vector, then removes it
				do {
					if (vehicles[i] == vehicle) {
						done = true;
					}
				} while (++i < vehicles.size() && !done);
				if (done) {
					busy = RUNNING;
					destroy1(--i);
				}
			}
			else if (busy == TOGGLE_REPAIR_VEHICLE && vehicle) {
				// toggle the vehicle's healthy state
				busy = RUNNING;
				RobotRescue::addTotalRepairs(!vehicle->isOK());
				vehicle->setState(!vehicle->isOK());
				vehicle->setDestroy(0);
			}
			else if (busy == EDITING_VEHICLE_ABILITY && vehicle) {
				// edit the vehicle's ability in the clicked tile
				vehicle->setHealth((input + 1) / 10.0f);
				input = 0;
				busy = RUNNING;
			}
			else if (busy == EDITING_MINERAL_LEVELS) {
				if (option == 4) {
					// edit palladium levels
					map[_x][_y].setPalladium(input);
				}
				else if (option == 5) {
					// edit iridium levels
					map[_x][_y].setIridium(input);
				}
				else if (option == 6) {
					// edit platinum levels
					map[_x][_y].setPlatinum(input);
				}
				busy = RUNNING;
				input = 0;
			}
			else if (busy == EDITING_DANGER_LEVELS) {
				// edit danger levels in the clicked tile
				map[_x][_y].setDanger(input / 10.0f);
				input = 0;
				busy = RUNNING;
			}
			else if (busy == RANDOMIZE_MAP_TILE) {
				// randomize mineral and danger values in the clicked tile
				Points *s = new Points();
				s->setFlagged(map[_x][_y].isFlagged());
				map[_x][_y] = *s;
				busy = RUNNING;
			}
			else if (busy == TOGGLE_DANGER_FLAG) {
				// put / remove flag in the clicked tile
				map[_x][_y].setFlagged(!map[_x][_y].isFlagged());
				RobotExplore::addTotalFlags(map[_x][_y].isFlagged() ? +1 : -1);
				busy = RUNNING;
			}
		}
	}
	else if (state == GLUT_UP) {
		// simple visual effect for the mouse click
		press_x = -1;
		press_y = -1;
	}
	glutPostRedisplay();
}

// returns remaining platinum in map
int getRemainingPlatinum()
{
	int plat = 0;
	for (int i = 0; i < WIDTH; i++) {
		for (int j = 0; j < HEIGHT; j++) {
			plat += map[i][j].getPlatinum();
		}
	}
	return plat;
}

// returns remaining iridium in map
int getRemainingIridium()
{
	int ir = 0;
	for (int i = 0; i < WIDTH; i++) {
		for (int j = 0; j < HEIGHT; j++) {
			ir += map[i][j].getIridium();
		}
	}
	return ir;
}

// returns remaining palladium in map
int getRemainingPalladium()
{
	int pal = 0;
	for (int i = 0; i < WIDTH; i++) {
		for (int j = 0; j < HEIGHT; j++) {
			pal += map[i][j].getPalladium();
		}
	}
	return pal;
}

// returns average danger in map
float getDanger()
{
	float danger = 0;
	for (int i = 0; i < WIDTH; i++) {
		for (int j = 0; j < HEIGHT; j++) {
			danger += map[i][j].getDanger();
		}
	}
	return danger / (WIDTH*HEIGHT);
}


// removes a vehicle from the vector
void clearVehicles(int value)
{
	// start from the vector's end to prevent errors
	for (int i = vehicles.size() - 1; i >= 0; i--) {
		if (vehicles[i]->getDestroy() < -3) {
			vehicles.erase(vehicles.begin() + i);
		}
	}
}

// prints everything
void draw(GLuint texture, float x, float y, float z) {
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// background gets darker if p(pause) is pressed and even darker if i(info) is pressed
	glColor3f(1.0f - (pause + info) * 0.25f + (press_x == x && press_y == y) *0.25,
		1.0f - (pause + info) * 0.25f + (press_x == x && press_y == y) *0.25,
		1.0f - (pause + info) * 0.25f + (press_x == x && press_y == y) *0.25);

	glBegin(GL_QUADS);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(x, y + 1, z);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(x + 1, y + 1, z);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(x + 1, y, z);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(x, y, z);

	glEnd();
}

// starts the vehicle teleporting animation
void goToBase(int i)
{
	// the animation order: teleport1 -> teleport2 -> teleport3 -> teleport2 -> teleport1
	// total duration 5 * 70 = 350ms
	glutTimerFunc(70, teleport1, i);
}

void teleport1(int i)
{
	// if the vehicle's storage is full, the animation goes forwards
	// or else it goes backwards
	if (((RobotMiner*)vehicles[i])->isFull()) {
		// start teleporting
		vehicles[i]->setTeleport(1);
		// call next function in 70ms
		glutTimerFunc(70, teleport2, i);
	}
	else {
		// the animation is finished
		vehicles[i]->setTeleport(0);
	}
	glutPostRedisplay();
}

void teleport2(int i)
{
	vehicles[i]->setTeleport(2);
	if (((RobotMiner*)vehicles[i])->isFull()) {
		// call next teleport function
		glutTimerFunc(70, teleport3, i);
	}
	else {
		// call previous teleport function
		glutTimerFunc(70, teleport1, i);
	}
	glutPostRedisplay();
}

void teleport3(int i)
{
	RobotMiner* r = (RobotMiner*)vehicles[i];
	int _ir, _plat, _pal;
	vehicles[i]->setTeleport(3);
	// moves the gathered minerals to the spaceship
	_ir = r->getIridium() + base_iridium > 500 ? 500 - base_iridium : r->getIridium();
	_plat = r->getPlatinum() + base_platinum > 500 ? 500 - base_platinum : r->getPlatinum();
	_pal = r->getPalladium() + base_palladium > 500 ? 500 - base_palladium : r->getPalladium();

	base_iridium += _ir;
	base_platinum += _plat;
	base_palladium += _pal;
	// empty the vehicle's storage
	r->setIridium(0);
	r->setPlatinum(0);
	r->setPalladium(0);
	// teleport back to the map
	glutTimerFunc(70, teleport2, i);
	glutPostRedisplay();
}

// set the vehicle in 'destroy' state (destroy = -4)
void destroy(int i)
{
	map[vehicles[i]->getX()][vehicles[i]->getY()].setOccupied(NULL);
	vehicles[i]->setDestroy(-4);
	glutPostRedisplay();
}

void destroy1(int i)
{
	// the animation order: destroy1 -> destroy2 -> destroy3
	// total duration 3 * 70 = 210ms
	vehicles[i]->setDestroy(-1);
	glutTimerFunc(70, destroy2, i);
	glutPostRedisplay();
}

void destroy2(int i)
{
	vehicles[i]->setDestroy(-2);
	glutTimerFunc(70, destroy3, i);
	glutPostRedisplay();
}

void destroy3(int i)
{
	// the animation is finished
	vehicles[i]->setDestroy(-3);
	glutTimerFunc(70, destroy, i);
	glutPostRedisplay();
}

GLuint loadTexture(Image* image) {
	GLuint textureId;
	glGenTextures(1, &textureId); //Make room for our texture
	glBindTexture(GL_TEXTURE_2D, textureId); //Tell OpenGL which texture to edit
	//Map the image to the texture
	glTexImage2D(GL_TEXTURE_2D,			//Always GL_TEXTURE_2D
		0,								//0 for now
		GL_RGB,							//Format OpenGL uses for image
		image->width, image->height,	//Width and height
		0,								//The border of the image
		GL_RGB,							//GL_RGB, because pixels are stored in RGB format
		GL_UNSIGNED_BYTE,				//GL_UNSIGNED_BYTE, because pixels are stored
										//as unsigned numbers
		image->pixels);					//The actual pixel data
	return textureId; //Returns the id of the texture
}

// draws the map
void drawMap()
{
	for (int i = 0; i < WIDTH; i++) {
		for (int j = 0; j < HEIGHT; j++) {
			if (map[i][j].getContents() > 3) {
				// map tile high in minerals
				draw(Grass_h, i, j, 0.0f);
			}
			else {
				// map tile low in minerals
				draw(Grass, i, j, 0.0f);
			}
		}
	}

}

// draws the robots
void drawVehicles()
{
	for (unsigned int i = 0; i < vehicles.size(); i++) {
		if (vehicles[i]->getDestroy() < -3) {} // ready to be destroyed
		else if (vehicles[i]->getTeleport()) { // teleport animation
			draw(Textures[5 + (vehicles[i]->getTeleport())],
				vehicles[i]->getX(), vehicles[i]->getY(), 0.5f);
		}
		else if (vehicles[i]->getDestroy() < 0) { // destroy animation
			draw(Textures[12 + 3 * (int)vehicles[i]->getType() + vehicles[i]->getDestroy()],
				vehicles[i]->getX(), vehicles[i]->getY(), 0.5f);
		}
		else { // vehicle with / without damage
			draw(Textures[(int)vehicles[i]->getType() + 3 * (!vehicles[i]->isOK())],
				vehicles[i]->getX(), vehicles[i]->getY(), 0.5f);
		}
	}
};

// draws the danger flags
void drawFlags() {
	for (int i = 0; i < WIDTH; i++) {
		for (int j = 0; j < HEIGHT; j++) {
			if (map[i][j].isFlagged()) {
				// danger flag texture
				draw(Danger, i, j, 0.5f);
			}
		}
	}
}

// draws the spaceship and prints general info
void drawInfo()
{
	char ch[256];
	// draw the spaceship
	glBindTexture(GL_TEXTURE_2D, Base);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glColor3f(1.0f, 1.0f, 1.0f);

	glBegin(GL_QUADS);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(2.5f, HEIGHT - 1.5f, 0.5f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(WIDTH - 2.5f, HEIGHT - 1.5f, 0.5f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(WIDTH - 2.5f, 1.5f, 0.5f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(2.5f, 1.5f, 0.5f);

	glEnd();

	glPushMatrix();
	glLoadIdentity();
	// print the gathered minerals
	sprintf_s(ch, sizeof(ch), "Iridium [%d / 500]", base_iridium);
	output(3.5f, HEIGHT / 2.0f + 2.4f, 1.0f, 0.5f, 0.3f, 0.2f, GLUT_BITMAP_HELVETICA_18, ch);

	sprintf_s(ch, sizeof(ch), "Platinum [%d / 500]", base_platinum);
	output(3.5f, HEIGHT / 2.0f + 3.4f, 1.0f, 0.2f, 0.5f, 0.3f, GLUT_BITMAP_HELVETICA_18, ch);

	sprintf_s(ch, sizeof(ch), "Palladium [%d / 500]", base_palladium);
	output(3.5f, HEIGHT / 2.0f + 4.4f, 1.0f, 0.3f, 0.2f, 0.5f, GLUT_BITMAP_HELVETICA_18, ch);

	glPopMatrix();
	// draw progress bar for the minerals
	glBegin(GL_QUADS);

	glColor3f(1.0f, 0.0f, 0.0f);

	glVertex3f(7.5f, HEIGHT / 2.0f + 3.0f, 1.0f);
	glVertex3f(7.5f, HEIGHT / 2.0f + 3.6f, 1.0f);
	glColor3f(1.0f, 1.0f, 0.0f);
	glVertex3f(7.6 + 8.5*base_iridium / 500.0f, HEIGHT / 2.0f + 3.6f, 1.0);
	glVertex3f(7.6 + 8.5*base_iridium / 500.0f, HEIGHT / 2.0f + 3.0f, 1.0f);

	glColor3f(0.0f, 1.0f, 0.0f);

	glVertex3f(7.5f, HEIGHT / 2.0f + 4.0f, 1.0f);
	glVertex3f(7.5f, HEIGHT / 2.0f + 4.6f, 1.0f);
	glColor3f(0.0f, 1.0f, 1.0f);
	glVertex3f(7.6 + 8.5*base_platinum / 500.0f, HEIGHT / 2.0f + 4.6f, 1.0);
	glVertex3f(7.6 + 8.5*base_platinum / 500.0f, HEIGHT / 2.0f + 4.0f, 1.0f);

	glColor3f(0.0f, 0.0f, 1.0f);

	glVertex3f(7.5f, HEIGHT / 2.0f + 5.0f, 1.0f);
	glVertex3f(7.5f, HEIGHT / 2.0f + 5.6f, 1.0f);
	glColor3f(1.0f, 0.0f, 1.0f);
	glVertex3f(7.6 + 8.5*base_palladium / 500.0f, HEIGHT / 2.0f + 5.6f, 1.0);
	glVertex3f(7.6 + 8.5*base_palladium / 500.0f, HEIGHT / 2.0f + 5.0f, 1.0f);

	glEnd();
}

// prints the menu
void drawOptions()
{
	glBindTexture(GL_TEXTURE_2D, Options);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glColor3f(1.0f, 1.0f, 1.0f);
	glBegin(GL_QUADS);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(0, HEIGHT + 2, 0.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(WIDTH, HEIGHT + 2, 0.0f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(WIDTH, HEIGHT, 0.0f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0, HEIGHT, 0.0f);

	glEnd();
	glDisable(GL_TEXTURE_2D);
	// print all strings in the menu
	output(8.0f, HEIGHT + 0.1f, 0.5f, 0, 1, 1, GLUT_BITMAP_9_BY_15, cmid);
	output(0.2f, HEIGHT - 0.4f, 0.5f, 1, 1, 1, GLUT_BITMAP_HELVETICA_18, c1);
	output(0.2f, HEIGHT + 0.6f, 0.5f, 1, 1, 1, GLUT_BITMAP_HELVETICA_18, c2);
	output(WIDTH / 2.5f, HEIGHT - 0.4f, 0.5f, 1, 1, 1, GLUT_BITMAP_HELVETICA_18, c3);
	output(WIDTH / 2.5f, HEIGHT + 0.6f, 0.5f, 1, 1, 1, GLUT_BITMAP_HELVETICA_18, c4);
	output(2.1 * WIDTH / 3.0f, HEIGHT - 0.4f, 0.5f, 1, 1, 1, GLUT_BITMAP_HELVETICA_18, c5);
	output(2.1 * WIDTH / 3.0f, HEIGHT + 0.6f, 0.5f, 1, 1, 1, GLUT_BITMAP_HELVETICA_18, c6);
	output(WIDTH - 1, HEIGHT + 0.6f, 0.5f, 0, 1, 1, GLUT_BITMAP_9_BY_15, cmenu);
	glEnable(GL_TEXTURE_2D);
}

// displays success message!
void drawSuccess()
{
	pause = true;

	glBindTexture(GL_TEXTURE_2D, Success);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glColor3f(1.0f, 1.0f, 1.0f);

	glBegin(GL_QUADS);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(2.5f, HEIGHT - 1.5f, 0.5f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(WIDTH - 2.5f, HEIGHT - 1.5f, 0.5f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(WIDTH - 2.5f, 1.5f, 0.5f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(2.5f, 1.5f, 0.5f);

	glEnd();
}

// displays failure message..
void drawFail()
{
	pause = true;

	glBindTexture(GL_TEXTURE_2D, Fail);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glColor3f(1.0f, 1.0f, 1.0f);

	glBegin(GL_QUADS);

	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(2.5f, HEIGHT - 1.5f, 0.5f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(WIDTH - 2.5f, HEIGHT - 1.5f, 0.5f);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(WIDTH - 2.5f, 1.5f, 0.5f);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(2.5f, 1.5f, 0.5f);

	glEnd();
}

// general draw function
void drawScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	// split the window into WIDTH * HEIGHT parts (+2 for the menu)
	glOrtho(0, WIDTH, HEIGHT + 2, 0, -1, 10);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glEnable(GL_TEXTURE_2D);

	if (info) {
		drawInfo();
	}
	else if (finished() == 1) {
		drawSuccess();
	}
	else if (finished() == 2) {
		drawFail();
	}
	// fill the menu string variables
	if (finished() || busy) {
		// empty menu
		fill(c1, c1 + 64, 0); fill(c2, c2 + 64, 0); fill(c3, c3 + 64, 0); fill(c4, c4 + 64, 0);
		fill(c5, c5 + 64, 0); fill(c6, c6 + 64, 0); fill(cmid, cmid + 64, 0); fill(cmenu, cmenu + 8, 0);
	}
	else if (!pause) {
		// default
		sprintf_s(cmid, sizeof(cmid), "Press 'p' to pause");
		fill(c1, c1 + 64, 0); fill(c2, c2 + 64, 0); fill(c3, c3 + 64, 0);
		fill(c4, c4 + 64, 0); fill(c5, c5 + 64, 0); fill(c6, c6 + 64, 0); fill(cmenu, cmenu + 8, 0);
	}
	else if (!VehiclesInfo && !info && !MapInfo && !editVeh && !editMap) {
		// pause menu
		sprintf_s(c1, sizeof(c1), "Press 'i' to view general info");
		sprintf_s(c2, sizeof(c2), "Press 'v' to view vehicles info");
		sprintf_s(c3, sizeof(c3), "Press 'm' to view map info");
		sprintf_s(c4, sizeof(c4), "Press 'e' to edit the vehicles");
		sprintf_s(c5, sizeof(c5), "Press 'r' to edit the map");
		sprintf_s(c6, sizeof(c6), "Press 'p' to resume");
		sprintf_s(cmenu, sizeof(cmenu), "\'p\'");
		fill(cmid, cmid + 64, 0);
	}
	else if (VehiclesInfo) {
		// vehicles info
		if (strcmp(c1, "Click a vehicle to view info!")) {
			fill(c2, c2 + 64, 0); fill(c3, c3 + 64, 0); fill(c4, c4 + 64, 0);
			fill(c5, c5 + 64, 0); fill(c6, c6 + 64, 0); fill(cmid, cmid + 64, 0); fill(cmenu, cmenu + 8, 0);
		}
		sprintf_s(c1, sizeof(c1), "Click a vehicle to view info!");
		sprintf_s(cmenu, sizeof(cmenu), "\'v\'");
	}
	else if (MapInfo) {
		// map info
		if (strcmp(c1, "Click somewhere to view info!")) {
			fill(c2, c2 + 64, 0); fill(c3, c3 + 64, 0); fill(c4, c4 + 64, 0);
			fill(c5, c5 + 64, 0); fill(c6, c6 + 64, 0); fill(cmid, cmid + 64, 0); fill(cmenu, cmenu + 8, 0);
		}
		sprintf_s(c1, sizeof(c1), "Click somewhere to view info!");
		sprintf_s(cmenu, sizeof(cmenu), "\'m\'");
	}
	else if (info) {
		// general info
		sprintf_s(c1, sizeof(c1), "Plat / Irid / Pallad mined: %d / %d / %d",
			RobotMiner::getTotalPlatinum(), RobotMiner::getTotalIridium(), RobotMiner::getTotalPalladium());
		sprintf_s(c2, sizeof(c2), "Plat / Irid / Pallad remaining: %d / %d / %d",
			getRemainingPlatinum(), getRemainingIridium(), getRemainingPalladium());
		sprintf_s(c3, sizeof(c3), "Total Mulfunctions: %d", Vehicles::getTotalFailures());
		sprintf_s(c4, sizeof(c4), "Total Rescues: %d", RobotRescue::getTotalRepairs());
		sprintf_s(c5, sizeof(c5), "Average Danger (0.0-0.9): %.1f", getDanger());
		sprintf_s(c6, sizeof(c6), "Total Flags: %d", RobotExplore::getTotalFlags());
		sprintf_s(cmenu, sizeof(cmenu), "\'i\'");
	}
	else if (editVeh) {
		// edit vehicles
		fill(cmid, cmid + 64, 0);
		sprintf_s(c1, sizeof(c1), "Press '1' to insert Exploration unit");
		sprintf_s(c3, sizeof(c3), "Press '2' to insert Rescue unit");
		sprintf_s(c5, sizeof(c5), "Press '3' to insert Miner unit");
		sprintf_s(c2, sizeof(c2), "Press 'z' to remove a vehicle");
		sprintf_s(c4, sizeof(c4), "Press 'x' to repair/break");
		sprintf_s(c6, sizeof(c6), "Press 'c' to edit ability");
		sprintf_s(cmenu, sizeof(cmenu), "\'e\'");
	}
	else if (editMap) {
		// edit Map
		fill(cmid, cmid + 64, 0);
		sprintf_s(c1, sizeof(c1), "Press '4' to change Palladium levels");
		sprintf_s(c3, sizeof(c3), "Press '5' to change Iridium levels");
		sprintf_s(c5, sizeof(c5), "Press '6' to change Platinum levels");
		sprintf_s(c2, sizeof(c2), "Press 'd' to change danger levels");
		sprintf_s(c4, sizeof(c4), "Press 'f' to randomize!");
		sprintf_s(c6, sizeof(c6), "Press 'g' to put/remove flag");
		sprintf_s(cmenu, sizeof(cmenu), "\'r\'");
	}

	if (busy == ADD_VEHICLE) {
		sprintf_s(cmid, sizeof(cmid), "Click on an empty tile");
		fill(c1, c1 + 64, 0); fill(c2, c2 + 64, 0); fill(c3, c3 + 64, 0);
		fill(c4, c4 + 64, 0); fill(c5, c5 + 64, 0); fill(c6, c6 + 64, 0); fill(cmenu, cmenu + 8, 0);
	}
	else if (busy == REMOVE_VEHICLE || busy == TOGGLE_REPAIR_VEHICLE || busy == EDITING_VEHICLE_ABILITY) {
		sprintf_s(cmid, sizeof(cmid), "Click on a vehicle");
		fill(c1, c1 + 64, 0); fill(c2, c2 + 64, 0); fill(c3, c3 + 64, 0);
		fill(c4, c4 + 64, 0); fill(c5, c5 + 64, 0); fill(c6, c6 + 64, 0); fill(cmenu, cmenu + 8, 0);
	}
	else if (busy == EDIT_VEHICLE_ABILITY || busy == EDIT_MINERAL_LEVELS || busy == EDIT_DANGER_LEVELS) {
		sprintf_s(cmid, sizeof(cmid), "Enter a number (0-9)");
		fill(c1, c1 + 64, 0); fill(c2, c2 + 64, 0); fill(c3, c3 + 64, 0);
		fill(c4, c4 + 64, 0); fill(c5, c5 + 64, 0); fill(c6, c6 + 64, 0); fill(cmenu, cmenu + 8, 0);
	}
	else if (busy == EDITING_MINERAL_LEVELS || busy == EDITING_DANGER_LEVELS ||
			 busy == RANDOMIZE_MAP_TILE || busy == TOGGLE_DANGER_FLAG) {
		sprintf_s(cmid, sizeof(cmid), "Click on a tile");
		fill(c1, c1 + 64, 0); fill(c2, c2 + 64, 0); fill(c3, c3 + 64, 0);
		fill(c4, c4 + 64, 0); fill(c5, c5 + 64, 0); fill(c6, c6 + 64, 0); fill(cmenu, cmenu + 8, 0);
	}

	// draw the window
	drawOptions();
	drawMap();
	drawVehicles();
	drawFlags();



	glDisable(GL_TEXTURE_2D);

	glutSwapBuffers();
}

// window resize
void changeSize(int w, int h)
{
	if (h == 0)
		h = 1;
	float ratio = w * 1.0 / h;
	pixels_h = h;
	pixels_w = w;

	glViewport(0, 0, w, h);

	glMatrixMode(GL_PROJECTION);

	gluPerspective(45, ratio, 0.1f, 1000);
	glLoadIdentity();

	// Get Back to the Modelview
	glMatrixMode(GL_MODELVIEW);
}

// does everything
void theGame(int value)
{
	if (finished() == 1) {
		// success message
		drawSuccess();
	}
	else if (finished() == 2) {
		// failure message
		drawFail();
	}
	else if (!pause) {
		for (unsigned int i = 0; i < vehicles.size(); i++) {
			if (vehicles[i]->getDestroy() < -3) {} // ready to be destroyed
			else if (vehicles[i]->getType() == MINER && ((RobotMiner*)vehicles[i])->isFull()) {
				goToBase(i); // teleport to the spaceship
			}
			else if (!vehicles[i]->isOK()) {
				vehicles[i]->setDestroy(vehicles[i]->getDestroy() + 1); // destruction timer
			}
			else if (!(vehicles[i]->isActive() - vehicles[i]->getSpeed())
					&& vehicles[i]->canMove(map)) {
				// move vehicle
				vehicles[i]->move(map);
				// possibility for failure during movement
				vehicles[i]->failure(map);
			}
			if (vehicles[i]->getDestroy() == 25) {
				// destroy vehicle after 25 turns of being damaged
				destroy1(i);
			}
			else if (!vehicles[i]->getDestroy()) {
				// execute vehicle's role
				vehicles[i]->action(map);
			}
		}
		// destroy animations need 210ms, we give double time for safety reasons
		// delete vehicles with destroy = -4
		glutTimerFunc(420, clearVehicles, 0);
	}
	// repeat after 0.5 seconds
	glutTimerFunc(500, theGame, 0);
	// display
	glutPostRedisplay();
}

void init() {
	// Makes 3D drawing work when something is in front of something else
	glEnable(GL_DEPTH_TEST);
	int i, X, Y;
	// load textures
	Image* image = loadBMP("Grass.bmp");
	Grass = loadTexture(image);
	image = loadBMP("Grass_h.bmp");
	Grass_h = loadTexture(image);
	image = loadBMP("Danger.bmp");
	Danger = loadTexture(image);
	image = loadBMP("Base.bmp");
	Base = loadTexture(image);
	image = loadBMP("Options.bmp");
	Options = loadTexture(image);
	image = loadBMP("Success.bmp");
	Success = loadTexture(image);
	image = loadBMP("Fail.bmp");
	Fail = loadTexture(image);
	image = loadBMP("V1.bmp");
	Textures[0] = loadTexture(image);
	image = loadBMP("V2.bmp");
	Textures[1] = loadTexture(image);
	image = loadBMP("V3.bmp");
	Textures[2] = loadTexture(image);
	image = loadBMP("V1_c.bmp");
	Textures[3] = loadTexture(image);
	image = loadBMP("V2_c.bmp");
	Textures[4] = loadTexture(image);
	image = loadBMP("V3_c.bmp");
	Textures[5] = loadTexture(image);
	image = loadBMP("V1_t1.bmp");
	Textures[6] = loadTexture(image);
	image = loadBMP("V1_t2.bmp");
	Textures[7] = loadTexture(image);
	image = loadBMP("V1_t3.bmp");
	Textures[8] = loadTexture(image);
	image = loadBMP("V1_d3.bmp");
	Textures[9] = loadTexture(image);
	image = loadBMP("V1_d2.bmp");
	Textures[10] = loadTexture(image);
	image = loadBMP("V1_d1.bmp");
	Textures[11] = loadTexture(image);
	image = loadBMP("V2_d3.bmp");
	Textures[12] = loadTexture(image);
	image = loadBMP("V2_d2.bmp");
	Textures[13] = loadTexture(image);
	image = loadBMP("V2_d1.bmp");
	Textures[14] = loadTexture(image);
	image = loadBMP("V3_d3.bmp");
	Textures[15] = loadTexture(image);
	image = loadBMP("V3_d2.bmp");
	Textures[16] = loadTexture(image);
	image = loadBMP("V3_d1.bmp");
	Textures[17] = loadTexture(image);
	delete image;
	// random coordinates for all vehicles
	for (i = 0; i < 10; i++) {
		do {
			X = rand() % WIDTH;
			Y = rand() % HEIGHT;
		} while (map[X][Y].isOccupied());

		RobotMiner *Veh1 = new RobotMiner();
		map[X][Y].setOccupied(Veh1);
		Veh1->setX(X);
		Veh1->setY(Y);
		vehicles.push_back(Veh1);
	}
	for (int i = 0; i < 10; i++) {
		do {
			X = rand() % WIDTH;
			Y = rand() % HEIGHT;
		} while (map[X][Y].isOccupied());

		RobotExplore *Veh2 = new RobotExplore();
		map[X][Y].setOccupied(Veh2);
		Veh2->setX(X);
		Veh2->setY(Y);
		vehicles.push_back(Veh2);
	}
	for (int i = 0; i < 15; i++) {
		do {
			X = rand() % WIDTH;
			Y = rand() % HEIGHT;
		} while (map[X][Y].isOccupied());

		RobotRescue *Veh3 = new RobotRescue();
		map[X][Y].setOccupied(Veh3);
		Veh3->setX(X);
		Veh3->setY(Y);
		vehicles.push_back(Veh3);
	}
}

int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	// window size
	glutInitWindowSize(pixels_w, pixels_h);
	// relevant window position
	glutInitWindowPosition(500, 100);
	// random seed
	srand(time(NULL));

	// window title!
	glutCreateWindow("PlanetX");
	// initialize textures and vehicles
	init();


	// Set handler functions for drawing, keypresses, and window resizes
	glutDisplayFunc(drawScene);
	glutKeyboardFunc(handleKeypress);
	// ignore continuous press of a button
	glutIgnoreKeyRepeat(1);
	glutMouseFunc(mouseClick);
	glutReshapeFunc(changeSize);
	glutTimerFunc(500, theGame, 0);


	glutMainLoop();
	return 0;
}
//This program simulates a predator and prey cellular automata relationship
//The rules are as follows:

//two organisms: 1. prey, 2. predator
//prey:
//move a direction, increment life, if life equals certain number, reproduce in random direction
//predator:
//move a direction, decrement life, if life equals 0... die, if touching a prey, convert it to predator

//The program also features a cell counter and visual representation of the cells as they go through generations

//Programmed by Nick Khouri
//4.10.18


#include <SFML\Graphics.hpp>
#include <iostream>
#include <ctime>
#include <chrono>
#include <thread>

using namespace std;
using namespace sf;

const int WIDTH = 1920;
const int HEIGHT = 1080;
const int RES = 10;
const int GWIDTH = HEIGHT / RES;
const int GHEIGHT = WIDTH / RES;
const int MAXLIFE = 200;

struct organism {
	int type;
	int life;
};

void initializeGrid(organism grid[GWIDTH][GHEIGHT]);
void clearGrid(organism grid[GWIDTH][GHEIGHT]);
void randomMove(organism grid[GWIDTH][GHEIGHT], int x, int y);
void reproduce(organism grid[GWIDTH][GHEIGHT], int x, int y);
void handleLifeAndDeath(organism grid[GWIDTH][GHEIGHT]);
void eatPrey(organism grid[GWIDTH][GHEIGHT], int x, int y);
int countCells(organism grid[GWIDTH][GHEIGHT], int type);


int main() {
	RenderWindow window(VideoMode(WIDTH, HEIGHT), "Predator and Prey v2");
	organism grid[GWIDTH][GHEIGHT];
	clearGrid(grid);
	initializeGrid(grid);
	srand(time(NULL));	//new seed
	int preyCount, predatorCount;

	sf::Font font;
	if (!font.loadFromFile("arial.ttf")) {
		cout << "ERROR\n";
	}

	//order of action: prey, predator, update

	int c = 0;

	//make rectangels to show scatter plot of the predator and prey
	RectangleShape preyP[1000];
	RectangleShape predatorP[1000];
	int lineCount = 0;

	while (window.isOpen()) {
		//start with prey movement
		for (int i = 1; i < GWIDTH - 1; i++) {
			for (int j = 1; j < GHEIGHT - 1; j++) {
				if (grid[i][j].type == 1)
					randomMove(grid, i, j);
			}
		}
		
		//have the predator eat
		for (int i = 1; i < GWIDTH - 1; i++) {
			for (int j = 1; j < GHEIGHT - 1; j++) {
				if (grid[i][j].type == 2) {
					eatPrey(grid, i, j);
				}
			}
		}

		//start with predator movement
		for (int i = 1; i < GWIDTH - 1; i++) {
			for (int j = 1; j < GHEIGHT - 1; j++) {
				if (grid[i][j].type == 2)
					randomMove(grid, i, j);
			}
		}

		//have the predator eat
		for (int i = 1; i < GWIDTH - 1; i++) {
			for (int j = 1; j < GHEIGHT - 1; j++) {
				if (grid[i][j].type == 2) {
					eatPrey(grid, i, j);
				}
			}
		}

		handleLifeAndDeath(grid);

		//calculate some statistics
		preyCount = countCells(grid, 1);
		predatorCount = countCells(grid, 2);

		//calculate the box plot positions
		if (lineCount >= 1000) {
			lineCount = 0;
		}
		preyP[lineCount].setSize(Vector2f(2, 2));
		preyP[lineCount].setPosition(Vector2f((RES * 20) + lineCount, 40 - (preyCount / 200)));
		preyP[lineCount].setFillColor(Color::Green);

		predatorP[lineCount].setSize(Vector2f(2, 2));
		predatorP[lineCount].setPosition(Vector2f((RES * 20) + lineCount, 40 - (predatorCount / 300)));
		predatorP[lineCount].setFillColor(Color::Red);
		lineCount++;


		window.clear();
		for (int i = 0; i < GWIDTH; i++) {
			for (int j = 0; j < GHEIGHT; j++) {
				if (grid[i][j].type == 1) {
					RectangleShape prey(Vector2f(RES, RES));
					prey.setFillColor(Color(0, 255, 0));
					prey.setPosition(j*RES, i*RES);
					window.draw(prey);
				}
				else if (grid[i][j].type == 2) {
					RectangleShape predator(Vector2f(RES, RES));
					predator.setFillColor(Color(255, 0, 0));
					predator.setPosition(j*RES, i*RES);
					window.draw(predator);
				}
			}
		}
		Text text;
		text.setFont(font);
		text.setString("Prey Count: " + to_string(preyCount));
		text.setCharacterSize(16);
		text.setFillColor(Color::Green);
		text.setPosition(RES, 0);

		Text text2;
		text2.setFont(font);
		text2.setString("Predator Count: " + to_string(predatorCount));
		text2.setCharacterSize(16);
		text2.setFillColor(Color::Red);
		text2.setPosition(RES, 16);

		Text text3;
		text3.setFont(font);
		text3.setString("Generation: " + to_string(c));
		text3.setCharacterSize(16);
		text3.setFillColor(Color::Blue);
		text3.setPosition(RES, 32);

		RectangleShape bg(Vector2f(GHEIGHT*RES, 5*RES));
		bg.setFillColor(Color::White);
		bg.setPosition(0, 0);

		window.draw(bg);
		window.draw(text);
		window.draw(text2);
		window.draw(text3);

		for (int i = 0; i < lineCount; i++) {
			window.draw(preyP[i]);
		}
		for (int i = 0; i < lineCount; i++) {
			window.draw(predatorP[i]);
		}

		window.display();
		c++;
	}
}

void initializeGrid(organism grid[GWIDTH][GHEIGHT]) {
	for (int i = 0; i < GWIDTH; i++) {
		for (int j = 0; j < GHEIGHT; j++) {
			int r = rand() % 1000;
			if (r < 5) {
				grid[i][j].type = 1;
				grid[i][j].life = (rand() % MAXLIFE-(MAXLIFE/5)) + 1;
			}
			else if (r >= 5 && r < 6) {
				grid[i][j].type = 2;
				grid[i][j].life = MAXLIFE;
			}
		}
	}
}

void clearGrid(organism grid[GWIDTH][GHEIGHT]) {
	for (int i = 0; i < GWIDTH; i++) {
		for (int j = 0; j < GHEIGHT; j++) {
			grid[i][j].type = 0;
			grid[i][j].life = -1;
		}
	}
}

void randomMove(organism grid[GWIDTH][GHEIGHT], int x, int y) {
	int eligableMove[4];
	int count = 0;

	if (grid[x][y + 1].type == 0) {
		eligableMove[count] = 1;
		count++;
	}
	if (grid[x + 1][y].type == 0) {
		eligableMove[count] = 2;
		count++;
	}
	if (grid[x][y - 1].type == 0) {
		eligableMove[count] = 3;
		count++;
	}
	if (grid[x - 1][y].type == 0) {
		eligableMove[count] = 4;
		count++;
	}

	if (count != 0) {
		int pick = rand() % count;
		int chosen = eligableMove[pick];

		if (chosen == 1) {
			grid[x][y + 1].type = grid[x][y].type;
			grid[x][y + 1].life = grid[x][y].life;
			grid[x][y].type = 0;
			grid[x][y].life = -1;
		}
		else if (chosen == 2) {
			grid[x + 1][y].type = grid[x][y].type;
			grid[x + 1][y].life = grid[x][y].life;
			grid[x][y].type = 0;
			grid[x][y].life = -1;
		}
		else if (chosen == 3) {
			grid[x][y - 1].type = grid[x][y].type;
			grid[x][y - 1].life = grid[x][y].life;
			grid[x][y].type = 0;
			grid[x][y].life = -1;
		}
		else if (chosen == 4) {
			grid[x - 1][y].type = grid[x][y].type;
			grid[x - 1][y].life = grid[x][y].life;
			grid[x][y].type = 0;
			grid[x][y].life = -1;
		}
	}
	//if cannot move, then dont
}

void reproduce(organism grid[GWIDTH][GHEIGHT], int x, int y) {
	int count = 0;
	int numDir = 0;
	int availableDirections[8];

	for (int i = x - 1; i < x + 2; i++) {
		for (int j = y - 1; j < y + 2; j++) {
			if (i == x && j == y) {
				//do nothing
			}
			else if (grid[i][j].type == 0) {
				availableDirections[count] = numDir;
				count++;
				numDir++;
			}
			else {
				numDir++;
			}
		}
	}

	//pick a random index from the array
	//place the organism into the grid
	if (count != 0) {
		int choice = availableDirections[rand() % count];

		if (choice = 0) {
			grid[x - 1][y - 1].type = 1;
			grid[x - 1][y - 1].life = 0;
		}
		else if (choice = 1) {
			grid[x - 1][y].type = 1;
			grid[x - 1][y].life = 0;
		}
		else if (choice = 2) {
			grid[x - 1][y + 1].type = 1;
			grid[x - 1][y + 1].life = 0;
		}
		else if (choice = 3) {
			grid[x][y - 1].type = 1;
			grid[x][y - 1].life = 0;
		}
		else if (choice = 4) {
			grid[x][y + 1].type = 1;
			grid[x][y + 1].life = 0;
		}
		else if (choice = 5) {
			grid[x + 1][y - 1].type = 1;
			grid[x + 1][y - 1].life = 0;
		}
		else if (choice = 6) {
			grid[x + 1][y].type = 1;
			grid[x + 1][y].life = 0;
		}
		else if (choice = 7) {
			grid[x + 1][y + 1].type = 1;
			grid[x + 1][y + 1].life = 0;
		}
	}
}

void handleLifeAndDeath(organism grid[GWIDTH][GHEIGHT]) {
	for (int i = 1; i < GWIDTH - 1; i++) {
		for (int j = 1; j < GHEIGHT - 1; j++) {
			if (grid[i][j].type == 1) {
				grid[i][j].life++;
			}
			else if (grid[i][j].type == 2) {
				grid[i][j].life--;
			}
		}
	}


	for (int i = 1; i < GWIDTH - 1; i++) {
		for (int j = 1; j < GHEIGHT - 1; j++) {
			if (grid[i][j].type == 1 && grid[i][j].life >= MAXLIFE) {
				reproduce(grid, i, j);
			}
			else if (grid[i][j].type == 1 && grid[i][j].life != MAXLIFE) {
				grid[i][j].life++;
			}
			else if (grid[i][j].type == 2 && grid[i][j].life <= 0) {
				grid[i][j].type = 0;
				grid[i][j].life = -1;
			}
			else if (grid[i][j].type == 2 && grid[i][j].life > 0) {
				grid[i][j].life--;
			}
		}
	}
}

//EAT PREY
//in: cell grid, x coordinate, y coordinate
//out: pass by cell grid with updated cells
void eatPrey(organism grid[GWIDTH][GHEIGHT], int x, int y) {
	int count = 0;
	int numDir = 0;
	int availableDirections[8];

	for (int i = x - 1; i < x + 2; i++) {
		for (int j = y - 1; j < y + 2; j++) {
			if (i == x && j == y) {
				//do nothing
			}
 			else if (grid[i][j].type == 1) {
				availableDirections[count] = numDir;
				count++;
				numDir++;
			}
			else {
				numDir++;
			}
		}
	}

	if (count != 0) {
		int choice = availableDirections[rand() % count];

		if (choice = 0) {
			grid[x - 1][y - 1].type = 2;
		}
		else if (choice = 1) {
			grid[x - 1][y].type = 2;
		}
		else if (choice = 2) {
			grid[x - 1][y + 1].type = 2;
		}
		else if (choice = 3) {
			grid[x][y - 1].type = 2;
		}
		else if (choice = 4) {
			grid[x][y + 1].type = 2;
		}
		else if (choice = 5) {
			grid[x + 1][y - 1].type = 2;
		}
		else if (choice = 6) {
			grid[x + 1][y].type = 2;
		}
		else if (choice = 7) {
			grid[x + 1][y + 1].type = 2;
		}
	}
	//if there is no possible way to eat, then do not
}

//COUNT CELLS
//in: cell grid, organism type to count
//out: int count of the organism type
int countCells(organism grid[GWIDTH][GHEIGHT], int type) {
	int count = 0;
	
	for (int i = 1; i < GWIDTH - 1; i++) {
		for (int j = 1; j < GHEIGHT - 1; j++) {
			if (grid[i][j].type == type)
				count++;
		}
	}
	return count;
}
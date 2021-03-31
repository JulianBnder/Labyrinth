#include <stdio.h>
#include <windows.h>
#include <stdbool.h>
#include <time.h>
#include <stdlib.h>



void main()
{
	// parameters: speed in units per second, startposition x, startposition y
	//use (0|0) to change it in the console
	return(pseudomain(40, 4, 4));
}

pseudomain(int sleeptemp, int tempX, int tempY)
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE); // initialising console output
	COORD position = { .X = tempX, .Y = tempY }; // sets the starting position for the robot
	int direction = 0; // 0 = up; 1 = right; 2 = down; 3 = left
	int sleeptime = 1000 / sleeptemp; // defines how long the robot stays in a position
	int** field;
	int sizeX = 0;
	int sizeY = 0;


	if (readLabyrinth(&field, &sizeX, &sizeY) != 0) // reads the Labyrinth from the file into the array
	{
		// sets the Coursor below the Labyrinth to not damage it
		position.X = 0;
		position.Y = sizeY + 5;
		SetConsoleCursorPosition(hConsole, position);
		return(-1);
	}
	showLabyrinth(field, sizeX, sizeY); //shows the laborinth on the console
	do //if the start-coordinates aren't both 0  it only checks for wrong values
	{
		placeRobot(&position, hConsole, sizeX, sizeY, field);
	} while (position.X == 0 && position.Y == 0);
	showRobot(&position, hConsole, direction);


	solveLabyrinth(&position, hConsole, &direction, sleeptime, sizeX, sizeY, field);


	//frees the storage space for the Labyrinth
	for (int i = 0; i < sizeX; i++)
	{
		free(field[i]);
	}
	free(field);


	//sets the Coursor below the Labyrinth to not damage it
	position.X = 0;
	position.Y = sizeY + 5;
	SetConsoleCursorPosition(hConsole, position);

	return(0);
}

readLabyrinth(int*** field, int* sizeX, int* sizeY)
{
	int** fieldTemp; //Temporary pointer for malloc
	FILE* fp; // pointer to the file to be read

	// puts the pointer to the start of the text in the file, "r" means read only mode
	fopen_s(&fp, "Labyrinth.txt", "r");

	//error if the file couldn't be opened
	if (fp == NULL)
	{
		printf("file couldn't be read.");
		return(-1);
	}

	// dimensions of the labyrinth are read in as numbers
	fscanf_s(fp, "%d ", sizeX);
	fscanf_s(fp, "%d\n", sizeY);

	// checks if the labyrinth is too small to find a solution that makes sense
	if (*sizeX <= 2 || *sizeY <= 2)
	{
		printf("X- and/or Y-values are too small\n");
		return(-2);
	}
	// checks if the laborinth is big enough to cause problems (not implemented yet
	else if (false)
	{
		printf("X- and/or Y-values are too big\n");
		return(-3);
	}


	//reserves storage space for the Array-Array and for the Arrays in the Array
	fieldTemp = (int**)malloc(*sizeX * sizeof(int*));
	for (int i = 0; i < *sizeX; i++)
	{
		fieldTemp[i] = (int*)malloc((*sizeY) * sizeof(int));
	}

	for (int i = 0; i < *sizeY; i++) // goes though the laborinth
	{
		for (int k = 0; k < *sizeX; k++)
		{
			fieldTemp[k][i] = ((int)fgetc(fp) - 48); // writh labyrinth from file into array
			fgetc(fp); // ignore spaces (and line changes) in the file (data format specified in assignment)
		}
	}

	*field = fieldTemp; //points the general pointer/array to the memory
	fclose(fp); //closes the file
	return(0);
}

showLabyrinth(int** field, int sizeX, int sizeY)
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE); //initialises console output

	for (int i = 0; i < sizeY; i++) //shows the labyrinth on the console
	{
		for (int k = 0; k < sizeX; k++)
		{
			if (field[k][i] == 1) // for walls:
			{
				SetConsoleTextAttribute(hConsole, BACKGROUND_BLUE | BACKGROUND_GREEN); //sets background to cyan
				printf(" ");
			}
			else // for corridors:
			{
				//sets background to black
				SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN);

				if (field[k][i] == 40) // 40 = ((int)"X") - 48
				{
					printf("X");
				}
				else
				{
					printf(" ");
				}
			}
		}
		printf("\n");
	}
	//resets background to black
	SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN);
}

placeRobot(COORD* position, HANDLE hConsole, int sizeX, int sizeY, int** field)
{
	COORD unterFeld; //courser for text
	int temp; //temporary variable, because scanf_s doesn't work with position.X and .Y
	unterFeld.X = 0;
	unterFeld.Y = sizeY + 4; //sets the Coursor below the Labyrinth to not damage it
	SetConsoleCursorPosition(hConsole, unterFeld);
	if (position->X == 0 && position->Y == 0) //if the values were wrong or the user wants to input over console
	{
		printf("where should the robot start? first X(right), space an then Y-value(down)\n");
		scanf_s("%d ", &temp); //writes the 1. value in temp
		position->X = temp;  //sets Posioion.X equal to the value in temp
		scanf_s("%d", &temp); // writes the 2. value in temp
		position->Y = temp; // sets Posioion.Y equal to the value in temp
	}
	unterFeld.Y = sizeY + 2; //sets the coursor above the first question
	SetConsoleCursorPosition(hConsole, unterFeld);

	//checks if the the robot is being spawned outside the labyrinth
	if (position->X < 0 || position->Y < 0 || position->X > sizeX || position->Y > sizeY)
	{
		printf("\n the robot got spawned outside the Labyrinth\n");
		position->X = 0;
		position->Y = 0;
	}
	//checks if the robot is being spawned in a wall
	else if (field[position->X][position->Y] == 1)
	{
		printf("\n the robot got spawned in a wall\n");
		position->X = 0;
		position->Y = 0;
	}
	else
	{
		return(0);
	}
}

solveLabyrinth(COORD* position, HANDLE hConsole, int* direction, int sleeptime, int sizeX, int sizeY, int** field)
{
	// so that the starting point is also part of the trail, not seen as a wall because moveRobot subracts one
	field[position->X][position->Y] = 1;

	// 40 = (int)"X" - 48 which is the destination
	while (field[position->X][position->Y] != 40)
	{
		//in moveRobot the robot gets deleted in it's old position
		moveRobot(position, hConsole, direction, sizeX, sizeY, field);
		showRobot(position, hConsole, *direction);
		Sleep(sleeptime); //lets the robot stay in a place for [sleeptime] ms
	}
}

unshowRobot(COORD* position, HANDLE hConsole, int** field, BOOL trail)
{
	if (trail) //when the space the robot is on is part of the shortest path
	{
		SetConsoleTextAttribute(hConsole, BACKGROUND_RED); //sets background to red
	}
	// resets the position to the robot after if got changed through writing
	SetConsoleCursorPosition(hConsole, *position);
	printf(" ");
	//resets background to black
	SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN);
}

moveRobot(COORD* position, HANDLE hConsole, int* direction, int sizeX, int sizeY, int** field)
{
	COORD tempCoord = { .X = 0, .Y = 0 }; // temporary coordinate for movement
	_Bool trail; // if a trail should be shown
	int ways = 0; // counter for the paths next to the current place of the robot


	// counts the paths next to the current place of the robot
	if (field[position->X - 1][position->Y] != 1) //checks if there's a path to the left
	{
		ways++;
	}
	if (field[position->X + 1][position->Y] != 1) //checks if there's a path to the right
	{
		ways++;
	}
	if (field[position->X][position->Y - 1] != 1) //checks if there's a path upwards
	{
		ways++;
	}
	if (field[position->X][position->Y + 1] != 1) //checks if there's a path downwards
	{
		ways++;
	}

	// counts how often the robot was on a space
	field[position->X][position->Y]--;
	trail = true; // generally the robot leaves a trail
	// except he was there as often as there are paths next to the current space, 
	//then it has to go back to where it came from and it was a dead end
	if (-(field[position->X][position->Y]) == ways)
	{
		trail = false;
	}
	//could be written as trail = !(-(--field[position->X][position->Y]) >= ways);

	//turns the robot 180 degrees so it looks right  after the first turn left
	*direction = (*direction + 2) % 4;
	while (field[tempCoord.X][tempCoord.Y] == 1)
	{
		*direction = (*direction + 3) % 4; //turns the robot to the left
		switch (*direction) // sets the tempCoord depeding on the direction
		{
		case 0: //robot looks up
		{
			tempCoord.X = position->X;
			tempCoord.Y = position->Y - 1;
			break;
		}
		case 1: //robot looks right
		{
			tempCoord.X = position->X + 1;
			tempCoord.Y = position->Y;
			break;
		}
		case 2: //robot looks down
		{
			tempCoord.X = position->X;
			tempCoord.Y = position->Y + 1;
			break;
		}
		case 3: //robot looks left
		{
			tempCoord.X = position->X - 1;
			tempCoord.Y = position->Y;
			break;
		}
		default:
			break;
		}
	};

	unshowRobot(position, hConsole, field, trail);
	position->X = tempCoord.X;
	position->Y = tempCoord.Y;
}

showRobot(COORD* position, HANDLE hConsole, int direction)
{
	SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN);
	SetConsoleCursorPosition(hConsole, *position);
	// shows different symbols based on direction
	switch (direction)
	{
	case 0: //robot looks up
	{
		printf("A");
		break;
	}
	case 1: //robot looks right
	{
		printf(">");
		break;
	}
	case 2: //robot looks down
	{
		printf("V");
		break;
	}
	case 3: //robot looks left
	{
		printf("<");
		break;
	}
	default:
		printf("Error: invalid direction");
		return(-1);
		break;
	}
}

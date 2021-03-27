#include <stdio.h>
#include <windows.h>
#include <stdbool.h>
#include <time.h>
//#include <stdlib.h>


//void leseSpielfeld(char*** feld, int* sizeX, int* sizeY);
//void ausgabeSpielfeld(char** feld, int sizeX, int sizeY);

/*
in main bei der Spielgrößenabfrage checken, ab wann es Probleme gibt und dann ein oberes Limit einbauen (false ersetzen)
*/
void main() {
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD position;
	char** feld;
	int sizeX;
	int sizeY;

	leseSpielfeld(&feld, &sizeX, &sizeY); //liest das Spielfeld ein
	if (sizeX <= 2 || sizeY <= 2)// checkt, ab das Spielfeld zu klein ist, um es sinnvoll zu lösen
	{
		printf("X- und/oder Y-Werte sind zu klein\n");
		return(-1);
	}
	else if (false) // checkt, ob das Spielfeld so groß ist, dass es Probleme verursacht
	{
		printf("X- und/oder Y-Werte sind zu groß\n");
		return(-1);
	}
	ausgabeSpielfeld(feld, sizeX, sizeY); //gibt das Spielfeld aus
	platziereRobo(position);
	zeigeRobo();
	löseLabyrinth();
}

void leseSpielfeld(char*** feld, int* sizeX, int* sizeY)
{
	int temp;

	FILE* fp;

	fopen_s(&fp, "Labyrinth2.0.txt", "r");

	if (fp == NULL) printf("Datei konnte nicht gelesen werden.");

	else
	{
		fscanf_s(fp, "%d ", sizeX);
		fscanf_s(fp, "%d\n", sizeY);



		
		char** feldTemp; //Speicher für Feld reservieren

		feldTemp = (char**)malloc(*sizeY * sizeof(char*));
		for (int i = 0; i < *sizeY; i++)
		{
			feldTemp[i] = (char*)malloc((*sizeX) * sizeof(char));

			
			for (int k = 0; k < *sizeX; k++) //Spielfeld aus Datei in Array schreiben
			{
				feldTemp[i][k] = (char)fgetc(fp);
				temp = fgetc(fp);
			}

		}
		*feld = feldTemp; //Die Ausgelesenen Werte in das Übergebene Array schreiben
	}
	fclose(fp);
}
void ausgabeSpielfeld(char** feld, int sizeX, int sizeY)
{

	printf("%d sizeX\n", sizeX);
	printf("%d sizeY\n", sizeY);
	HANDLE hConsole;
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	for (int i = 0; i < sizeY; i++)
	{
		for (int k = 0; k < sizeX; k++)
		{
			if (feld[i][k] == '1')
			{
				SetConsoleTextAttribute(hConsole, BACKGROUND_BLUE | BACKGROUND_GREEN);
				printf(" ");
			}
			else
			{
				SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN);
				printf(" ");
			}
		}
		printf("\n");
	}
	SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN);
}
platziereRobo(*COORD)
{
	SetConsoleCursorPosition(hConsole, position);
	printf("Wo soll der R");
	scanf();
}
löseLabyrinth()
{
	while (/*nicht am Rand*/)
	{
		bewegeRobo();
		zeigeRobo)();
	}
}
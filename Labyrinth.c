#include <stdio.h>
#include <windows.h>
#include <stdbool.h>
#include <time.h>
//#include <stdlib.h>


void leseSpielfeld(char*** feld, int* sizeX, int* sizeY);
void ausgabeSpielfeld(char** feld, int sizeX, int sizeY);

/*
in main bei der Spielgrößenabfrage checken, ab wann es Probleme gibt und dann ein oberes Limit einbauen (false ersetzen)
Wenn noch Zeit ist in main beim Funktiosaufruf von platziereRobo und in PlatziereRobo ändern, sodass es nach neuem Input fragt
	um der Funktionsaufruf das: //while (position.X == 0 && position.Y == 0) //gibt die Möglichkeit die Position neu zu wählen, wenn sie falsch gesetzt wurde
	oder noch besser while Schleife in der Funktion
Wenn noch Zeit ist die Sleeptime in der Konsole abfragen
*/
void main() {
	int sleeptime = 500; //legt fest wie lange er in einer Position ist
	int richtung = 0; //die Richtung, in die der  Roboter schaut
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD position;
	position.Y = 0;
	position.X = 0;
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
	platziereRobo(&position, hConsole, sizeX, sizeY, feld); //fragt den Benutzer, wo der Roboter anfangen soll
	zeigeRobo();
	löseLabyrinth(sleeptime);
	for (int i = 0; i < sizeY; i++) //befreit den Speicherplatz des Feldes
	{
		free(feld[i]);
	}
	free(feld);
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

		feldTemp = (char**)malloc(*sizeY * sizeof(char*)); //reserviert Speicherplatz für den Array-Array
		for (int i = 0; i < *sizeX; i++)
		{
			feldTemp[i] = (char*)malloc((*sizeY) * sizeof(char)); //reserviert Speicherplatz für die Arrays im Array
		}
		for (int i = 0; i < *sizeY; i++)
		{
			for (int k = 0; k < *sizeX; k++)
			{
				feldTemp[k][i] = (char)fgetc(fp); //Spielfeld aus Datei in Array schreiben
				temp = fgetc(fp); //Leerzeichen aus der Datei in temp schreiben
			}

		}
		*feld = feldTemp; //Die Ausgelesenen Werte in das Übergebene Array schreiben
	}
	fclose(fp);
}
void ausgabeSpielfeld(char** feld, int sizeX, int sizeY)
{
	HANDLE hConsole;
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	for (int i = 0; i < sizeY; i++) //
	{
		for (int k = 0; k < sizeX; k++)
		{
			if (feld[k][i] == '1')
			{
				SetConsoleTextAttribute(hConsole, BACKGROUND_BLUE | BACKGROUND_GREEN); //Hintergrund auf Türkis setzen
				printf(" ");
			}
			else
			{
				SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN); //Vordergrund auf Schwarz setzen
				printf(" ");
			}
		}
		printf("\n");
	}
	SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN); //Vordergrund auf Schwarz setzen
}
löseLabyrinth(int sleeptime)
{
	while (/*nicht am Rand*/true)
	{
		bewegeRobo();
		zeigeRobo();
		sleep(sleeptime);
	}
}
int platziereRobo(COORD* position, HANDLE hConsole, int sizeX, int sizeY, char** feld)
{
	int temp; //temporärvariable, weil scanf_s mit position.X und.Ynicht geht
	COORD unterFeld; //Courser für Text
	unterFeld.X = 0;
	unterFeld.Y = sizeY + 2; //setzt den Cursor unter das Spielfeld, um es nicht zu beschädigen
	SetConsoleCursorPosition(hConsole, unterFeld);
	printf("Wo soll der Roboter starten? erst X(rechts), Leerzeichen und dann Y-Wert(runter)\n");
	scanf_s("%d ", &temp); //schreibt den 1. eingegebenen Wert in temp
	position->X = temp;  //setzt Posioion.X gleich dem Wert temp
	scanf_s("%d", &temp); // schreibt den 2. eingegebenen Wert für in temp
	position->Y = temp; // setzt Posioion.Y gleich dem Wert temp


	if (position->X < 0 || position->Y < 0 || position->X > sizeX || position->Y > sizeY) //checkt, ob der Roboter auserhalb der Spielfeldes gesetzt werden soll
	{
		printf("\nder Roboter wurde auserhalb des Spielfeldes gespawnt\n");
		return(-1);
	}
	else if (feld[position->X][position->Y] ==  '1') //checkt, ob der Robotor auf einer Wand gespawnt wird
	{
		printf("\nder Roboter wurde in einer Wand gespawnt\n");
		return(-1);

	}
	
	unterFeld.Y = sizeY + 10; //setzt den Cursor unter das Spielfeld, um es nicht zu beschädigen
	SetConsoleCursorPosition(hConsole, unterFeld);
	//printf("                                                                                    \n");
	//printf("                                                                                    \n");
	//printf("                                                                                    \n");
	//printf("                                                                                    \n");
	//printf("                                                                                    \n");
}
bewegeRobo()
{}
zeigeRobo(COORD* position, HANDLE hConsole, int richtung)
{
	SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN);
	SetConsoleCursorPosition(hConsole, *position);
	switch (richtung)
	{
	case:
	default:
		break;
	}
	printf("A");

}
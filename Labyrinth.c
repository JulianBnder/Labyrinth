#include <stdio.h>
#include <windows.h>
#include <stdbool.h>
#include <time.h>
#include <stdlib.h>

HANDLE hConsole;
COORD debugCoord;
void leseSpielfeld(char*** feld, int* sizeX, int* sizeY);
void ausgabeSpielfeld(char** feld, int sizeX, int sizeY);

/*
in main bei der Spielgr��enabfrage checken, ab wann es Probleme gibt und dann ein oberes Limit einbauen (false ersetzen)
Wenn noch Zeit ist in main beim Funktiosaufruf von platziereRobo und in PlatziereRobo �ndern, sodass es nach neuem Input fragt
	um der Funktionsaufruf das: //while (position.X == 0 && position.Y == 0) //gibt die M�glichkeit die Position neu zu w�hlen, wenn sie falsch gesetzt wurde
	oder noch besser while Schleife in der Funktion
Wenn noch Zeit ist die Sleeptime in der Konsole abfragen
*/
void main() {
	int sleeptime = 200; //legt fest wie lange er in einer Position ist
	int richtung = 0; //die Richtung, in die der  Roboter schaut
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD position;
	position.Y = 0;
	position.X = 0;
	char** feld;
	int sizeX;
	int sizeY;

	leseSpielfeld(&feld, &sizeX, &sizeY); //liest das Spielfeld ein
	if (sizeX <= 2 || sizeY <= 2)// checkt, ab das Spielfeld zu klein ist, um es sinnvoll zu l�sen
	{
		printf("X- und/oder Y-Werte sind zu klein\n");
		return(-1);
	}
	else if (false) // checkt, ob das Spielfeld so gro� ist, dass es Probleme verursacht
	{
		printf("X- und/oder Y-Werte sind zu gro�\n");
		return(-1);
	}
	ausgabeSpielfeld(feld, sizeX, sizeY); //gibt das Spielfeld aus
	platziereRobo(&position, hConsole, sizeX, sizeY, feld); //fragt den Benutzer, wo der Roboter anfangen soll
	zeigeRobo(&position, hConsole, richtung);
	l�seLabyrinth(&position, hConsole, &richtung, sleeptime, sizeX, sizeY, feld);
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

	fopen_s(&fp, "Labyrinth.txt", "r");

	if (fp == NULL) printf("Datei konnte nicht gelesen werden.");

	else
	{
		fscanf_s(fp, "%d ", sizeX);
		fscanf_s(fp, "%d\n", sizeY);




		char** feldTemp; //Speicher f�r Feld reservieren

		feldTemp = (char**)malloc(*sizeY * sizeof(char*)); //reserviert Speicherplatz f�r den Array-Array
		for (int i = 0; i < *sizeX; i++)
		{
			feldTemp[i] = (char*)malloc((*sizeY) * sizeof(char)); //reserviert Speicherplatz f�r die Arrays im Array
		}
		for (int i = 0; i < *sizeY; i++)
		{
			for (int k = 0; k < *sizeX; k++)
			{
				feldTemp[k][i] = (char)fgetc(fp); //Spielfeld aus Datei in Array schreiben
				temp = fgetc(fp); //Leerzeichen aus der Datei in temp schreiben
			}

		}
		*feld = feldTemp; //Die Ausgelesenen Werte in das �bergebene Array schreiben
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
				SetConsoleTextAttribute(hConsole, BACKGROUND_BLUE | BACKGROUND_GREEN); //Hintergrund auf T�rkis setzen
				printf(" ");
			}
			else if (feld[k][i] == 'X')
			{
				SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN);
				printf("X");
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
l�seLabyrinth(COORD* position, HANDLE hConsole, int *richtung, int sleeptime, int sizeX, int sizeY, char** feld)
{
	while (feld[position->X][position->Y] != 'X')
	{
		bewegeRobo(position, hConsole, richtung, sizeX, sizeY, feld);
		zeigeRobo(position, hConsole, *richtung);
		Sleep(sleeptime); //l�sst den Roboter f�r Sleeptime ms in einer Stelle verweilen
	}
}
int platziereRobo(COORD* position, HANDLE hConsole, int sizeX, int sizeY, char** feld)
{
	int temp; //tempor�rvariable, weil scanf_s mit position.X und.Ynicht geht
	COORD unterFeld; //Courser f�r Text
	unterFeld.X = 0;
	unterFeld.Y = sizeY + 2; //setzt den Cursor unter das Spielfeld, um es nicht zu besch�digen
	SetConsoleCursorPosition(hConsole, unterFeld);
	printf("Wo soll der Roboter starten? erst X(rechts), Leerzeichen und dann Y-Wert(runter)\n");
	scanf_s("%d ", &(position->X)); //schreibt den 1. eingegebenen Wert in Posioion.X
	scanf_s("%d", &(position->Y)); // schreibt den 2. eingegebenen Wert in Posioion.Y


	if (position->X < 0 || position->Y < 0 || position->X > sizeX || position->Y > sizeY) //checkt, ob der Roboter auserhalb der Spielfeldes gesetzt werden soll
	{
		printf("\nder Roboter wurde auserhalb des Spielfeldes gespawnt\n");
		return(-1);
	}
	else if (feld[position->X][position->Y] == '1') //checkt, ob der Robotor auf einer Wand gespawnt wird
	{
		printf("\nder Roboter wurde in einer Wand gespawnt\n");
		return(-1);

	}

	unterFeld.Y = sizeY + 10; //setzt den Cursor unter das Spielfeld, um es nicht zu besch�digen
	SetConsoleCursorPosition(hConsole, unterFeld);
	//printf("                                                                                    \n");
	//printf("                                                                                    \n");
	//printf("                                                                                    \n");
	//printf("                                                                                    \n");
	//printf("                                                                                    \n");
}
l�scheRobo(COORD* position, HANDLE hConsole, char** feld/*, BOOL spur*/)
{
	SetConsoleCursorPosition(hConsole, *position); // setzt die Position auf den Roboter zur�ck, nachdem sie durch das Schreiben ge�ndert wurde
	printf(" "); // l�scht das Zeichen f�r den Roboter wieder

}
bewegeRobo(COORD* position, HANDLE hConsole, int *richtung, int sizeX, int sizeY, char** feld)
{
	COORD tempCoord;
	tempCoord.X = 0;
	tempCoord.Y = 0;
	BOOL spur;
	int ausg�nge = 0;
	int tempRichtung;
	/*
	for (int i = -1; i <= 1; i++)
	{
		switch ((richtung + i) % 4)
		{
		case 0: //Robotor schaut nach oben
		{
			if (feld)
			{

			}
		}
		case 1: //Robotor schaut nach rechts
		{
			printf(">");
			break;
		}
		case 2: //Robotor schaut nach unten
		{
			printf("V");
			break;
		}
		case 3: //Robotor schaut nach links
		{
			printf("<");
			break;
		}
		default:
			break;
		}
	}*/
	*richtung = (*richtung + 2) % 4; //Dreht den Roboter nach hinten, damit er nach der Linksdrehung
	do
	{
		*richtung = (*richtung + 3) % 4; //Dreht den Roboter nach links
		switch (*richtung)
		{
		case 0: //Robotor schaut nach oben
		{
			tempCoord.X = position->X;
			tempCoord.Y = position->Y - 1;
			break;
		}
		case 1: //Robotor schaut nach rechts
		{
			tempCoord.X = position->X + 1;
			tempCoord.Y = position->Y;
			break;
		}
		case 2: //Robotor schaut nach unten
		{
			tempCoord.X = position->X;
			tempCoord.Y = position->Y + 1;
			break;
		}
		case 3: //Robotor schaut nach links
		{
			tempCoord.X = position->X - 1;
			tempCoord.Y = position->Y;
			break;
		}
		default:
			break;
		}
	} while (feld[tempCoord.X][tempCoord.Y] == '1');

	l�scheRobo(position, hConsole, feld/*, spur*/);
	position->X = tempCoord.X;
	position->Y = tempCoord.Y;
}
zeigeRobo(COORD* position, HANDLE hConsole, int richtung)
{
	SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN); // Vordegrund auf Schwarz setzen
	SetConsoleCursorPosition(hConsole, *position);
	switch (richtung) // Switch case mit der Variable richtung -> Zeigt in Abh�ngigkeit der Richtung verscheidene Symbole an
	{
	case 0: //Robotor schaut nach oben
	{
		printf("A");
		break;
	}
	case 1: //Robotor schaut nach rechts
	{
		printf(">");
		break;
	}
	case 2: //Robotor schaut nach unten
	{
		printf("V");
		break;
	}
	case 3: //Robotor schaut nach links
	{
		printf("<");
		break;
	}
	default:
		printf("Error: keine valide Richtung");
		break;
	}
}
debug(int i, int j, int k)
{
	SetConsoleCursorPosition(hConsole, debugCoord);
}
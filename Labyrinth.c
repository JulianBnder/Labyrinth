#include <stdio.h>
#include <windows.h>
#include <stdbool.h>
#include <time.h>
#include <stdlib.h>


/*
in main bei der Spielgroeßenabfrage checken, ab wann es Probleme gibt und dann ein oberes Limit einbauen (false ersetzen)
Wenn noch Zeit ist in main beim Funktiosaufruf von platziereRobo und in PlatziereRobo aendern, sodass es nach neuem Input fragt
	um der Funktionsaufruf das: //while (position.X == 0 && position.Y == 0) //gibt die Moeglichkeit die Position neu zu waehlen, wenn sie falsch gesetzt wurde
	oder noch besser while Schleife in der Funktion

Feldtemp vielleicht nicht nehmen und durch *Feld ersetzen
*/
/******************************************************************************************************
Hauptfunktion

Ruft die pseudomain Funktion auf, damit diese mit Parametern arbeiten kann

Rückgabewerte:
siehe Pseudomain
******************************************************************************************************/
int main()
{
	return(pseudomain(1000, 1, 1));
	//Parameter: Geschwindigkeit in LE pro Sekunde, Startposition x, Startposition y
}

/******************************************************************************************************
die eingentliche Mainfunktion

ruft andere Funktionen auf, die Variablen, die sonst global implementiert werden würden sind hier als
lokale Variablen gespeichert

Parameter:
Die Parameter werden alle mit call by value übergeben, weil die Werte in der main egal sind
int sleeptemp			Wie schnell der Roboter sich bewegen soll in LE pro Sekunde
int tempX				X-Koordinate des Roboters
int tempY				Y-Koordinate des Roboters

Rückgabewerte:
 0				Alles ok
-1
******************************************************************************************************/
int pseudomain(int sleeptemp, int tempX, int tempY)
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD position = { .X = tempX, .Y = tempY }; // legt die Koordinaten für den Roboter fest
	int richtung = 0;
	int sleeptime = 1000 / sleeptemp; //legt fest wie lange er in einer Position ist
	int** feld;
	int sizeX = 0;
	int sizeY = 0;

	leseLabyrinth(&feld, &sizeX, &sizeY); //liest das Labyrinth ein
	ausgabeLabyrinth(feld, sizeX, sizeY); //gibt das Labyrinth aus
	platziereRobo(&position, hConsole, sizeX, sizeY, feld); //fragt den Benutzer, wo der Roboter anfangen soll
	zeigeRobo(&position, hConsole, richtung);


	loeseLabyrinth(&position, hConsole, &richtung, sleeptime, sizeX, sizeY, feld);


	for (int i = 0; i < sizeX; i++) //befreit den Speicherplatz des Feldes
	{
		free(feld[i]);
	}
	free(feld);

	position.X = 0;
	position.Y = sizeY + 2; //setzt den Cursor unter das Labyrinth, um es nicht zu beschaedigen
	SetConsoleCursorPosition(hConsole, position);
}

/******************************************************************************************************
Liest die Datei Labyrinth.txt aus und speichert sie in einem Array

Parameter:
Alle Parameter werden durch call by reference übergeben, weil die Funktion dem Programm Informationen
bereitstellt und deshalb in die "generellen" Variablen schreibt
int*** feld				Der Doppelpointer/2D-Array, der auf das Labyrinth zeigt
int* sizeX /sizeY		Die Größe des Labyrinths in X-/Y-Richtung

Rückgabewerte:
 0				Alles ok
-1				Datei konnte nicht gelesen werden
-2				Das Angegebene Spielfeld ist zu klein
-3				Das Angegebene Spielfeld ist zu groß (noch nicht implementiert)
******************************************************************************************************/
int leseLabyrinth(int*** feld, int* sizeX, int* sizeY)
{
	int temp;
	int** feldTemp; //Temporärer Pointer für malloc
	FILE* fp;

	fopen_s(&fp, "Labyrinth.txt", "r");

	if (fp == NULL) //Fehlermeldung, wenn die Datei nicht geöffnet werden konnte
	{
		printf("Datei konnte nicht gelesen werden.");
		return(-1);
	}

	else
	{
		fscanf_s(fp, "%d ", sizeX);
		fscanf_s(fp, "%d\n", sizeY);

		if (sizeX <= 2 || sizeY <= 2)// checkt, ab das Labyrinth zu klein ist, um es sinnvoll zu loesen
		{
			printf("X- und/oder Y-Werte sind zu klein\n");
			return(-2);
		}
		else if (false) // checkt, ob das Labyrinth so groß ist, dass es Probleme verursacht
		{
			printf("X- und/oder Y-Werte sind zu groß\n");
			return(-1);
		}



		feldTemp = (int**)malloc(*sizeX * sizeof(int*)); //reserviert Speicherplatz für den Array-Array
		for (int i = 0; i < *sizeX; i++)
		{
			feldTemp[i] = (int*)malloc((*sizeY) * sizeof(int)); //reserviert Speicherplatz für die Arrays im  äuseren Array
		}
		for (int i = 0; i < *sizeY; i++) // geht das Labyrinth durch
		{
			for (int k = 0; k < *sizeX; k++)
			{
				feldTemp[k][i] = ((int)fgetc(fp) - 48); //Labyrinth aus Datei in Array schreiben
				temp = fgetc(fp); //Leerzeichen (/Zeilenumbrüche) aus der Datei ignorieren
			}

		}

		*feld = feldTemp; //den eigentlichen feld-Array auf den Speicher zeigen lassen
	}
	fclose(fp); //schliest die Datei wieder
	return(0);
}

/******************************************************************************************************
[Beschreibung]

[Längere Beschreibung]

Parameter:
call by reference / value
var				Beschreibung

Rückgabewerte:
 0				Alles ok
-1
******************************************************************************************************/
ausgabeLabyrinth(int** feld, int sizeX, int sizeY)
{
	HANDLE hConsole;
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	for (int i = 0; i < sizeY; i++) //
	{
		for (int k = 0; k < sizeX; k++)
		{
			if (feld[k][i] == 1)
			{
				SetConsoleTextAttribute(hConsole, BACKGROUND_BLUE | BACKGROUND_GREEN); //Hintergrund auf Türkis setzen
				printf(" ");
			}
			else if (feld[k][i] == 40) // 40 = (int)"X" - 48
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

/******************************************************************************************************
[Beschreibung]

[Längere Beschreibung]

Parameter:
call by reference / value
var				Beschreibung

Rückgabewerte:
 0				Alles ok
-1
******************************************************************************************************/
loeseLabyrinth(COORD* position, HANDLE hConsole, int* richtung, int sleeptime, int sizeX, int sizeY, int** feld)
{
	while (feld[position->X][position->Y] != 40) // 40 = (int)"X" - 48
	{
		bewegeRobo(position, hConsole, richtung, sizeX, sizeY, feld);
		zeigeRobo(position, hConsole, *richtung);
		Sleep(sleeptime); //laesst den Roboter für Sleeptime ms in einer Stelle verweilen
	}
}

/******************************************************************************************************
[Beschreibung]

[Längere Beschreibung]

Parameter:
call by reference / value
var				Beschreibung

Rückgabewerte:
 0				Alles ok
-1
******************************************************************************************************/
platziereRobo(COORD* position, HANDLE hConsole, int sizeX, int sizeY, int** feld)
{
	COORD unterFeld; //Courser für Text
	unterFeld.X = 0;
	unterFeld.Y = sizeY + 2; //setzt den Cursor unter das Labyrinth, um es nicht zu beschaedigen
	SetConsoleCursorPosition(hConsole, unterFeld);

	/*printf("Wo soll der Roboter starten? erst X(rechts), Leerzeichen und dann Y-Wert(runter)\n");
	scanf_s("%d ", &(position->X)); //schreibt den 1. eingegebenen Wert in Posioion.X
	scanf_s("%d", &(position->Y)); // schreibt den 2. eingegebenen Wert in Posioion.Y
	*/


	if (position->X < 0 || position->Y < 0 || position->X > sizeX || position->Y > sizeY) //checkt, ob der Roboter auserhalb des Labyrinthes gesetzt werden soll
	{
		printf("\nder Roboter wurde auserhalb des Labyrinths gespawnt\n");
		return(-1);
	}
	else if (feld[position->X][position->Y] == 1) //checkt, ob der Robotor auf einer Wand gespawnt wird
	{
		printf("\nder Roboter wurde in einer Wand gespawnt\n");
		return(-1);

	}

	unterFeld.Y = sizeY + 10; //setzt den Cursor unter des Labyrinth, um es nicht zu beschaedigen
	SetConsoleCursorPosition(hConsole, unterFeld);
	//printf("                                                                                    \n");
	//printf("                                                                                    \n");
	//printf("                                                                                    \n");
	//printf("                                                                                    \n");
	//printf("                                                                                    \n");
}

/******************************************************************************************************
[Beschreibung]

[Längere Beschreibung]

Parameter:
call by reference / value
var				Beschreibung

Rückgabewerte:
 0				Alles ok
-1
******************************************************************************************************/
loescheRobo(COORD* position, HANDLE hConsole, int** feld, BOOL spur)
{
	if (spur)
	{
		SetConsoleTextAttribute(hConsole, BACKGROUND_RED); //Hintergrund auf rot setzen
	}
	SetConsoleCursorPosition(hConsole, *position); // setzt die Position auf den Roboter zurück, nachdem sie durch das Schreiben geaendert wurde
	printf(" "); // loescht das Zeichen für den Roboter wieder
	SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN); //Vordergrund auf Schwarz setzen

}

/******************************************************************************************************
[Beschreibung]

[Längere Beschreibung]

Parameter:
call by reference / value
var				Beschreibung

Rückgabewerte:
 0				Alles ok
-1
******************************************************************************************************/
bewegeRobo(COORD* position, HANDLE hConsole, int* richtung, int sizeX, int sizeY, int** feld)
{
	COORD tempCoord;
	tempCoord.X = 0;
	tempCoord.Y = 0;
	_Bool spur;
	int ausgaenge = 0;
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
	for (int i = 0; i < 4; i++) // zaehlt, in wie viele Richtungen der Roboter gehen kann
	{
		switch (i)
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
		if (feld[tempCoord.X][tempCoord.Y] != 1) //checkt, ob in die Richtung ein Weg ist
		{
			ausgaenge++;
		}

	}











	* richtung = (*richtung + 2) % 4; //Dreht den Roboter nach hinten, damit er nach der Linksdrehung nach Rechts schaut
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
	} while (feld[tempCoord.X][tempCoord.Y] == 1);
	spur = true;
	feld[position->X][position->Y]--;
	if (feld[position->X][position->Y] <= -ausgaenge)
	{
		spur = false;
	}

	//spur = !(--feld[position->X][position->Y] <= -ausgaenge);

	//debug_f(debugCoord.Y, ausgaenge, spur);
	loescheRobo(position, hConsole, feld, spur);
	position->X = tempCoord.X;
	position->Y = tempCoord.Y;
}

/******************************************************************************************************
[Beschreibung]

[Längere Beschreibung]

Parameter:
call by reference / value
var				Beschreibung

Rückgabewerte:
 0				Alles ok
-1
******************************************************************************************************/
zeigeRobo(COORD* position, HANDLE hConsole, int richtung)
{
	SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN); // Vordegrund auf Schwarz setzen
	SetConsoleCursorPosition(hConsole, *position);
	switch (richtung) // Switch case mit der Variable richtung -> Zeigt in Abhaengigkeit der Richtung verscheidene Symbole an
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

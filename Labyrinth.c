#include <stdio.h>
#include <windows.h>
#include <stdbool.h>
#include <time.h>
#include <stdlib.h>



void main()
{
	return(pseudomain(4, 1, 2));
	//Parameter: Geschwindigkeit in LE pro Sekunde, Startposition x, Startposition y
}

pseudomain(int sleeptemp, int tempX, int tempY)
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD position = { .X = tempX, .Y = tempY }; // legt die Koordinaten für den Roboter fest
	int richtung = 0; //0 = obern; 1 = rechts; 2 = unten; 3 = links
	int sleeptime = 1000 / sleeptemp; //legt fest wie lange er in einer Position ist
	int** feld;
	int sizeX = 0;
	int sizeY = 0;

	leseLabyrinth(&feld, &sizeX, &sizeY); //liest das Labyrinth ein
	ausgabeLabyrinth(feld, sizeX, sizeY); //gibt das Labyrinth aus
	if (platziereRobo(&position, hConsole, sizeX, sizeY, feld)) //fragt den Benutzer, wo der Roboter anfangen soll
	{
		position.X = 0;
		position.Y = sizeY + 5; //setzt den Cursor unter das Labyrinth, um es nicht zu beschaedigen
		SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN); //Vordergrund auf Schwarz setzen
		SetConsoleCursorPosition(hConsole, position);
		return(-1);
	}
	zeigeRobo(&position, hConsole, richtung);


	loeseLabyrinth(&position, hConsole, &richtung, sleeptime, sizeX, sizeY, feld);


	for (int i = 0; i < sizeX; i++) //befreit den Speicherplatz des Feldes
	{
		free(feld[i]);
	}
	free(feld);

	//{ //macht den Anfangspunkt des Roboters rot, weil der sonst schwarz ist, wenn der Roboter in einer Sackgasse startet
	//	position.X = tempX;
	//	position.Y = tempY;
	//	SetConsoleTextAttribute(hConsole, BACKGROUND_RED); //Hintergrund auf rot setzen
	//	printf(" ");
	//}

	position.X = 0;
	position.Y = sizeY + 5; //setzt den Cursor unter das Labyrinth, um es nicht zu beschaedigen
	SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN); //Vordergrund auf Schwarz setzen
	SetConsoleCursorPosition(hConsole, position);

	return(0);
}

leseLabyrinth(int*** feld, int* sizeX, int* sizeY)
{
	int** feldTemp; //Temporärer Pointer für malloc
	FILE* fp; // Pointer auf die Datei, die gelesen wird

	fopen_s(&fp, "Labyrinth.txt", "r"); //setzt den Pointer auf den Anfang des Textes in der Datei

	if (fp == NULL) //Fehlermeldung, wenn die Datei nicht geöffnet werden konnte
	{
		printf("Datei konnte nicht gelesen werden.");
		return(-1);
	}

	fscanf_s(fp, "%d ", sizeX); // die Dimensionen des Labyrinths in X- und Y-Richtung werden als Zahlen eingelesen
	fscanf_s(fp, "%d\n", sizeY);

	if (sizeX <= 2 || sizeY <= 2)// checkt, ab das Labyrinth zu klein ist, um es sinnvoll zu loesen
	{
		printf("X- und/oder Y-Werte sind zu klein\n");
		return(-2);
	}
	else if (false) // checkt, ob das Labyrinth so groß ist, dass es Probleme verursacht
	{
		printf("X- und/oder Y-Werte sind zu groß\n");
		return(-3);
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
			fgetc(fp); //Leerzeichen (und Zeilenumbrüche) aus der Datei ignorieren
		}

	}

	*feld = feldTemp; //den eigentlichen feld-Array auf den Speicher zeigen lassen
	fclose(fp); //schliest die Datei wieder
	return(0);
}

ausgabeLabyrinth(int** feld, int sizeX, int sizeY)
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE); //Initialisierung für die Ausgabe über die Konsole

	for (int i = 0; i < sizeY; i++) //Zeigt das Labyrinth auf dem Spielfeld an
	{
		for (int k = 0; k < sizeX; k++)
		{
			if (feld[k][i] == 1) // Bei einer Wand
			{
				SetConsoleTextAttribute(hConsole, BACKGROUND_BLUE | BACKGROUND_GREEN); //Hintergrund auf Türkis setzen
				printf(" ");
			}
			else // in einem Gang
			{
				SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN); //Vordergrund auf Schwarz setzen

				if (feld[k][i] == 40) // 40 = ((int)"X") - 48
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
	SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN); //Vordergrund auf Schwarz setzen
}

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
	else
	{
		return(0);
	}
}

loeseLabyrinth(COORD* position, HANDLE hConsole, int* richtung, int sleeptime, int sizeX, int sizeY, int** feld)
{
	while (feld[position->X][position->Y] != 40) // 40 = (int)"X" - 48 also  Wärend das Ziel noch nicht gefunden wurde wird weitergesucht
	{
		bewegeRobo(position, hConsole, richtung, sizeX, sizeY, feld); //in der Funktion wird auch an der alten Stelle der Roboter in der Konsole gelöscht
		zeigeRobo(position, hConsole, *richtung);
		Sleep(sleeptime); //laesst den Roboter für [Sleeptime] ms in einer Stelle verweilen
	}
}

loescheRobo(COORD* position, HANDLE hConsole, int** feld, BOOL spur)
{
	if (spur) //wenn das Feld, auf dem der Roboter ist, ein Teil des kürzesten Weges ist
	{
		SetConsoleTextAttribute(hConsole, BACKGROUND_RED); //Hintergrund auf rot setzen
	}
	SetConsoleCursorPosition(hConsole, *position); // setzt die Position auf den Roboter zurück, nachdem sie durch das Schreiben eins nach rechts verschoben wurde
	printf(" "); // loescht das Zeichen für den Roboter wieder
	SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN); //Vordergrund auf Schwarz setzen

}

bewegeRobo(COORD* position, HANDLE hConsole, int* richtung, int sizeX, int sizeY, int** feld)
{
	COORD tempCoord = { .X = 0, .Y = 0 }; // temporäre Koordinate für Bewegung
	_Bool spur; // ob eine Spur gezogen werden soll
	int ausgaenge = 0; // Zähler für die Wege, die neben dem derzeitigen Weg liegen


	{ // Zählt, wie viele Wege neben dem Feld sind, auf dem der Roboter ist
		if (feld[position->X - 1][position->Y] != 1) //checkt, ob links ein Weg ist
		{
			ausgaenge++;
		}
		if (feld[position->X + 1][position->Y] != 1) //checkt, ob rechts ein Weg ist
		{
			ausgaenge++;
		}
		if (feld[position->X][position->Y - 1] != 1) //checkt, ob oben ein Weg ist
		{
			ausgaenge++;
		}
		if (feld[position->X][position->Y + 1] != 1) //checkt, ob unten ein Weg ist
		{
			ausgaenge++;
		}
	}

	feld[position->X][position->Y]--; // zählt wie oft der Roboter auf einem Feld war, es zählt negativ, weil der Roboter denken würde es ist eine Wand, wenn es 1 werden würde
	spur = true; // generell zieht der Roboter eine Spur hinter sich her
	if (-(feld[position->X][position->Y]) >= ausgaenge) // auser er war so oft da, wie es benachberte Wege hat, dann dann geht er dahin zurück, wo er hergekommen ist und es war eine Sackgasse
	{
		spur = false;
	}


	*richtung = (*richtung + 2) % 4; //Dreht den Roboter nach hinten, damit er nach der 1. Linksdrehung nach Rechts schaut
	while (feld[tempCoord.X][tempCoord.Y] == 1)
	{
		*richtung = (*richtung + 3) % 4; //Dreht den Roboter nach links
		switch (*richtung) // setzt jeweils die temporäre Koordinate für die while-Abfrage
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
	};

	//spur = !(--feld[position->X][position->Y] <= -ausgaenge);

	loescheRobo(position, hConsole, feld, spur);
	position->X = tempCoord.X;
	position->Y = tempCoord.Y;
}

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
		return(-1);
		break;
	}
}

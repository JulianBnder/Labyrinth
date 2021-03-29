#include <stdio.h>
#include <windows.h>
#include <stdbool.h>
#include <time.h>
#include <stdlib.h>



void main()
{
	//Parameter: Geschwindigkeit in LE pro Sekunde, Startposition x, Startposition y
	return(pseudomain(43, 1, 2));

}

pseudomain(int sleeptemp, int tempX, int tempY)
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE); // Intialisierung für Konsolenausgabe
	COORD position = { .X = tempX, .Y = tempY }; // legt die Startoordinaten für den Roboter fest
	int richtung = 0; //0 = oben; 1 = rechts; 2 = unten; 3 = links
	int sleeptime = 1000 / sleeptemp; //legt fest wie lange er in einer Position ist
	int** feld;
	int sizeX = 0;
	int sizeY = 0;


	if (leseLabyrinth(&feld, &sizeX, &sizeY) != 0) //liest das Labyrinth ein
	{
		//setzt den Cursor unter das Labyrinth, um es nicht zu beschaedigen und setzt den Vordergrund auf Schwarz
		position.X = 0;
		position.Y = sizeY + 5;
		SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN);
		SetConsoleCursorPosition(hConsole, position);
		return(-1);
	}
	ausgabeLabyrinth(feld, sizeX, sizeY); //gibt das Labyrinth aus
	if (platziereRobo(&position, hConsole, sizeX, sizeY, feld) != 0) //fragt den Benutzer, wo der Roboter anfangen soll
	{
		//setzt den Cursor unter das Labyrinth, um es nicht zu beschaedigen und setzt den Vordergrund auf Schwarz
		position.X = 0;
		position.Y = sizeY + 5;
		SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN);
		SetConsoleCursorPosition(hConsole, position);
		return(-2);
	}
	zeigeRobo(&position, hConsole, richtung);

	// läst das Labyrinth
	loeseLabyrinth(&position, hConsole, &richtung, sleeptime, sizeX, sizeY, feld);


	//befreit den Speicherplatz des Feldes
	for (int i = 0; i < sizeX; i++)
	{
		free(feld[i]);
	}
	free(feld);

	//Der Startpunkt wird rot gemacht
	position.X = tempX;
	position.Y = tempY;
	SetConsoleCursorPosition(hConsole, position);
	SetConsoleTextAttribute(hConsole, BACKGROUND_RED);
	printf(" ");

	//setzt den Cursor unter das Labyrinth, um es nicht zu beschaedigen und den Hintergrund Schwarz
	position.X = 0;
	position.Y = sizeY + 5;
	SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN);
	SetConsoleCursorPosition(hConsole, position);

	return(0);
}

leseLabyrinth(int*** feld, int* sizeX, int* sizeY)
{
	int** feldTemp; //Temporärer Pointer für malloc
	FILE* fp; // Pointer auf die Datei, die gelesen wird

	//setzt den Pointer auf den Anfang des Textes in der Datei
	fopen_s(&fp, "Labyrinth.txt", "r");

	//Fehlermeldung, wenn die Datei nicht geöffnet werden konnte
	if (fp == NULL)
	{
		printf("Datei konnte nicht gelesen werden.");
		return(-1);
	}

	// die Dimensionen des Labyrinths in X- und Y-Richtung werden als Zahlen eingelesen
	fscanf_s(fp, "%d ", sizeX);
	fscanf_s(fp, "%d\n", sizeY);

	// checkt, ab das Labyrinth zu klein ist, um es sinnvoll zu loesen
	if (*sizeX <= 2 || *sizeY <= 2)
	{
		printf("X- und/oder Y-Werte sind zu klein\n");
		return(-2);
	}
	// checkt, ob das Labyrinth so groß ist, dass es Probleme verursacht
	else if (false)
	{
		printf("X- und/oder Y-Werte sind zu groß\n");
		return(-3);
	}


	//reserviert Speicherplatz für den Array-Array und für die Arrays in dem Array
	feldTemp = (int**)malloc(*sizeX * sizeof(int*));
	for (int i = 0; i < *sizeX; i++)
	{
		feldTemp[i] = (int*)malloc((*sizeY) * sizeof(int));
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
				//Hintergrund auf Schwarz setzen
				SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN);

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

	//checkt, ob der Roboter auserhalb des Labyrinthes gesetzt werden soll
	if (position->X < 0 || position->Y < 0 || position->X > sizeX || position->Y > sizeY)
	{
		printf("\nder Roboter wurde auserhalb des Labyrinths gespawnt\n");
		return(-1);
	}
	//checkt, ob der Robotor auf einer Wand gespawnt wird
	else if (feld[position->X][position->Y] == 1)
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
	// 40 = (int)"X" - 48 also  Wärend das Ziel noch nicht gefunden wurde wird weitergesucht
	while (feld[position->X][position->Y] != 40)
	{
		//in bewegeRobo wird auch an der alten Stelle der Roboter in der Konsole gelöscht
		bewegeRobo(position, hConsole, richtung, sizeX, sizeY, feld);
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
	// setzt die Position auf den Roboter zurück, nachdem sie durch das Schreiben versetzt wurde
	SetConsoleCursorPosition(hConsole, *position);
	printf(" ");
	SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN);
}

bewegeRobo(COORD* position, HANDLE hConsole, int* richtung, int sizeX, int sizeY, int** feld)
{
	COORD tempCoord = { .X = 0, .Y = 0 }; // temporäre Koordinate für Bewegung
	_Bool spur; // ob eine Spur gezogen werden soll
	int ausgaenge = 0; // Zähler für die Wege, die neben dem derzeitigen Weg liegen


	// Zählt, wie viele Wege neben dem Feld sind, auf dem der Roboter ist
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

	// zählt wie oft der Roboter auf einem Feld war, negativ, weil Wert 1 eine Wand darstellt
	feld[position->X][position->Y]--;
	spur = true; // generell zieht der Roboter eine Spur hinter sich her
	// auser er war so oft da, wie es benachberte Wege hat, 
	//dann dann geht er dahin zurück, wo er hergekommen ist und es war eine Sackgasse
	if (-(feld[position->X][position->Y]) >= ausgaenge)
	{
		spur = false;
	}
	//spur = !(-(--feld[position->X][position->Y]) >= ausgaenge);

	//Dreht den Roboter nach hinten, damit er nach der 1. Linksdrehung nach Rechts schaut
	*richtung = (*richtung + 2) % 4;
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

	loescheRobo(position, hConsole, feld, spur);
	position->X = tempCoord.X;
	position->Y = tempCoord.Y;
}

zeigeRobo(COORD* position, HANDLE hConsole, int richtung)
{
	SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN);
	SetConsoleCursorPosition(hConsole, *position);
	// Zeigt in Abhaengigkeit der Richtung verscheidene Symbole an
	switch (richtung)
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

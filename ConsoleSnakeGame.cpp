// ConsoleSnakeGame.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <wchar.h>
#include <Windows.h>
#include <thread>
#include <utility>
#include <random>

using namespace std;


const int screenwidth = 120;
const int screenheight = 30;
const int playingfieldwidth = screenwidth / 2;
const int playingfieldheight = screenheight / 2;
int pfieldxoffset = 30;
int pfieldyoffset = 7;
int xpos = 0;
int ypos = 0;
int preypos = ypos;
int prexpos = xpos;

struct snakebody
{
	pair<int, int> currpos;
	pair<int, int> prevpos;
};

struct foodloc
{
	pair<int, int> pos;
};

bool InBound(int curx, int cury, int xleftbound, int yupbound, int xrightbound, int ydownbound)
{
	return curx > xleftbound && curx < xrightbound&& cury > yupbound && cury < ydownbound;
}


foodloc GenerateFood(int snakesize, snakebody snek[])
{
	bool isvalid = false;
	int row = 0;
	int column = 0;

	while (!isvalid)
	{
		row = abs(rand()) % ((pfieldyoffset + playingfieldheight - 2) - (pfieldyoffset + 1) + 1) + (pfieldyoffset + 1);
		column = abs(rand()) % ((pfieldxoffset + playingfieldwidth - 2) - (pfieldxoffset + 1) + 1) + (pfieldxoffset + 1);

		for (int i = 0; i < snakesize; i++)
		{
			isvalid = (snek[i].currpos.first == column && snek[i].currpos.second == row) ? false : true;
		}
	}

	foodloc location;
	location.pos.first = column;
	location.pos.second = row;

	return location;
}


void Snake(HANDLE h, DWORD& dw)
{
	snakebody snake[(playingfieldwidth - 1) * (playingfieldheight - 1)];
	snake[0].currpos.first = pfieldxoffset + playingfieldwidth / 2, snake[0].currpos.second = pfieldyoffset + playingfieldheight / 2;
	snake[1].currpos.first = pfieldxoffset + playingfieldwidth / 2, snake[1].currpos.second = (pfieldyoffset + playingfieldheight / 2) + 1;
	snake[2].currpos.first = pfieldxoffset + playingfieldwidth / 2, snake[2].currpos.second = (pfieldyoffset + playingfieldheight / 2) + 2;
	int currsnakesize = 3;
	wchar_t sbodypart = L'X';
	wchar_t food = L'O';
	int occupiedspaces[(playingfieldwidth - 1) * (playingfieldheight - 1) - 1];
	wchar_t playingfieldbuffer[screenheight * screenwidth];
	for (int i = 0; i < screenwidth * screenheight; i++) playingfieldbuffer[i] = L' ';

	bool buttonpress[4];

	int row = -1;
	int column = 0;

	for (int i = 0; i < playingfieldwidth * playingfieldheight; i++)
	{
		column = i % playingfieldwidth;
		row += (column == 0) ? 1 : 0;

		playingfieldbuffer[(row + pfieldyoffset) * screenwidth + (column + pfieldxoffset)] = ((row == 0 || row == playingfieldheight - 1) || (column == 0 || column % playingfieldwidth == playingfieldwidth - 1)) ? L'#' : L' ';
	}

	foodloc fl = GenerateFood(currsnakesize, snake);

	playingfieldbuffer[snake[0].currpos.second * screenwidth + snake[0].currpos.first] = sbodypart;
	playingfieldbuffer[snake[1].currpos.second * screenwidth + snake[1].currpos.first] = sbodypart;
	playingfieldbuffer[snake[2].currpos.second * screenwidth + snake[2].currpos.first] = sbodypart;

	playingfieldbuffer[fl.pos.second * screenwidth + fl.pos.first] = food;

	SetConsoleActiveScreenBuffer(h);


	int xvelocity = 0;
	int yvelocity = 0;
	bool gameover = false;
	bool generatefood = false;
	while (!gameover)
	{
		this_thread::sleep_for(50ms);

		if (GetAsyncKeyState(VK_LEFT) && xvelocity != 1)
		{
			xvelocity = -1;
			yvelocity = 0;
		}

		if (GetAsyncKeyState(VK_RIGHT) && xvelocity != -1)
		{
			xvelocity = 1;
			yvelocity = 0;
		}

		if (GetAsyncKeyState(VK_UP) && yvelocity != 1)
		{
			xvelocity = 0;
			yvelocity = -1;
		}

		if (GetAsyncKeyState(VK_DOWN) && yvelocity != -1)
		{
			xvelocity = 0;
			yvelocity = 1;
		}

		if (GetAsyncKeyState(VK_RETURN))
		{
			break;
		}

		for (int i = 0; i < currsnakesize; i++)
		{
			playingfieldbuffer[snake[i].currpos.second * screenwidth + snake[i].currpos.first] = L' ';

			snake[i].prevpos.first = snake[i].currpos.first;
			snake[i].prevpos.second = snake[i].currpos.second;

			if (xvelocity + yvelocity != 0)
			{
				snake[i].currpos.first = (i == 0) ? snake[0].currpos.first + xvelocity : snake[i - 1].prevpos.first;
				snake[i].currpos.second = (i == 0) ? snake[0].currpos.second + yvelocity : snake[i - 1].prevpos.second;
			}

			playingfieldbuffer[snake[i].currpos.second * screenwidth + snake[i].currpos.first] = sbodypart;
		}

		if (snake[0].currpos.first == fl.pos.first && snake[0].currpos.second == fl.pos.second)
		{
			generatefood = true;
			//cout << generatefood << endl;
		}

		if (generatefood)
		{
			//cout << "Here" << endl;
			currsnakesize++;
			snake[currsnakesize - 1].currpos.first = snake[currsnakesize - 2].prevpos.first, snake[currsnakesize - 1].currpos.second = snake[currsnakesize - 2].prevpos.second;
			playingfieldbuffer[snake[currsnakesize - 1].currpos.second * screenwidth + snake[currsnakesize - 1].currpos.first] = sbodypart;

			fl = GenerateFood(currsnakesize, snake);
			playingfieldbuffer[fl.pos.second * screenwidth + fl.pos.first] = food;
			generatefood = false;
		}

		gameover = (InBound(snake[0].currpos.first, snake[0].currpos.second, pfieldxoffset, pfieldyoffset, pfieldxoffset + playingfieldwidth, pfieldyoffset + playingfieldheight)) ? false : true;

		for (int i = 1; i < currsnakesize; i++)
		{
			if (snake[0].currpos.first == snake[i].currpos.first && snake[0].currpos.second == snake[i].currpos.second)
			{
				cout << "Here" << endl;
				gameover = true;
				break;
			}
		}

		WriteConsoleOutputCharacter(h, playingfieldbuffer, screenwidth * screenheight, { 0, 0 }, &dw);
	}

}

int main()
{
	srand((unsigned)time(0));
	HANDLE screen = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	DWORD dwbytes = 0;

	Snake(screen, dwbytes);
	
	CloseHandle(screen);

	return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file

#include <windows.h>
#include <iostream>
#include <conio.h>
#include <time.h>
#include <ctime>

enum { LEFT = 75, UP = 72, RIGHT = 77, DOWN = 80, ESC = 27};

bool game_over = false;
HANDLE hOut;
CONSOLE_SCREEN_BUFFER_INFO con_info;
SHORT w = 0;
SHORT h = 0;

struct snake
{
	unsigned char dir = RIGHT;
	COORD* seg;
	int size;
};

static snake snk;
COORD meal;
char* snake_style = "*";
char* bounds_style = "#";
char* meal_style = "@";

void draw_char(COORD pos, char* ch)
{
	DWORD written;
	WriteConsoleOutputCharacter(hOut, ch, strlen(ch),pos, &written);
}

void draw_bounds()
{
	system("cls");
	char* l_filler = new char[w+1];
	memset(l_filler,*bounds_style,sizeof(char)*w);
	l_filler[w] = '\0';

	draw_char({ 0,0 }, l_filler);

	draw_char({ 0,h }, l_filler);

	for (int i = 1; i < h; i++)
		draw_char({0,(short)i}, bounds_style);

	for (int i = 1; i < h; i++)
		draw_char({w-1,(short)i}, bounds_style);

	delete[] l_filler;
}

bool growth = false;

void collision_det()
{
	if (snk.seg[snk.size - 1].X >= w || snk.seg[snk.size - 1].X <= 0 ||
		snk.seg[snk.size - 1].Y >= h || snk.seg[snk.size - 1].Y <= 0)
		game_over = true;

	for (int i = 0; i < snk.size - 1; i++)
		if (snk.seg[snk.size - 1].X == snk.seg[i].X &&
			snk.seg[snk.size - 1].Y == snk.seg[i].Y)
			game_over = true;

	if (snk.seg[snk.size - 1].X == meal.X &&
		snk.seg[snk.size - 1].Y == meal.Y)
	{
		growth = true;
		meal.X = std::rand() % w;
		meal.Y = std::rand() % h;
		draw_char(meal, meal_style);
	}
}

void input()
{
	if (_kbhit())
	{
		_getch();
		int tmp = _getch();
		if(tmp == UP    && snk.dir != DOWN || 
		   tmp == RIGHT && snk.dir != LEFT || 
		   tmp == DOWN  && snk.dir != UP || 
		   tmp == LEFT  && snk.dir != RIGHT)
			snk.dir = tmp;
	}
}

void move()
{
	if(!growth)
		draw_char(snk.seg[0], " ");
	else
	{
		snk.size++;
		snk.seg[snk.size - 1] = snk.seg[snk.size - 2];
	}

	for (int i = 0; i < snk.size - 1 && !growth; i++)
	{
		snk.seg[i] = snk.seg[i + 1];
		draw_char(snk.seg[i], snake_style);
	}

	switch (snk.dir)
	{
		case LEFT:
			snk.seg[snk.size - 1].X--;
		break;

		case UP:
			snk.seg[snk.size - 1].Y--;
		break;

		case RIGHT:
			snk.seg[snk.size - 1].X++;
		break;

		case DOWN: 
			snk.seg[snk.size - 1].Y++;
		break;
	}
	draw_char(snk.seg[snk.size - 1], snake_style);
	growth = false;
}

void update_console_dim()
{
	GetConsoleScreenBufferInfo(hOut, &con_info);
	w = con_info.srWindow.Right + 1;
	h = con_info.srWindow.Bottom;
}

void update()
{
	unsigned int tw = w;
	unsigned int th = h;

	update_console_dim();

	if (w != tw || h != th)
	{
		con_info.dwSize.X = w;
		con_info.dwSize.Y = h;
		SetConsoleScreenBufferSize(hOut, con_info.dwSize);
		draw_bounds();
		return;
	}

	input();
	move();
	collision_det();
}

int main()
{
	hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	update_console_dim();
	draw_bounds();
	snk.seg = new COORD[w*h];
	snk.seg[0].X = 1;
	snk.seg[0].Y = 1;
	snk.size = 1;

	meal.X = (std::rand() % w-1) + 1;
	meal.Y = (std::rand() % h-1) + 1;
	draw_char(meal, meal_style);

	clock_t tmp = clock();
	while (!game_over)
	{
		if (((clock()-tmp)/(float)CLOCKS_PER_SEC)*1000 >= 200)
		{
			update();
			tmp = clock();
		}
	}
}
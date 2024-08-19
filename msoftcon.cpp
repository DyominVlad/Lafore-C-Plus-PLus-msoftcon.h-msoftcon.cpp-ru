//softcon.cpp
// подпрограммы доступа к консольным функциям Windows

// компилятор должен знать, где искать этот файл
// в MCV++, /Tools/Options/Directories/Include/type путь

#include "msoftcon.h"
HANDLE hConsole;    // поддержка консольного режима
char fill_char;    // символ заполнения
//
void init_graphics()
{
	COORD console_size = { 80, 25 };
	// открыть канал ввода/вывода на консоль
	hConsole = CreateFile(L"CONOUT$", GENERIC_WRITE | GENERIC_READ,
						  FILE_SHARE_READ | FILE_SHARE_WRITE,
						  0L, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0L);
	// установить размер экрана 80x25
	SetConsoleScreenBufferSize(hConsole, console_size);
	// текст белым по черному
	SetConsoleTextAttribute(hConsole, (WORD)((0 << 4) | 15));

	fill_char = '\xDB'; // заполнение по умолчанию
	clear_screen();
}
//
void set_color(color foreground, color background)
{
	SetConsoleTextAttribute(hConsole,
		(WORD)((background << 4) | foreground));
} // конец setcolor()

  /* 0 Черный    8 Темно-серый
  1 Темно-синий  9 Синий
  2 Темно-зеленый 10 Зеленый
  3 Темно-голубой 11 Алый
  4 Темно-красный 12 красный
  5 Темно-алый  13 алый
  6 Коричневый  14 Желтый
  7 Светло-серый  15 Белый
  */
  //
void set_cursor_pos(int x, int y)
{
	COORD cursor_pos;     // Начало в верхнем левом
	cursor_pos.X = x - 1;    // Windows начинает с (0, 0)
	cursor_pos.Y = y - 1;    // мы начнем с (1, 1)
	SetConsoleCursorPosition(hConsole, cursor_pos);
}
//
void clear_screen()
{
	set_cursor_pos(1, 25);
	for(int j = 0; j < 25; j++)
		_putch('\n');
	set_cursor_pos(1, 1);
}
//
void wait(int milliseconds)
{
	Sleep(milliseconds);
}
//
void clear_line()       // очистка до конца строки
{			            // 80 пробелов
	// .....1234567890123456789012345678901234567890
	// .....0........1.........2.........3.........4
	_cputs("                ");
	_cputs("                ");
}
//
void draw_rectangle(int left, int top, int right, int bottom)
{
	char temp[80];
	int width = right - left + 1;

	int j;
	for(j = 0; j < width; j++)   // строка квадратов
		temp[j] = fill_char;
	temp[j] = 0;          // null

	for(int y = top; y <= bottom; y++) // строковый стек
	{
		set_cursor_pos(left, y); _cputs(temp);
	}
}
//
void draw_circle(int xC, int yC, int radius)
{
	double theta, increment, xF, pi = 3.14159;
	int x, xN, yN;

	increment = 0.8 / static_cast<double>(radius);
	for(theta = 0; theta <= pi / 2; theta += increment)//1/4 окружности
	{
		xF = radius * cos(theta);
		xN = static_cast<int>(xF * 2 / 1); // пикселы не
										   // квадратные :-(
		yN = static_cast<int>(radius * sin(theta) + 0.5);
		x = xC - xN;
		while(x <= xC + xN) // заполнить две горизонтальные линии
		{					// по одной на каждую ? окружности
			set_cursor_pos(x, yC - yN); _putch(fill_char);  // верх 
			set_cursor_pos(x++, yC + yN); _putch(fill_char); // низ
		}
	} // конец for
}
//
void draw_line(int x1, int y1, int x2, int y2)
{

	int w, z, t, w1, w2, z1, z2;
	double xDelta = x1 - x2, yDelta = y1 - y2, slope;
	bool isMoreHoriz;

	if(fabs(xDelta) > fabs(yDelta)) // еще горизонтальная
	{
		isMoreHoriz = true; slope = yDelta / xDelta;
		w1 = x1; z1 = y1; w2 = x2, z2 = y2; // w = x, z = y
	}
	else           // еще вертикальная
	{
		isMoreHoriz = false; slope = xDelta / yDelta;
		w1 = y1; z1 = x1; w2 = y2, z2 = x2; // w = y, z = x
	}

	if(w1 > w2)        // если за w
	{
		t = w1; w1 = w2; w2 = t;  // заменить (w1, z1)
		t = z1; z1 = z2; z2 = t;  // на (w2, z2)
	}
	for(w = w1; w <= w2; w++)
	{
		z = static_cast<int>(z1 + slope * (w - w1));
		if(!(w == 80 && z == 25)) // убрать прокрутку на 80, 25
		{
			if(isMoreHoriz)
				set_cursor_pos(w, z);
			else
				set_cursor_pos(z, w);
			_putch(fill_char);
		}
	}
}
//
void draw_pyramid(int x1, int y1, int height)
{
	int x, y;
	for(y = y1; y < y1 + height; y++)
	{
		int incr = y - y1;
		for(x = x1 - incr; x <= x1 + incr; x++)
		{
			set_cursor_pos(x, y);
			_putch(fill_char);
		}
	}
}
//
void set_fill_style(fstyle fs)
{
	switch(fs)
	{
		case SOLID_FILL:  fill_char = '\xDB'; break;
		case DARK_FILL:   fill_char = '\xB0'; break;
		case MEDIUM_FILL: fill_char = '\xB1'; break;
		case LIGHT_FILL:  fill_char = '\xB2'; break;
		case X_FILL:      fill_char = 'X'   ; break;
		case O_FILL:      fill_char = 'O'   ; break;
	}
}
//
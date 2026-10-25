#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <math.h>
#include <Windows.h>

#define CONSOLE_SIZE 120
#define ROTATE 0.1
#define CAMERA_POINT 3

// Structs
typedef struct _Vector3
{
	double x;
	double y;
	double z;
} Vector3;

typedef struct _Cube3D
{
	Vector3 frontTopLeft;
	Vector3 frontTopRight;
	Vector3 frontBottomLeft;
	Vector3 frontBottomRight;
	Vector3 backTopLeft;
	Vector3 backTopRight;
	Vector3 backBottomLeft;
	Vector3 backBottomRight;
} Cube3D;

// Global variables
Cube3D cube;

char axis;
int direction;

BOOL screen[CONSOLE_SIZE + 1][CONSOLE_SIZE + 1];

char buffer[((CONSOLE_SIZE + 1) * 2) * (CONSOLE_SIZE + 1) + 1];

// Functions
void Init();
void Render();
void Control();
void Update();

Vector3 RotateX(Vector3 point, int direction);
Vector3 RotateY(Vector3 point, int direction);
Vector3 RotateZ(Vector3 point, int direction);
COORD PointProjection(Vector3 point);

void PointConnection(COORD a, COORD b);
void Gotoxy(unsigned short x, unsigned short y);

int main(void)
{
	Init();
	
	while (1)
	{
		Render();
		Control();
		Update();
	}
	
	return 0;
}

void Init()
{
	// Setup console
	system("title 3D Cube");
	system("mode con cols=242 lines=121");
	
	// Set font size
	const wchar_t* fontName = L"FONT";
	
	HFONT hFont = CreateFontW(4, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH || FF_DONTCARE, fontName);
	LOGFONT logFont;
	
	GetObject(hFont, sizeof logFont, &logFont);
	
	CONSOLE_FONT_INFOEX fontInfo = { sizeof(CONSOLE_FONT_INFOEX) };
	
	fontInfo.nFont = 12;
	fontInfo.dwFontSize.X = logFont.lfWidth;
	fontInfo.dwFontSize.Y = logFont.lfHeight;
	fontInfo.FontFamily = FF_DONTCARE;
	fontInfo.FontWeight = logFont.lfWeight;
	
	wcscpy(fontInfo.FaceName, fontName);
	SetCurrentConsoleFontEx(GetStdHandle(STD_OUTPUT_HANDLE), FALSE, &fontInfo);
	
	// Hode console cursor
	CONSOLE_CURSOR_INFO consoleCursorInfo;
	
	consoleCursorInfo.bVisible = 0;
	consoleCursorInfo.dwSize = 1;
	
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &consoleCursorInfo);
	
	// Initialize global variables
	cube.frontTopLeft.x = -1;
	cube.frontTopLeft.y = 1;
	cube.frontTopLeft.z = -1;
	
	cube.frontTopRight.x = 1;
	cube.frontTopRight.y = 1;
	cube.frontTopRight.z = -1;
	
	cube.frontBottomLeft.x = -1;
	cube.frontBottomLeft.y = -1;
	cube.frontBottomLeft.z = -1;
	
	cube.frontBottomRight.x = 1;
	cube.frontBottomRight.y = -1;
	cube.frontBottomRight.z = -1;
	
	cube.backTopLeft.x = -1;
	cube.backTopLeft.y = 1;
	cube.backTopLeft.z = 1;
	
	cube.backTopRight.x = 1;
	cube.backTopRight.y = 1;
	cube.backTopRight.z = 1;
	
	cube.backBottomLeft.x = -1;
	cube.backBottomLeft.y = -1;
	cube.backBottomLeft.z = 1;
	
	cube.backBottomRight.x = 1;
	cube.backBottomRight.y = -1;
	cube.backBottomRight.z = 1;
}

void Render()
{
	int x;
	int y;
	
	memset(screen, 0, sizeof(screen));
	
	PointConnection(PointProjection(cube.frontTopLeft), PointProjection(cube.frontTopRight));
	PointConnection(PointProjection(cube.frontBottomLeft), PointProjection(cube.frontBottomRight));
	PointConnection(PointProjection(cube.frontTopLeft), PointProjection(cube.frontBottomLeft));
	PointConnection(PointProjection(cube.frontTopRight), PointProjection(cube.frontBottomRight));
	PointConnection(PointProjection(cube.backTopLeft), PointProjection(cube.backTopRight));
	PointConnection(PointProjection(cube.backBottomLeft), PointProjection(cube.backBottomRight));
	PointConnection(PointProjection(cube.backTopLeft), PointProjection(cube.backBottomLeft));
	PointConnection(PointProjection(cube.backTopRight), PointProjection(cube.backBottomRight));
	PointConnection(PointProjection(cube.frontTopLeft), PointProjection(cube.backTopLeft));
	PointConnection(PointProjection(cube.frontTopRight), PointProjection(cube.backTopRight));
	PointConnection(PointProjection(cube.frontBottomLeft), PointProjection(cube.backBottomLeft));
	PointConnection(PointProjection(cube.frontBottomRight), PointProjection(cube.backBottomRight));
	
	for (y = CONSOLE_SIZE; y >= 0; y--)
	{
		for (x = 0; x <= CONSOLE_SIZE; x++)
		{
			if (screen[y][x])
			{
				buffer[((CONSOLE_SIZE - y) * (CONSOLE_SIZE + 1) + x) * 2] = -95;
				buffer[((CONSOLE_SIZE - y) * (CONSOLE_SIZE + 1) + x) * 2 + 1] = -31;
				
				continue;
			}
			
			buffer[((CONSOLE_SIZE - y) * (CONSOLE_SIZE + 1) + x) * 2] = ' ';
			buffer[((CONSOLE_SIZE - y) * (CONSOLE_SIZE + 1) + x) * 2 + 1] = ' ';
		}
	}
	
	printf("%s", buffer);
	
	Gotoxy(0, 0);
}

void Control()
{
	while (1)
	{
		switch (_getch())
		{
		case 'W':
		case 'w':
			axis = 'X';
			direction = 1;
			
			return;
		case 'S':
		case 's':
			axis = 'X';
			direction = -1;
			
			return;
		case 'A':
		case 'a':
			axis = 'Y';
			direction = 1;
			
			return;
		case 'D':
		case 'd':
			axis = 'Y';
			direction = -1;
			
			return;
		case 'Q':
		case 'q':
			axis = 'Z';
			direction = 1;
			
			return;
		case 'E':
		case 'e':
			axis = 'Z';
			direction = -1;
			
			return;
		case 27:
			exit(0);
		}
	}
}

void Update()
{
	Vector3(*rotateFunction)(Vector3, int);
	
	switch (axis)
	{
	case 'X':
		rotateFunction = RotateX;
		
		break;
	case 'Y':
		rotateFunction = RotateY;
		
		break;
	case 'Z':
		rotateFunction = RotateZ;
		
		break;
	default:
		exit(0);
	}
	
	cube.frontTopLeft = rotateFunction(cube.frontTopLeft, direction);
	cube.frontTopRight = rotateFunction(cube.frontTopRight, direction);
	cube.frontBottomLeft = rotateFunction(cube.frontBottomLeft, direction);
	cube.frontBottomRight = rotateFunction(cube.frontBottomRight, direction);
	cube.backTopLeft = rotateFunction(cube.backTopLeft, direction);
	cube.backTopRight = rotateFunction(cube.backTopRight, direction);
	cube.backBottomLeft = rotateFunction(cube.backBottomLeft, direction);
	cube.backBottomRight = rotateFunction(cube.backBottomRight, direction);
}

Vector3 RotateX(Vector3 point, int direction)
{
	return (Vector3){ point.x, point.y * cos(ROTATE * direction) - point.z * sin(ROTATE * direction), point.y * sin(ROTATE * direction) + point.z * cos(ROTATE * direction) };
}

Vector3 RotateY(Vector3 point, int direction)
{
	return (Vector3){ point.z * sin(ROTATE * direction) + point.x * cos(ROTATE * direction), point.y, point.z * cos(ROTATE * direction) - point.x * sin(ROTATE * direction) };
}

Vector3 RotateZ(Vector3 point, int direction)
{
	return (Vector3){ point.x * cos(ROTATE * direction) - point.y * sin(ROTATE * direction), point.x * sin(ROTATE * direction) + point.y * cos(ROTATE * direction), point.z };
}

COORD PointProjection(Vector3 point)
{
	return (COORD){ ((point.x / (point.z + 1 + CAMERA_POINT)) + 1) * (CONSOLE_SIZE / 2), ((point.y / (point.z + 1 + CAMERA_POINT)) + 1) * (CONSOLE_SIZE / 2) };
}

void PointConnection(COORD a, COORD b)
{
	int dx = b.X - a.X;
	int dy = b.Y - a.Y;
	int ix = (dx > 0) - (dx < 0);
	int iy = (dy > 0) - (dy < 0);
	int error;
	
	dx = abs(dx) * 2;
	dy = abs(dy) * 2;
	screen[a.Y][a.X] = TRUE;
	
	if (dx >= dy)
	{
		error = dy - dx / 2;
		
		while (a.X != b.X)
		{
			if (error >= 0)
			{
				if (error != 0 || ix > 0)
				{
					a.Y += iy;
					error -= dx;
				}
			}
			
			a.X += ix;
			error += dy;
			screen[a.Y][a.X] = TRUE;
		}
		
		return;
	}
	
	error = dx - dy / 2;
	
	while (a.Y != b.Y)
	{
		if (error >= 0)
		{
			if (error != 0 || iy > 0)
			{
				a.X += ix;
				error -= dy;
			}
		}
		
		a.Y += iy;
		error += dx;
		screen[a.Y][a.X] = TRUE;
	}
}

void Gotoxy(unsigned short x, unsigned short y)
{
	COORD position = { x, y };
	
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), position);
}

#include <stdio.h>
#include <conio.h>
#include <Windows.h>

#define CONSOLE_WIDTH 160
#define CONSOLE_HEIGHT 45
#define MAP_SIZE 50

// Struct
typedef struct _Vector3
{
	int x;
	int y;
	int z;
} Vector3;

typedef struct _Object
{
	Vector3 position;
	int active;
} Object;

typedef struct _Player
{
	Vector3 position;
	Vector3 rotation;
} Player;

// Gloval Variables
Player player;
Object map[MAP_SIZE * MAP_SIZE * MAP_SIZE];

// Function Prototype
void Init();
void Update();
void Render();
void Gotoxy(int x, int y);
void SetMapObjectActiveXYZ(int x, int y, int z, int active);
int Input();

int main(void)
{
	Init();
	
	while (1)
	{
		Update();
		Render();
		Sleep(5);
	}
	
	return 0;
}

void Init()
{
	int x;
	int y;
	int z;
	
	system("title Console 3D");
	system("mode con cols=160 lines=45");
	
	CONSOLE_CURSOR_INFO consoleCursorInfo;
	
	consoleCursorInfo.bVisible = 0;
	consoleCursorInfo.dwSize = 1;
	
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &consoleCursorInfo);
	
	player.position.x = 0;
	player.position.y = 0;
	player.position.z = 0;
	player.rotation.x = 0;
	player.rotation.y = 0;
	player.rotation.z = 0;
	
	for (z = 0; z < MAP_SIZE; z++)
	{
		for (y = 0; y < MAP_SIZE; y++)
		{
			for (x = 0; x < MAP_SIZE; x++)
			{
				map[x + MAP_SIZE * y + MAP_SIZE * MAP_SIZE * z].position.x = x;
				map[x + MAP_SIZE * y + MAP_SIZE * MAP_SIZE * z].position.y = y;
				map[x + MAP_SIZE * y + MAP_SIZE * MAP_SIZE * z].position.z = z;
				map[x + MAP_SIZE * y + MAP_SIZE * MAP_SIZE * z].active = 0;
			}
		}
	}
	
	for (z = 0; z < 5; z++)
	{
		for (y = 0; y < 5; y++)
		{
			for (x = 0; x < 5; x++)
			{
				SetMapObjectActiveXYZ(x + 10, y + 10, z + 5, 1);
			}
		}
	}
}

void Update()
{
	int key = Input();
	
	if (key == -2)
	{
		return;
	}
	
	switch (key)
	{
	case 'A':
	case 'a':
		player.position.x--;
		
		break;
	case 'D':
	case 'd':
		player.position.x++;
		
		break;
	case 'S':
	case 's':
		player.position.z--;
		
		break;
	case 'W':
	case 'w':
		player.position.z++;
		
		break;
	case 224:
		switch (_getch())
		{
		// Rotate X+
		case 72:
			player.rotation.x++;
			
			break;
		// Rotate Y-
		case 75:
			player.rotation.y--;
			
			break;
			
		// Rotate Y+
		case 77:
			player.rotation.y++;
			
			break;
			
		// Rotate X-
		case 80:
			player.rotation.x--;
			
			break;
		}
		
		break;
	}
}

void Render()
{
	system("cls");
	
	// Player Position X, Y, Z
	Gotoxy(CONSOLE_WIDTH - 33, 1);
	printf("Player Position : [%d, %d, %d]", player.position.x, player.position.y, player.position.z);
	
	// Player Rotation X, Y, Z
	Gotoxy(CONSOLE_WIDTH - 33, 2);
	printf("Player Rotation : [%d, %d, %d]", player.rotation.x, player.rotation.y, player.rotation.z);
	
	int x;
	int y;
	int z;
	
	// Map
	for (z = 0; z < MAP_SIZE; z++)
	{
		for (y = 0; y < MAP_SIZE; y++)
		{
			for (x = 0; x < MAP_SIZE; x++)
			{
				if (map[x + MAP_SIZE * y + MAP_SIZE * MAP_SIZE * z].active && x - player.position.x - (int)(player.rotation.y * 3.6) % 1296 >= 0 && x - player.position.x - (int)(player.rotation.y * 3.6) % 1296 < CONSOLE_WIDTH && y + player.position.z + (((int)(player.rotation.x * 0.5) >> 1) >= 180 ? (int)(player.rotation.x * -0.5) : (int)(player.rotation.x * 0.5)) >= 0 && y + player.position.z + (((int)(player.rotation.x * 0.5) >> 1) >= 180 ? (int)(player.rotation.x * -0.5) : (int)(player.rotation.x * 0.5)) < CONSOLE_HEIGHT)
				{
					Gotoxy(x - player.position.x - (int)(player.rotation.y * 3.6) % 1296, y + player.position.z + (((int)(player.rotation.x * 0.5) >> 1) >= 180 ? (int)(player.rotation.x * -0.5) : (int)(player.rotation.x * 0.5)));
					printf("#");
				}
			}
		}
	}
	
	// Cross Hair
	Gotoxy(CONSOLE_WIDTH >> 1, CONSOLE_HEIGHT >> 1);
	printf("+");
}

void Gotoxy(int x, int y)
{
	COORD position = { x, y };
	
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), position);
}

void SetMapObjectActiveXYZ(int x, int y, int z, int active)
{
	map[x + MAP_SIZE * y + MAP_SIZE * MAP_SIZE * z].active = active;
}

int Input()
{
	return _kbhit() ? _getch() : -2;
}

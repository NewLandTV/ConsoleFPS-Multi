#include <iostream>
#include <vector>
#include <chrono>
#include <Windows.h>
#include <algorithm>

int screenWidth = 120;
int screenHeight = 40;

float playerX = 8.0f;
float playerY = 8.0f;
float playerA = 0.0f;

int mapHeight = 16;
int mapWidth = 16;

float FOV = 3.14159f / 4.0f;
float depth = 16.0f;

int main(void)
{
	system("mode con cols=120 lines=40");

	// Create Screen Buffer
	wchar_t* screen = new wchar_t[screenWidth * screenHeight];

	HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);

	SetConsoleActiveScreenBuffer(hConsole);

	DWORD dwBytesWritten = 0;

	std::wstring map;

	map += L"################";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#..........#...#";
	map += L"#..........#...#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#.......########";
	map += L"#..............#";
	map += L"#..............#";
	map += L"################";

	auto tp1 = std::chrono::system_clock::now();
	auto tp2 = std::chrono::system_clock::now();

	// Game Loop
	while (1)
	{
		tp2 = std::chrono::system_clock::now();

		std::chrono::duration<float> elapsedTime = tp2 - tp1;

		tp1 = tp2;

		float floatElapsedTime = elapsedTime.count();

		// Controls
		// Handle CCW Rotation
		if (GetAsyncKeyState(65) & 0x8000)
		{
			playerA -= 0.8f * floatElapsedTime;
		}
		if (GetAsyncKeyState(68) & 0x8000)
		{
			playerA += 0.8f * floatElapsedTime;
		}
		if (GetAsyncKeyState(87) & 0x8000)
		{
			playerX += sinf(playerA) * 5.0f * floatElapsedTime;
			playerY += cosf(playerA) * 5.0f * floatElapsedTime;

			if (map[(int)playerY * mapWidth + (int)playerX] == '#')
			{
				playerX -= sinf(playerA) * 5.0f * floatElapsedTime;
				playerY -= cosf(playerA) * 5.0f * floatElapsedTime;
			}
		}
		if (GetAsyncKeyState(83) & 0x8000)
		{
			playerX -= sinf(playerA) * 5.0f * floatElapsedTime;
			playerY -= cosf(playerA) * 5.0f * floatElapsedTime;

			if (map[(int)playerY * mapWidth + (int)playerX] == '#')
			{
				playerX += sinf(playerA) * 5.0f * floatElapsedTime;
				playerY += cosf(playerA) * 5.0f * floatElapsedTime;
			}
		}

		for (int x = 0; x < screenWidth; x++)
		{
			// For each column, calculate the projected ray angle into world space
			float rayAngle = (playerA - FOV / 2.0f) + ((float)x / (float)screenWidth) * FOV;

			float distanceToWall = 0.0f;
			bool hitWall = false;
			bool boundary = false;

			// Unit vector for ray in player space
			float eyeX = sinf(rayAngle);
			float eyeY = cosf(rayAngle);

			while (!hitWall && distanceToWall < depth)
			{
				distanceToWall += 0.1f;

				int testX = (int)(playerX + eyeX * distanceToWall);
				int testY = (int)(playerY + eyeY * distanceToWall);

				// Test if ray is out of bounds
				if (testX < 0 || testX >= mapWidth || testY < 0 || testY >= mapHeight)
				{
					// Just set distance to maximum depth
					hitWall = true;
					distanceToWall = depth;
				}
				else
				{
					// Ray is inbounds so test to see if the ray cell is a wall block
					if (map[testY * mapWidth + testX] == '#')
					{
						hitWall = true;

						// distance, dot
						std::vector<std::pair<float, float>> p;

						for (int tx = 0; tx < 2; tx++)
						{
							for (int ty = 0; ty < 2; ty++)
							{
								float vy = (float)testY + ty - playerY;
								float vx = (float)testX + tx - playerX;

								float d = sqrt(vx * vx + vy * vy);

								float dot = (eyeX * vx / d) + (eyeY * vy / d);

								p.push_back(std::make_pair(d, dot));
							}
						}

						// Sort Pairs from closest to farthest
						std::sort(p.begin(), p.end(), [](const std::pair<float, float>& left, const std::pair<float, float>& right) { return left.first < right.first; });

						float bound = 0.01f;

						if (acos(p.at(0).second) < bound)
						{
							boundary = true;
						}
						if (acos(p.at(1).second) < bound)
						{
							boundary = true;
						}
						/*if (acos(p.at(2).second) < bound)
						{
							boundary = true;
						}*/
					}
				}
			}

			// Calculate distance to ceiling and floor
			int ceiling = (float)(screenHeight / 2.0f) - screenHeight / ((float)distanceToWall);
			int floor = screenHeight - ceiling;

			short shade = ' ';

			if (distanceToWall <= depth / 4.0f)
			{
				// Very close
				shade = 0x2588;
			}
			else if (distanceToWall < depth / 3.0f)
			{
				shade = 0x2593;
			}
			else if (distanceToWall < depth / 2.0f)
			{
				shade = 0x2592;
			}
			else if (distanceToWall < depth)
			{
				shade = 0x2591;
			}
			else
			{
				// Too far away
				shade = ' ';
			}

			if (boundary)
			{
				// Black it out
				shade = ' ';
			}

			for (int y = 0; y < screenHeight; y++)
			{
				if (y <= ceiling)
				{
					screen[y * screenWidth + x] = ' ';
				}
				else if (y > ceiling && y <= floor)
				{
					screen[y * screenWidth + x] = shade;
				}
				else
				{
					// Shade floor based on distance
					float b = 1.0f - (((float)y - screenHeight / 2.0f) / ((float)screenHeight / 2.0f));

					if (b < 0.25f)
					{
						shade = '#';
					}
					else if (b < 0.5f)
					{
						shade = 'x';
					}
					else if (b < 0.75f)
					{
						shade = '.';
					}
					else if (b < 0.9f)
					{
						shade = '-';
					}
					else
					{
						shade = ' ';
					}

					screen[y * screenWidth + x] = shade;
				}
			}
		}

		// Display Stats
		swprintf_s(screen, 40, L"X=%3.2f, Y=%3.2f, A=%3.2f FPS=%3.2f ", playerX, playerY, playerA, 1.0f / floatElapsedTime);

		// Display Map
		for (int nx = 0; nx < mapWidth; nx++)
		{
			for (int ny = 0; ny < mapHeight; ny++)
			{
				screen[(ny + 1) * screenWidth + nx] = map[ny * mapWidth + nx];
			}
		}

		screen[((int)playerY + 1) * screenWidth + (int)playerX] = 'P';

		screen[screenWidth * screenHeight - 1] = '\0';

		WriteConsoleOutputCharacter(hConsole, screen, screenWidth * screenHeight, { 0, 0 }, &dwBytesWritten);
	}

	return 0;
}
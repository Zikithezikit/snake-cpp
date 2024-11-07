#include <conio.h>	 //Non-Standard Library that contains console commands.
#include <iostream>	 //Standard Input and Output Library of C++..
#include <windows.h> //Standard Input and Output Library of C++..
#include <string>
#include <limits>
#include <thread>
#include <atomic>
// end include

// namespace
using namespace std;

// Global vars
atomic<bool> runRendering{true};
std::atomic<int> frameCount{0};
std::atomic<int> fps{0};

const int windowWidth = 80;
const int windowHeight = 20;

int snakeHeadCordX, snakeHeadCordY;

int fruitCordX, fruitCordY;

int playerScore;

int snakeTailX[windowWidth + 1], snakeTailY[windowHeight + 1]; // Store the snakes tail (= using the head cords)

int snakeTailLength;

// store snake direction
enum snakesDirection
{
	STOP = 0,
	LEFT,
	RIGHT,
	UP,
	DOWN
};
// var to access snake direction
snakesDirection sDir;

bool isGameOver;

// end Global vars

// init the game
void GameInit()
{
	isGameOver = false;
	sDir = STOP;
	// snake head in center
	snakeHeadCordX = windowWidth / 2;
	snakeHeadCordY = windowHeight / 2;
	// random fruit location
	fruitCordX = rand() % windowWidth;	// 0 in range windowWidth-1
	fruitCordY = rand() % windowHeight; // 0 in range windowHeight-1
	playerScore = 0;
}

// Function for creating the game board & rendering
void GameRender(string playerName)
{
	system("cls"); // Clear the console

	// Creating top walls with '-'
	for (int i = 0; i < windowWidth + 2; i++)
		cout << "-";
	cout << endl;

	for (int i = 0; i < windowHeight; i++)
	{
		for (int j = 0; j <= windowWidth; j++)
		{
			// Creating side walls with '|'
			if (j == 0 || j == windowWidth)
				cout << "|";
			// Creating snake's head with 'O'
			if (i == snakeHeadCordY && j == snakeHeadCordX)
				cout << "O";
			// Creating the sanke's food with '#'
			else if (i == fruitCordY && j == fruitCordX)
				cout << "#";
			// Creating snake's head with 'O'
			else
			{
				bool prTail = false; // is the pixel a tail
				// find previous documented head location to print snake tail
				for (int k = 0; k < snakeTailLength; k++)
				{
					if (snakeTailX[k] == j && snakeTailY[k] == i)
					{
						cout << "o";
						prTail = true;
					}
				}
				if (!prTail)
					cout << " ";
			}
		}
		cout << endl;
	}

	// Creating bottom walls with '-'
	for (int i = 0; i < windowWidth + 2; i++)
		cout << "-";
	cout << endl;

	// Display player's score and FPS
	cout << playerName << "'s Score: " << playerScore << " | FPS: " << fps.load() << endl;
}

// Function for updating the game state
void UpdateGame()
{
	// move old head pos to the tail arrays at the start of the arrays
	int prevX = snakeTailX[0];
	int prevY = snakeTailY[0];
	int prev2X, prev2Y;
	snakeTailX[0] = snakeHeadCordX;
	snakeTailY[0] = snakeHeadCordY;

	for (int i = 1; i < snakeTailLength; i++)
	{
		prev2X = snakeTailX[i];
		prev2Y = snakeTailY[i];
		snakeTailX[i] = prevX;
		snakeTailY[i] = prevY;
		prevX = prev2X;
		prevY = prev2Y;
	}
	// check direction of movement and move accordingly
	switch (sDir)
	{
	case LEFT:
		snakeHeadCordX--;
		break;
	case RIGHT:
		snakeHeadCordX++;
		break;
	case UP:
		snakeHeadCordY--;
		break;
	case DOWN:
		snakeHeadCordY++;
		break;
	}

	// Checks for snake's collision with the wall (|)
	if (snakeHeadCordX >= windowWidth || snakeHeadCordX < 0 || snakeHeadCordY >= windowHeight || snakeHeadCordY < 0)
		isGameOver = true;

	// Checks for collision with the tail (o)
	for (int i = 0; i < snakeTailLength; i++)
	{
		if (snakeTailX[i] == snakeHeadCordX && snakeTailY[i] == snakeHeadCordY)
			isGameOver = true;
	}

	// Checks for snake's collision with the food (#)
	if (snakeHeadCordX == fruitCordX && snakeHeadCordY == fruitCordY)
	{
		playerScore += 10;
		// set new food location
		fruitCordX = rand() % windowWidth;	// 0 in range windowWidth-1
		fruitCordY = rand() % windowHeight; // 0 in range windowHeight-1
		snakeTailLength++;
	}
}

// Function to set the game difficulty level in milisec for wait() func
int SetDifficulty()
{
	int dfc, choice;
	cout << "\nSET DIFFICULTY\n1: Easy\n2: Medium\n3: hard "
			"\nNOTE: if not chosen or pressed any other "
			"key, the difficulty will be automatically set "
			"to medium\nChoose difficulty level: ";

	// safeish cin input
	while (!(cin >> choice))
	{
		cin.clear();										 // clear flags
		cin.ignore(numeric_limits<streamsize>::max(), '\n'); // clear buffer
		cout << "Invalid input. Please enter a number: ";
	}

	switch (choice)
	{
	case '1':
		dfc = 50;
		break;
	case '2':
		dfc = 100;
		break;
	case '3':
		dfc = 150;
		break;
	default:
		dfc = 100;
	}
	return dfc;
}

// Function to handle user UserInput
void UserInput()
{
	// Checks if a key is pressed or not
	if (_kbhit())
	{
		// Getting the pressed key
		switch (_getch())
		{
		case 'a':
			sDir = LEFT;
			break;
		case 'd':
			sDir = RIGHT;
			break;
		case 'w':
			sDir = UP;
			break;
		case 's':
			sDir = DOWN;
			break;
		case 'x':
			isGameOver = true;
			break;
		}
	}
}

// calc fps
void CalculateFPS()
{
	using namespace std::chrono;
	auto start = high_resolution_clock::now();
	while (runRendering)
	{
		std::this_thread::sleep_for(seconds(1));
		auto end = high_resolution_clock::now();
		auto duration = duration_cast<milliseconds>(end - start).count();
		fps.store(frameCount.load() * 1000 / duration);
		frameCount.store(0);
		start = high_resolution_clock::now();
	}
}

int main()
{
	string playerName;
	cout << "enter your name: ";
	while (!(cin >> playerName))
	{
		cin.clear();										 // clear flags
		cin.ignore(numeric_limits<streamsize>::max(), '\n'); // clear buffer
		cout << "Invalid input. Please enter a number: ";
	}
	int dfc = SetDifficulty();

	GameInit();

	// Start the rendering thread
	thread renderThread([&]()
						{
		while (runRendering)
		{
			GameRender(playerName);
			frameCount++;
			// Sleep(5); // control the frame rate }
		} });
	// Start the FPS calculation thread
	thread fpsThread(CalculateFPS);

	while (!isGameOver)
	{
		// GameRender(playerName);
		UserInput();
		UpdateGame();
		// creating a delay for according to the chosen
		// difficulty
		Sleep(dfc); // control the game update rate
	}
	// Stop the rendering thread
	runRendering = false;
	renderThread.join();
	fpsThread.join();

	return 0;
}

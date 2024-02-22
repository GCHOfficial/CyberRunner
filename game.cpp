/*
CyberRunner

George-Cosmin Hanta
*/

#include "raylib.h"
#include <string>

// Structure for textures to be animated
struct AnimData
{
	Rectangle recS; // source rectangle (exact selection of the texture from sprite sheet
	Rectangle recD; // destination rectangle (scaled texture with final position
	Vector2 pos; // origin
	int rot; // rotation
	int v; // velocity
	int frame; // current frame
	float runningTime; // time passed until next frame
	float updateTime; // time at which texture switches to next sprite
	int maxFrame; // maximum amount of sprite in spritesheet
};

// Function for updating animations which goes through each frame in a sprite sheet
AnimData updateAnimations(AnimData data, float deltaTime, bool noRestart = false)
{
	data.runningTime += deltaTime; // update time
	if (data.runningTime >= data.updateTime) // check if it's time to update
	{
		data.recS.x = data.frame * data.recS.width; // change the source rectangle to the next sprite in the spritesheet
		if (data.frame <= data.maxFrame) // check if it's last sprite
		{
			data.frame++; // increment if not
		}
		else if (!noRestart)
		{
			data.frame = 0; // go back to first sprite if yes
		}
		data.runningTime = 0; // reset time until next frame
	}
	return data; // return adjusted animation data
}

int main()
{
	// Window dimensions
	const int windowSize[2] = { 800, 600 };

	// Gravity constant
	const int gravity = 1500;

	// Jump height and in air detection
	const int jumpHeight = 1200;
	bool isJumping = false;

	// Collision detector;
	bool collision = false;

	// Initialize window
	InitWindow(windowSize[0], windowSize[1], "CyberRunner");

	// Main character textures and setup
	Texture2D mcTextures[3] =
	{
		LoadTexture("textures/MC/walk.png"),
		LoadTexture("textures/MC/jump.png"),
		LoadTexture("textures/MC/death.png")
	};
	float mcSpeed = 6.0;
	float mcSpeedIncrement = 0.2;
	AnimData mcData
	{
		{ 0, 0, mcTextures[0].width / 6,  mcTextures[0].height }, // recS
		{ windowSize[0] / 2 - mcTextures[0].width / 6 * 4 / 2, windowSize[1] - mcTextures[0].height * 4, mcTextures[0].width / 6 * 4, mcTextures[0].height * 4 }, // recD
		{ 0, 0 }, // origin
		0, // rot
		0, // v
		0, // frame
		0.0, // running time
		1.0 / 12.0, // update time
		4 // max animation frames
	};
	AnimData mcDataNewGame = mcData;

	// Drone texture and setup
	Texture2D drone = LoadTexture("textures/Drone/roam.png");
	const int numberOfDrones = 3;
	const int droneVelocity[2] = { 275, 400 };
	const int droneStartPosition[2] = { 300, 800 };
	float droneDistanceModifier = 1.0;
	float droneDistanceLimit = 0.5;
	float droneSpeedModifier = 1.0;
	float droneSpeedIncrement = 0.1;
	AnimData drones[numberOfDrones]{};

	// Time variables
	float droneDifficultyUpdateTime = 60.0;
	float droneDifficultyTime = 0.0;

	// Backgrounds
	Texture2D bg = LoadTexture("textures/Backgrounds/bg.png");
	Texture2D b1 = LoadTexture("textures/Backgrounds/b1.png");
	Texture2D b2 = LoadTexture("textures/Backgrounds/b2.png");
	Texture2D b3 = LoadTexture("textures/Backgrounds/b3.png");
	Texture2D b4 = LoadTexture("textures/Backgrounds/b4.png");
	float bgX = 0.0;
	float b1X = 0.0;
	float b2X = 0.0;
	float b3X = 0.0;
	float b4X = 0.0;
	float bgSpeedModifier = 1.0;
	float bgSpeedIncrement = 2.5;

	// Score variables
	int score = 0;
	int scorePerSecond = 100;
	int scoreMultiplier = 2;
	int scoreTick = 1;
	float scoreTime = 0.0;

	// Set target FPS and start game loop until X/Esc are used
	SetTargetFPS(60);
	while (!WindowShouldClose())
	{
		// Drone value initialization and respawn on screen exit
		for (int i = 0; i < numberOfDrones; i++)
		{
			if (drones[i].recD.x <= 0)
			{
				drones[i].recS.x = 0;
				drones[i].recS.y = 0;
				drones[i].recS.width = drone.width / 8;
				drones[i].recS.height = drone.height;
				drones[i].recD.x = windowSize[0] + i * GetRandomValue(droneStartPosition[0], droneStartPosition[1]) * droneDistanceModifier;
				drones[i].recD.y = windowSize[1] - drone.height * 3;
				drones[i].recD.width = drone.width / 8 * 3;
				drones[i].recD.height = drone.height * 3;
				drones[i].pos.x = 0;
				drones[i].pos.y = 0;
				drones[i].rot = 0;
				drones[i].v = GetRandomValue(droneVelocity[0], droneVelocity[1]) * droneSpeedModifier;
				drones[i].frame = 0;
				drones[i].runningTime = 0;
				drones[i].updateTime = 1.0 / 12.0;
				drones[i].maxFrame = 6;
			}
		}

		// Initialize drawing and delta time
		BeginDrawing();
		ClearBackground(WHITE);
		const float dT = GetFrameTime();

		// Initialize backgrounds
		bgX -= 20 * dT * bgSpeedModifier;
		if (bgX <= -bg.width * 2)
		{
			bgX = 0.0;
		}
		Vector2 bg1Pos{ bgX, 0.0 };
		DrawTextureEx(bg, bg1Pos, 0.0, 2.0, WHITE);
		Vector2 bg2Pos{ bgX + bg.width * 2.0, 0.0 };
		DrawTextureEx(bg, bg2Pos, 0.0, 2.0, WHITE);
		b1X -= 40 * dT * bgSpeedModifier;
		if (b1X <= -b1.width * 2)
		{
			b1X = 0.0;
		}
		Vector2 b1Pos{ b1X, 0.0 };
		DrawTextureEx(b1, b1Pos, 0.0, 2.0, WHITE);
		Vector2 b1Pos2{ b1X + b1.width * 2.0, 0.0 };
		DrawTextureEx(b1, b1Pos2, 0.0, 2.0, WHITE);
		b2X -= 60 * dT * bgSpeedModifier;
		if (b2X <= -b2.width * 2)
		{
			b2X = 0.0;
		}
		Vector2 b2Pos{ b2X, 0.0 };
		DrawTextureEx(b2, b2Pos, 0.0, 2.0, WHITE);
		Vector2 b2Pos2{ b2X + b2.width * 2.0, 0.0 };
		DrawTextureEx(b2, b2Pos2, 0.0, 2.0, WHITE);
		b3X -= 80 * dT * bgSpeedModifier;
		if (b3X <= -b3.width * 2)
		{
			b3X = 0.0;
		}
		Vector2 b3Pos{ b3X, 0.0 };
		DrawTextureEx(b3, b3Pos, 0.0, 2.0, WHITE);
		Vector2 b3Pos2{ b3X + b3.width * 2.0, 0.0 };
		DrawTextureEx(b3, b3Pos2, 0.0, 2.0, WHITE);
		b4X -= 100 * dT * bgSpeedModifier;
		if (b4X <= -b4.width * 2)
		{
			b4X = 0.0;
		}
		Vector2 b4Pos{ b4X, 0.0 };
		DrawTextureEx(b4, b4Pos, 0.0, 2.0, WHITE);
		Vector2 b4Pos2{ b4X + b4.width * 2.0, 0.0 };
		DrawTextureEx(b4, b4Pos2, 0.0, 2.0, WHITE);


		// Prevent jumping outside of screen
		if (mcData.recD.y <= 0 && mcData.v < 0)
		{
			mcData.v = 0;
		}

		// Drone difficulty updater
		droneDifficultyTime += dT;
		if (droneDifficultyTime >= droneDifficultyUpdateTime)
		{
			if (droneDistanceModifier > droneDistanceLimit)
			{
				droneDistanceModifier -= 0.1;
				mcSpeed += mcSpeedIncrement;
			}
			droneSpeedModifier += droneSpeedIncrement;
			droneDifficultyTime = 0;
			bgSpeedModifier += bgSpeedIncrement;
			droneDifficultyUpdateTime += droneDifficultyUpdateTime / 4;
			scorePerSecond *= scoreMultiplier;
		}

		// Gravity and ground boundaries
		if (mcData.recD.y >= windowSize[1] - mcData.recD.height)
		{
			mcData.recD.y = windowSize[1] - mcData.recD.height;
			mcData.v = 0;
			isJumping = false;
		}
		else
		{
			mcData.v += gravity * dT;
			isJumping = true;
		}

		// Score keeping
		if (!collision)
		{
			scoreTime += dT;
			if (scoreTime >= scoreTick)
			{
				score += scorePerSecond;
				scoreTime = 0;
			}
		}
		else
		{
			scoreTime = 0;
			scorePerSecond = 100;
		}
		std::string scoreText = "Score: " + std::to_string(score);

		// Player movement
		if (!collision)
		{
			// Jump
			if (IsKeyPressed(KEY_SPACE) && !isJumping)
			{
				mcData.v -= jumpHeight;
			}

			// Walk and side boundaries
			if (IsKeyDown(KEY_D))
			{
				if (mcData.recD.x < windowSize[0] - mcData.recD.width / 2)
				{
					mcData.recD.x += mcSpeed;
				}
			}
			if (IsKeyDown(KEY_A))
			{
				if (mcData.recD.x > 0)
				{
					mcData.recD.x -= mcSpeed;
				}
			}
		}
		else
		{
			// New game
			if (IsKeyPressed(KEY_SPACE))
			{
				mcData = mcDataNewGame;
				collision = false;
				score = 0;
				droneDifficultyTime = 0;
				droneSpeedModifier = 1.0;
				bgSpeedModifier = 1.0;
				droneDistanceModifier = 1.0;
				mcSpeed = 6.0;
				for (int i = 0; i < numberOfDrones; i++)
				{
					drones[i].recS.x = 0;
					drones[i].recS.y = 0;
					drones[i].recS.width = drone.width / 8;
					drones[i].recS.height = drone.height;
					drones[i].recD.x = windowSize[0] + i * GetRandomValue(droneStartPosition[0], droneStartPosition[1]) * droneDistanceModifier;
					drones[i].recD.y = windowSize[1] - drone.height * 3;
					drones[i].recD.width = drone.width / 8 * 3;
					drones[i].recD.height = drone.height * 3;
					drones[i].pos.x = 0;
					drones[i].pos.y = 0;
					drones[i].rot = 0;
					drones[i].v = GetRandomValue(droneVelocity[0], droneVelocity[1]) * droneSpeedModifier;
					drones[i].frame = 0;
					drones[i].runningTime = 0;
					drones[i].updateTime = 1.0 / 12.0;
					drones[i].maxFrame = 6;
				}
			}
		}

		// Update MC position and draw it
		mcData.recD.y += mcData.v * dT;
		if (isJumping)
		{
			mcData = updateAnimations(mcData, dT);
			DrawText(scoreText.c_str(), 10, 10, 40, WHITE);
			DrawText("Use A, D and Space", windowSize[0] - MeasureText("Use A, D and Space", 40) - 10, 10, 40, WHITE);
			DrawTexturePro(mcTextures[1], mcData.recS, mcData.recD, mcData.pos, mcData.rot, WHITE);
		}
		else if (collision)
		{
			mcData = updateAnimations(mcData, dT, true);
			DrawTexturePro(mcTextures[2], mcData.recS, mcData.recD, mcData.pos, mcData.rot, WHITE);
			DrawText(scoreText.c_str(), windowSize[0] / 2 - MeasureText(scoreText.c_str(), 40)/2, windowSize[1] / 2 - 110, 40, WHITE);
			DrawText("You Died!", windowSize[0] / 2 - MeasureText("You Died!", 80) / 2, windowSize[1] / 2 - 50, 80, RED);
			DrawText("Press space for new game", windowSize[0] / 2 - MeasureText("Press space for new game", 40) / 2, windowSize[1] / 2 + 50, 40, WHITE);
		}
		else
		{
			mcData = updateAnimations(mcData, dT);
			DrawText(scoreText.c_str(), 10, 10, 40, WHITE);
			DrawText("Use A, D and Space", windowSize[0] - MeasureText("Use A, D and Space", 40) - 10, 10, 40, WHITE);
			DrawTexturePro(mcTextures[0], mcData.recS, mcData.recD, mcData.pos, mcData.rot, WHITE);
		}

		if (!collision)
		{
			for (int i = 0; i < numberOfDrones; i++)
			{
				drones[i] = updateAnimations(drones[i], dT);
			}
			// Update Drone position and draw it
			for (int i = 0; i < numberOfDrones; i++)
			{
				if (drones[i].recD.x <= -drones[i].recD.width)
				{
					drones[i].recD.x = windowSize[0];
				}
				else
				{
					drones[i].recD.x -= drones[i].v * dT;
				}
				DrawTexturePro(drone, drones[i].recS, drones[i].recD, drones[i].pos, drones[i].rot, WHITE);
			}
		}

		// Check for collision
		for (AnimData d : drones)
		{
			float pad = 20.0;
			Rectangle dRec
			{
				d.recD.x + pad,
				d.recD.y + pad,
				d.recD.width - 6 * pad,
				d.recD.height - 3 * pad,
			};
			Rectangle mcRec
			{
				mcData.recD.x + pad,
				mcData.recD.y + pad,
				mcData.recD.width - 6 * pad,
				mcData.recD.height - 3 * pad,
			};
			if (CheckCollisionRecs(d.recD, mcRec))
			{
				collision = true;
			}
		}

		// Stop drawing
		EndDrawing();
	}
	// Unload textures
	UnloadTexture(mcTextures[0]);
	UnloadTexture(mcTextures[1]);
	UnloadTexture(mcTextures[2]);
	UnloadTexture(drone);
	UnloadTexture(bg);
	UnloadTexture(b1);
	UnloadTexture(b2);
	UnloadTexture(b3);
	CloseWindow();
}
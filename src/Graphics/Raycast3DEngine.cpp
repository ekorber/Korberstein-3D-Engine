#include "Raycast3DEngine.h"
#include "GLRenderer.h"
#include "../Controls/InputControls.h"
#include "../Map.h"
#include "../../images/doors.ppm"
#include "../../images/brick.ppm"
#include <iostream>

#define PI 3.14159f
#define PI2 PI * 0.5f
#define PI3 PI * 1.5f
#define RAD 0.0174533f	// one degree in radians
#define DEG 57.2957795f  // on radian in degrees

static float playerPosX = 200, playerPosY = 100, playerDX = 0, playerDY = 0, playerAngle = 0;
static float moveSpeed = 300;
static float rotSpeed = 4;
static int log2MapBlockSize = 0;

//Initialization
void raycastRendererInit(int screenW, int screenH) {
	init(screenW, screenH);
	log2MapBlockSize = (int) log2(mapBlockSize);
}

float get2DVectorLength(float ax, float ay, float bx, float by) {
	return sqrt((bx - ax) * (bx - ax) + (by - ay) * (by - ay));
}

/*	Sets new angle value, keeping it between 0 and (2 * PI). Wraps around if out of bounds */
float getNewAngleSafely(float newAngle) {
	
	//Prevent new value from going out of bounds
	if (newAngle < 0) {
		newAngle += 2.0f * 3.14159f;
	}
	if (newAngle > 2 * 3.14159) {
		newAngle -= 2.0f * 3.14159f;
	}

	return newAngle;
}

void draw3DMap(float drawThickness = 1.0f) {
	
	const int numRays = screenWidth / drawThickness;
	const int maxCheckDepth = 8;
	
	//The x, y coordinates where the ray hits a grid intersection
	float rayHitX = 0, rayHitY = 0;

	//The x, y distances from one grid intersection to the next
	float xOffset = 0, yOffset = 0;

	//The angle the ray is facing, in radians
	float rayAngle = getNewAngleSafely(playerAngle - RAD * 30.0f);

	setPointSize(drawThickness);
	startDrawingPoints();

	//Render each vertical slice of graphics on-screen
	for (int x = 0; x < numRays; x++) {

		//Precalculate tan-related values
		float rayTanShifted = tan(rayAngle + PI2);
		float rayNegativeTan = -tan(rayAngle);

		//Setup horizontal and vertical ray values
		float distHor = 1000000, distVer = 1000000;
		float horRayPosX = playerPosX, horRayPosY = playerPosY;
		float verRayPosX = playerPosX, verRayPosY = playerPosY;

		int mapTileValueH = 0;
		int mapTileValueV = 0;

		/* ------------------------------------------------- */
		/* Calculate raycast hit for walls running West/East */
		/* ------------------------------------------------- */

		int checkDepth = 0;

		//If facing south
		if (rayAngle > PI) {

			// Round float down to integer, then bit shift digits down to divide by mapBlockSize,
			// and bit shift digits back up into its original slot, to multiply by mapBlockSize.
			
			// We're trying to find the first intersection of the ray and the grid, on the y-axis.
			rayHitY = (((int)playerPosY >> log2MapBlockSize) << log2MapBlockSize) - 0.0001f;

			// Player's x position, factoring in the angle the ray is facing,
			// along with the distance from the grid intersection.
			rayHitX = playerPosX + (rayTanShifted * (playerPosY - rayHitY));

			yOffset = -mapBlockSize;
			xOffset = mapBlockSize * rayTanShifted;
		}

		//If facing north
		if (rayAngle < PI) {

			//Same as above, but just adding mapBlockSize to check the block above the player
			rayHitY = (((int)playerPosY >> log2MapBlockSize) << log2MapBlockSize) + mapBlockSize;

			// Player's x position, factoring in the angle the ray is facing,
			// along with the distance from the grid intersection.
			rayHitX = playerPosX + (rayTanShifted * (playerPosY - rayHitY));

			yOffset = mapBlockSize;
			xOffset = mapBlockSize * -rayTanShifted;
		}

		//If facing directly east or west
		if (rayAngle == 0 || rayAngle == PI || rayAngle == 2 * PI) {
			rayHitX = playerPosX;
			rayHitY = playerPosY;
		}
		

		//Check grid, increasingly further out, until either a wall is found, or maxCheckDepth is reached
		while (checkDepth < maxCheckDepth) {
			int mapX = (int) rayHitX >> log2MapBlockSize;
			int mapY = (int) rayHitY >> log2MapBlockSize;
			int mapIndex = (mapY * mapLengthX) + mapX;

			//If hit wall
			if (mapIndex > 0 && mapIndex < (mapLengthX * mapLengthY) && mapWalls[mapIndex] > 0) {
				checkDepth = maxCheckDepth;

				horRayPosX = rayHitX;
				horRayPosY = rayHitY;

				distHor = get2DVectorLength(playerPosX, playerPosY, rayHitX, rayHitY);

				mapTileValueH = mapWalls[mapIndex];
				
			} else {	//Next grid piece
				rayHitX += xOffset;
				rayHitY += yOffset;
				checkDepth++;
			}
		}



		/* --------------------------------------------------- */
		/* Calculate raycast hit for walls running North/South */
		/* --------------------------------------------------- */

		checkDepth = 0;
		
		//If facing west
		if (rayAngle > PI2 || rayAngle < PI3) {

			// Round float down to integer, then bit shift digits down to divide by mapBlockSize,
			// and bit shift digits back up into its original slot, to multiply by mapBlockSize.

			// We're trying to find the first intersection of the ray and the grid, on the y-axis.
			rayHitX = (((int) playerPosX >> log2MapBlockSize) << log2MapBlockSize) - 0.0001f;

			// Player's x position, factoring in the angle the ray is facing,
			// along with the distance from the grid intersection.
			rayHitY = playerPosY + (rayNegativeTan * (playerPosX - rayHitX));

			xOffset = -mapBlockSize;
			yOffset = mapBlockSize * rayNegativeTan;
		}

		//If facing east
		if (rayAngle < PI2 || rayAngle > PI3) {

			//Same as above, but just adding mapBlockSize to check the block above the player
			rayHitX = (((int)playerPosX >> log2MapBlockSize) << log2MapBlockSize) + mapBlockSize;

			// Player's x position, factoring in the angle the ray is facing,
			// along with the distance from the grid intersection.
			rayHitY = playerPosY + (rayNegativeTan * (playerPosX - rayHitX));

			xOffset = mapBlockSize;
			yOffset = mapBlockSize * -rayNegativeTan;
		}

		//If facing north or south
		if (rayAngle == PI2 || rayAngle == PI3) {
			rayHitX = playerPosX;
			rayHitY = playerPosY;

			//Skips the upcoming loop
			checkDepth = maxCheckDepth;
		}

		//Check grid, increasingly further out, until either a wall is found, or maxCheckDepth is reached
		while (checkDepth < maxCheckDepth) {
			int mapX = (int)rayHitX >> log2MapBlockSize;
			int mapY = (int)rayHitY >> log2MapBlockSize;
			int mapIndex = (mapY * mapLengthX) + mapX;

			//If hit wall
			if (mapIndex > 0 && mapIndex < (mapLengthX * mapLengthY) && mapWalls[mapIndex] > 0) {
				checkDepth = maxCheckDepth;

				verRayPosX = rayHitX;
				verRayPosY = rayHitY;

				distVer = get2DVectorLength(playerPosX, playerPosY, rayHitX, rayHitY);

				mapTileValueV = mapWalls[mapIndex];
			}
			else {	//Next grid piece
				rayHitX += xOffset;
				rayHitY += yOffset;
				checkDepth++;
			}
		}



		/* ------------------------------------------------ */
		/* Select the shorter distance between the two rays */
		/* ------------------------------------------------ */

		float rayDist;
		float shadingValue;

		//Establish shortest distance to wall, between the horizontal and vertical rays
		if (distHor < distVer) {
			rayHitX = horRayPosX;
			rayHitY = horRayPosY;
			rayDist = distHor;
			shadingValue = 0.5;
		}
		else {
			rayHitX = verRayPosX;
			rayHitY = verRayPosY;
			rayDist = distVer;
			shadingValue = 1;
		}




		/* ----------------------------------------------- */
		/* Render the current vertical slice of the screen */
		/* ----------------------------------------------- */
		
		//Get rid of fisheye lens look
		float antiFisheyeAngle = getNewAngleSafely(playerAngle - rayAngle);
		rayDist *= cos(antiFisheyeAngle);

		//Setup 3D walls
		float maxWallHeight = screenHeight;
		float lineHeight = (mapLengthX * mapLengthY * maxWallHeight) / rayDist;
		float textureStep = 32.0f / lineHeight;
		float textureYOffset = 0;

		if (lineHeight > maxWallHeight) {
			textureYOffset = (lineHeight - maxWallHeight) * 0.5f;
			lineHeight = maxWallHeight;
		}
		
		float heightOffset = (maxWallHeight * 0.5f) - (lineHeight * 0.5f);
		
		float textureX;
		float textureY = textureYOffset * textureStep;
		int mapTileValue = 0;

		//Fix potential texture orientation issues
		if (shadingValue == 0.5f) {
			textureX = (int)(rayHitX * 0.5f) % 32;
			
			//To fix x-mirror issue when facing north vs south
			if (rayAngle < PI) {
				textureX = 31 - textureX;
			}

			mapTileValue = mapTileValueH;
		}
		else {
			textureX = (int)(rayHitY * 0.5f) % 32;

			//To fix x-mirror issue when facing north vs south
			if (rayAngle > PI2 && rayAngle < PI3) {
				textureX = 31 - textureX;
			}

			mapTileValue = mapTileValueV;
		}

		//Draw 3D walls
		
		for (int y = 0; y < lineHeight; y++) {
			
			int pixel = ((int)textureY * 32 + (int)textureX) * 3;
			int r = 1, g = 1, b = 1;

			switch (mapTileValue) {
			case 1:
				r = brickTexture[pixel];
				g = brickTexture[pixel + 1];
				b = brickTexture[pixel + 2];
				break;
			case 2:
				r = doorsTexture[pixel];
				g = doorsTexture[pixel + 1];
				b = doorsTexture[pixel + 2];
				break;

			default:
				break;
			}

			setDrawColor(r * shadingValue, g * shadingValue, b * shadingValue);
			drawVertex(x * drawThickness, y + heightOffset);
			textureY += textureStep;
		}

		//Draw floor
		for (int y = lineHeight + heightOffset; y < screenHeight; y++) {
			setDrawColor(100, 100, 100);
			drawVertex(x * drawThickness, y);
		}

		//Draw ceiling
		for (int y = 0; y < heightOffset; y++) {
			setDrawColor(140, 140, 140);
			drawVertex(x * drawThickness, y);
		}

		/* -------------------------------------- */
		/* Prepare for next vertical render slice */
		/* -------------------------------------- */

		//Update angle for next iteration
		rayAngle = getNewAngleSafely(rayAngle + (RAD * drawThickness / 12.0f));
	}
	stopDrawing();
}

void draw2DMap() {

	float x0, y0;

	for (int x = 0; x < mapLengthX; x++) {
		for (int y = 0; y < mapLengthY; y++) {

			//If a wall
			if (mapWalls[(x * mapLengthY) + y] > 1) {
				setDrawColor(255, 255, 255);
			}
			else {
				setDrawColor(0, 0, 0);
			}


			x0 = x * mapBlockSize;
			y0 = y * mapBlockSize;
			
			drawRect(x0, y0, x0 + mapBlockSize, y0 + mapBlockSize, 2);
		}
	}
}

void drawPlayerOn2DMap() {
	setDrawColor(255, 0, 0);

	drawPoint(playerPosX, playerPosY, 8);
	drawLine(playerPosX, playerPosY, playerPosX + (playerDX * 15), playerPosY + (playerDY * 15), 3);
}

/* Useful for checking collisions against walls, or even doors that need to be opened */
bool isTileNearPlayer(float offsetValue, bool checkInFront, bool returnXAxisResult, int mapValue) {
	
	float xOffset = 0;
	float yOffset = 0;

	if (playerDX < 0) {
		xOffset = -offsetValue;
	}
	else {
		xOffset = offsetValue;
	}

	if (playerDY < 0) {
		yOffset = -offsetValue;
	}
	else {
		yOffset = offsetValue;
	}

	int playerGridPosX = playerPosX / mapBlockSize;
	int playerGridPosY = playerPosY / mapBlockSize;

	int gridPosXAddingOffset = (playerPosX + xOffset) / mapBlockSize;
	int gridPosXSubtractingOffset = (playerPosX - xOffset) / mapBlockSize;

	int gridPosYAddingOffset = (playerPosY + yOffset) / mapBlockSize;
	int gridPosYSubtractingOffset = (playerPosY - yOffset) / mapBlockSize;

	int mapIndex;

	if (checkInFront) {
		if (returnXAxisResult) {
			mapIndex = playerGridPosY * mapLengthX + gridPosXAddingOffset;
		}
		else {
			mapIndex = gridPosYAddingOffset * mapLengthX + playerGridPosX;
		}
	}
	else {
		if (returnXAxisResult) {
			mapIndex = playerGridPosY * mapLengthX + gridPosXSubtractingOffset;
		}
		else {
			mapIndex = gridPosYSubtractingOffset * mapLengthX + playerGridPosX;
		}
	}

	return (mapWalls[mapIndex] == mapValue);
}

int getIndexOfTileInFrontOfPlayer(float offsetValue, bool returnXAxisResult) {

	float xOffset = 0;
	float yOffset = 0;

	if (playerDX < 0) {
		xOffset = -offsetValue;
	}
	else {
		xOffset = offsetValue;
	}

	if (playerDY < 0) {
		yOffset = -offsetValue;
	}
	else {
		yOffset = offsetValue;
	}

	int playerGridPosX = playerPosX / mapBlockSize;
	int playerGridPosY = playerPosY / mapBlockSize;

	int gridPosXAddingOffset = (playerPosX + xOffset) / mapBlockSize;
	int gridPosYAddingOffset = (playerPosY + yOffset) / mapBlockSize;
	int mapIndex;

	if (returnXAxisResult) {
		mapIndex = playerGridPosY * mapLengthX + gridPosXAddingOffset;
	}
	else {
		mapIndex = gridPosYAddingOffset * mapLengthX + playerGridPosX;
	}

	return mapIndex;
}

void swapTileIfInFrontOfPlayer(int tileToDestroy, int replacementTile) {
	int index = getIndexOfTileInFrontOfPlayer(30, true);
	if (mapWalls[index] == tileToDestroy) {
		mapWalls[index] = replacementTile;
	}

	index = getIndexOfTileInFrontOfPlayer(30, false);
	if (mapWalls[index] == tileToDestroy) {
		mapWalls[index] = replacementTile;
	}
}

//Loop called every frame
void raycastRenderLoop(float dt) {

	if (leftPressed) {
		playerAngle -= rotSpeed * dt;
		
		if (playerAngle < 0)
			playerAngle += 2 * PI;

		playerDX = cos(playerAngle);
		playerDY = sin(playerAngle);
	}
	if (rightPressed) {
		playerAngle += rotSpeed * dt;

		if (playerAngle >= 2 * PI)
			playerAngle -= 2 * PI;

		playerDX = cos(playerAngle);
		playerDY = sin(playerAngle);
	}
	if (upPressed) {
		
		if (isTileNearPlayer(20, true, true, 0))
			playerPosX += playerDX * moveSpeed * dt;
		if (isTileNearPlayer(20, true, false, 0))
			playerPosY += playerDY * moveSpeed * dt;

		//Open doors
		swapTileIfInFrontOfPlayer(2, 0);
	}
	if (downPressed) {
		
		if (isTileNearPlayer(20, false, true, 0))
			playerPosX -= playerDX * moveSpeed * dt;
		if (isTileNearPlayer(20, false, false, 0))
			playerPosY -= playerDY * moveSpeed * dt;
	}

	//draw2DMap();
	//drawPlayerOn2DMap();
	draw3DMap();
}
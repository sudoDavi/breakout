#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

class BreakOut : public olc::PixelGameEngine{
public:
	BreakOut(){
		sAppName = "Breakout";
	}

private:
	float fBatPos{ 20.0f };
	float fBatWidth{ 40.0f };
	float fBatSpeed{ 0.5f };

	olc::vf2d vBall{ 200.0f, 200.0f };
	olc::vf2d vBallVel{ 200.0f, -100.0f };
	float fBallRadius{ 5.0f };

	float fTargetFrameTime{ 1.0f / 100.0f};
	float fAccumulatedTime{ 0.0f };

public:
	bool OnUserCreate() override{
		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override{
		fAccumulatedTime += fElapsedTime;
		if (fAccumulatedTime >= fTargetFrameTime){
			// Execute the frame
			fAccumulatedTime -= fTargetFrameTime;
			fElapsedTime = fTargetFrameTime;
		}
		else
			return true; // Break out of this frame

		// Clears screen
		Clear(olc::DARK_BLUE);

		// Handle User Input
		// Lerp the bat into the mouse X position
		fBatPos +=
			(fBatSpeed * (static_cast<float>(GetMouseX()) - fBatPos - static_cast<int>(fBatWidth) / 2)) * fElapsedTime;

		// Manage out of bounds bat position
		if (fBatPos < 11.0f) fBatPos = 11.0f;
		if (fBatPos + fBatWidth > static_cast<float>(ScreenWidth()) - 10.0f)
			fBatPos = static_cast<float>(ScreenWidth()) - 10.0f - fBatWidth;

		// Debuggin
		if (GetMouse(0).bHeld){
			if (GetMouseWheel() > 0) fBatSpeed += 0.1f;
			if (GetMouseWheel() < 0) fBatSpeed -= 0.1f;	
		}
		
		if (GetMouse(1).bHeld){
			vBall = { static_cast<float>(GetMouseX()), static_cast<float>(GetMouseY()) };
			if (GetMouseWheel() > 0) fBallRadius += 1.0f;
			if (GetMouseWheel() < 0) fBallRadius -= 1.0f;
		}

		if (GetMouse(2).bPressed) fBallRadius = 5.0f;

		// Update Ball
		vBall += vBallVel * fElapsedTime;

		// Really crude collision detection
		if (vBall.y <= 10.0f) vBallVel.y *= -1.0f;
		if (vBall.x <= 10.0f) vBallVel.x *= -1.0f;
		if (vBall.x >= static_cast<float>(ScreenWidth()) - 10.0f) vBallVel.x *= -1.0f;

		// Check Bat
		if (
			vBall.y >= (static_cast<float>(ScreenHeight() - 20.0f))
			&& (vBall.x > fBatPos)
			&& (vBall.x < fBatPos + fBatWidth)
		)
			vBallVel.y *= -1.0f;

		// Check if ball has gone off screen
		if (vBall.y > ScreenHeight()){
			// Reset ball location
			vBall = { 200.0f, 200.0f };
			float fAngle{ (static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) * 2.0f * 3.14159f };
			vBallVel = { 300.0f * cos(fAngle), 300.0f * sin(fAngle) };
		}
		// Draw Boundary
		DrawLine(10, 10, ScreenWidth() - 10, 10, olc::YELLOW);
		DrawLine(10, 10, 10, ScreenHeight() - 10, olc::YELLOW);
		DrawLine(ScreenWidth() - 10, 10, ScreenWidth() - 10, ScreenHeight() - 10, olc::YELLOW);

		// Draw Bat
		FillRect(static_cast<int>(fBatPos), ScreenHeight() - 20, static_cast<int>(fBatWidth), 10, olc::GREEN);

		// Draw Ball
		DrawCircle(vBall, static_cast<int>(fBallRadius), olc::CYAN);
		return true;
	}
};

int main(){
	BreakOut demo;
	if (demo.Construct(512, 480, 2, 2, false, true))
		demo.Start();
	return 0;
}

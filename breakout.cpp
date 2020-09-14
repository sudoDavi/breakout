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
	float fBallRadius{ 5.0f };

public:
	bool OnUserCreate() override{
		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override{
		// Clears screen
		Clear(olc::DARK_BLUE);

		// Handle User Input
		fBatPos += (fElapsedTime * (static_cast<float>(GetMouseX()) - fBatPos)) * fBatSpeed;

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



		// Draw Boundary
		DrawLine(10, 10, ScreenWidth() - 10, 10, olc::YELLOW);
		DrawLine(10, 10, 10, ScreenHeight() - 10, olc::YELLOW);
		DrawLine(ScreenWidth() - 10, 10, ScreenWidth() - 10, ScreenHeight() - 10, olc::YELLOW);

		// Draw Bat
		FillRect(int(fBatPos), ScreenHeight() - 20, int(fBatWidth), 10, olc::GREEN);

		// Draw Ball
		DrawCircle(vBall, static_cast<int>(fBallRadius), olc::CYAN);
		return true;
	}
};

int main(){
	BreakOut demo;
	if (demo.Construct(512, 480, 2, 2))
		demo.Start();
	return 0;
}

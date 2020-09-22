#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

class BreakOut : public olc::PixelGameEngine{
public:
	BreakOut(){
		sAppName = "Breakout";
	}

private:
	float fBatPos{ 0.0f };
	float fBatWidth{ 40.0f };
	float fBatSpeed{ 3.0f };

	olc::vf2d vBallPos{ 0.0f, 0.0f };
	olc::vf2d vBallDir{ 0.0f, 0.0f };
	float fBallRadius{ 5.0f };
	float fBallSpeed{ 20.0f };

	olc::vi2d vBlockSize{ 16, 16 };
	std::unique_ptr<int[]> blocks;

	std::unique_ptr<olc::Sprite> sprTiles;

	std::unique_ptr<olc::Sprite> sprFragment;
	std::unique_ptr<olc::Decal> decFragment;

	struct sFragment{
		olc::vf2d pos;
		olc::vf2d vel;
		float fAngle;
		float fTime;
		olc::Pixel color;
	};

	std::list<sFragment> listFragments;

public:
	bool OnUserCreate() override{
		blocks = std::make_unique<int[]>(24 * 30);
		for	(int y = 0; y <30; y++){
			for	(int x= 0; x < 24; x++){
				if (x == 0 || y == 0 || x == 23)
					blocks[y * 24 + x] = 10;
				else
					blocks[y * 24 + x] = 0;
				if (x > 2 && x <= 20 && y > 3 && y <= 5)
					blocks[y * 24 + x] = 1;
				if (x > 2 && x <= 20 && y > 5 && y <= 7)
					blocks[y * 24 + x] = 2;
				if (x > 2 && x <= 20 && y > 7 && y <= 9)
					blocks[y * 24 + x] = 3;
			}
		}

		// Load boundary sprite
		sprTiles = std::make_unique<olc::Sprite>("./tut_tiles.png");

		// Load Fragment Sprite
		sprFragment = std::make_unique<olc::Sprite>("./tut_fragment.png");

		// Create decal of fragment
		decFragment = std::make_unique<olc::Decal>(sprFragment.get());

		
		// Ball start
		float fAngle = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 2.0f * 3.14159f;
		vBallDir = { cos(fAngle), sin(fAngle) };
		vBallPos = { 12.5f, 15.5f };
		return true;
	}

	

	bool OnUserUpdate(float fElapsedTime) override{
		// Collision detect
		// Calculate where ball should be, if no collision
		olc::vf2d vPotentialBallPos = vBallPos + vBallDir * fBallSpeed * fElapsedTime;


		// Handle User Input
		// Lerp the bat into position
		fBatPos +=
			((fBatSpeed * (static_cast<float>(GetMouseX()) - fBatPos - static_cast<int>(fBatWidth) / 2)) * fElapsedTime) / vBlockSize.x;

		// Manage out of bounds bat
		// if (fBatPos < 11.0f) fBatPos = 11.0f;
		// if (fBatPos + fBatWidth > static_cast<float>(ScreenWidth()) - 10.0f)
			// fBatPos = static_cast<float>(ScreenWidth()) - 10.0f - fBatWidth;
	
		olc::vf2d vTileBallRadialDims{ fBallRadius / vBlockSize.x, fBallRadius / vBlockSize.y };

		auto TestResolveCollisionPoint = [&](const olc::vf2d& point, olc::vf2d& hitpos, int& id){
				olc::vi2d vTestPoint = vPotentialBallPos + vTileBallRadialDims * point;
				auto& tile = blocks[vTestPoint.y * 24 + vTestPoint.x];

				// Check if ball collided with bat
				if (
					vTestPoint.y >= (static_cast<float>(ScreenHeight() - 20.0f))
					&& (vTestPoint.x > fBatPos)
					&& (vTestPoint.x < fBatPos + fBatWidth)
				)
					vBallDir.x *= -1.0f;

				// Check for tile collisions
				if (tile == 0) // Do nothing, no collision
					return false;
				else{
					// Ball has collided with a tile
					bool bTileHit = tile < 10;
					if (bTileHit){
						id = tile;
						hitpos = { static_cast<float>(vTestPoint.x), static_cast<float>(vTestPoint.y) };
						tile--;
					} 

					// Collision response
					if (point.x == 0.0f)
						vBallDir.y *= -1.0f;
					if (point.y == 0.0f)
						vBallDir.x *= -1.0f;
					return bTileHit;
				}
		};

		// Check if ball has gone off screen
		olc::vf2d vBallScreenPos{ vPotentialBallPos * vBlockSize };
		if (vBallScreenPos.y > ScreenHeight()){
			// Reset ball location
			float fAngle = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 2.0f * 3.14159f;
			vBallDir = { cos(fAngle), sin(fAngle) };
			vBallPos = { 12.5f, 15.5f };
		}

		// Test for hits on 4 points around the ball
		bool bHasHitTile = false;
		olc::vf2d hitPos;
		int hitId{};
		bHasHitTile |= TestResolveCollisionPoint(olc::vf2d(0 , -1), hitPos, hitId);
		bHasHitTile |= TestResolveCollisionPoint(olc::vf2d(0 , +1), hitPos, hitId);
		bHasHitTile |= TestResolveCollisionPoint(olc::vf2d(-1 , 0), hitPos, hitId);
		bHasHitTile |= TestResolveCollisionPoint(olc::vf2d(+1 , 0), hitPos, hitId);

		// Update ball position
			vBallPos += vBallDir * fBallSpeed * fElapsedTime;

		if (bHasHitTile){
			for (int i = 0; i < 100; i++){
				sFragment f;
				f.pos = { hitPos.x + 0.5f, hitPos.y + 0.5f };
				float fAngle = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 2.0f * 3.14159f;
				float fVelocity = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 10.0f;
				f.vel = { fVelocity * cos(fAngle), fVelocity * sin(fAngle) };
				f.fAngle = fAngle;
				f.fTime = 3.0f;
				if (hitId == 1) f.color = olc::RED;
				if (hitId == 2) f.color = olc::GREEN;
				if (hitId == 3) f.color = olc::YELLOW;
				listFragments.push_back(f);
			}
		}


		// End of Collision Code

		// Particle Code
		// Update fragments
		for (auto& f : listFragments){
			f.vel += olc::vf2d(0.0f, 20.0f) * fElapsedTime;
			f.pos += f.vel * fElapsedTime;
			f.fAngle += 5.0f * fElapsedTime;
			f.fTime -= fElapsedTime;
			f.color.a = (f.fTime / 3.0f) * 255;
		}

		// Remove dead fragments
		listFragments.erase(
			std::remove_if(
				listFragments.begin(),
				listFragments.end(),
				[](const sFragment& f){ return f.fTime < 0.0f; }
			),
			listFragments.end()
		);
		// End of Particle Code
		
		// Draw Screen
		Clear(olc::DARK_BLUE);

		// Draw Tiles
		SetPixelMode(olc::Pixel::MASK); // Don't draw pixels which have any transparency i.e A < 255
		for	(int y = 0; y < 30; y++){
			for	(int x = 0; x < 24; x++){
				switch	(blocks[y * 24 + x]){
					case 0: // Break free
						break;
					case 10: // Draw Boundary
						DrawPartialSprite(olc::vi2d(x, y) * vBlockSize, sprTiles.get(), olc::vi2d(0, 0) * vBlockSize, vBlockSize);
						break;
					case 1: // Draw Red Block
						DrawPartialSprite(olc::vi2d(x, y) * vBlockSize, sprTiles.get(), olc::vi2d(1, 0) * vBlockSize, vBlockSize);
						break;
					case 2: // Draw Green Block
						DrawPartialSprite(olc::vi2d(x, y) * vBlockSize, sprTiles.get(), olc::vi2d(2, 0) * vBlockSize, vBlockSize);
						break;
					case 3: // Draw Yellow Block
						DrawPartialSprite(olc::vi2d(x, y) * vBlockSize, sprTiles.get(), olc::vi2d(3, 0) * vBlockSize, vBlockSize);
						break;
				}
				if ((y * 24 + x) != 0)
					DrawRect(olc::vi2d(x, y), olc::vi2d(1, 1), olc::WHITE);
			}
		}
		SetPixelMode(olc::Pixel::NORMAL); // Draw Everything without transparency

		// Draw Ball
		DrawCircle(vBallPos * vBlockSize, fBallRadius, olc::CYAN);

		// Draw Bat
		FillRect(fBatPos * vBlockSize.x, ScreenHeight() - 20, fBatWidth, 10, olc::GREEN);

		// Draw Collider
		DrawRect(fBatPos, ScreenHeight() - 20, fBatWidth, 10, olc::RED);
		DrawRect(vBallPos, { fBallRadius, fBallRadius }, olc::GREY);
		// Draw Decals
		for (auto& f : listFragments)
			DrawRotatedDecal(f.pos * vBlockSize, decFragment.get(), f.fAngle, {4, 4}, {1, 1}, f.color);
		return true;
	}
};

int main(){
	BreakOut demo;
	if (demo.Construct(512, 480, 1, 1))
		demo.Start();
	return 0;
}

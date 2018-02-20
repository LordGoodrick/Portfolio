#include "pch.h"
#include "HeroSprite.h"
#include "BackgroundLoader.h"
#include "GameController.h"
#include "PlatformLoader.h"

#include "Utilities.h"
#include <WICTextureLoader.h>


using namespace std;
using namespace Microsoft::WRL;
using namespace DirectX;
using namespace Windows::Foundation;
using namespace DirectX::SimpleMath;

HeroSprite::HeroSprite(void) : 
	//Sprite sheet texture info:
	// Sheet length = 2304
	// Sheet height = 384
	// Sprite width = 384
	texture ( nullptr )
	, position ( 0.0f, 0.0f )
	, velocity ( 0.0f, 0.0f )
	, tint ( 1.0f, 1.0f, 1.0f, 1.0f )
	, boundingBox ( 0.0f, 0.0f, 84.0f, 177.0f )
	, sourceRect ( nullptr )
	//*****************
	, lives ( 3 )
	, painKillers ( 3 )
	//**************************** Jake Evans
	, spriteBatch ( nullptr )
	, commonStates ( nullptr )
	, frameHeight ( 236 )
	, frameScaledHeight ( 177 )
	, frameWidth ( 112 )
	, frameScaledWidth ( 84 )
	, frame ( 0 )
	, sheetLength ( 6 )
	, spriteEffect ( SpriteEffects_None )
	, frameRate ( 4 )
	, accelerationDueToGravity ( 0.0f, 0.6f )
	, jumping ( false )
	, jumpDistance ( 60 )
	, acceleration ( 0.0f, 0.0f )
	, gamepad ( )
	, highlightEnemy ( false )
	, enemyDead ( false )
	, nextLevel ( false )
	, enemyNumber ( 0 )
	, mindControlling ( false )
	, doorLocked ( false )
	, speed ( 360.0f )
	, scaleX ( 1.0 )
	, scaleY ( 1.0 )
	, jumpStrength ( -10.0f )
	//*************************************** Brandon
	, activatedPad1 (0)
	, activatedPad2 (0)
	, activatedPad3 (0)
	//***********************************************
{
}


HeroSprite::~HeroSprite(void)
{
}

void HeroSprite::Initialize(wstring filePathName, 
							ComPtr<ID3D11Device1> d3dDevice,
							ComPtr<ID3D11DeviceContext1> d3dDeviceContext, float scaleX, float scaleY)
{
	ThrowIfFailed(
		CreateWICTextureFromFile(d3dDevice.Get(), d3dDeviceContext.Get(),
		filePathName.c_str(), 0, &this->texture)
		);

	this->spriteBatch = std::unique_ptr<SpriteBatch>(new SpriteBatch(d3dDeviceContext.Get()));
	this->commonStates = std::unique_ptr<CommonStates>(new CommonStates(d3dDevice.Get()));

	this->scaleX = scaleX;
	this->scaleY = scaleY;

	this->sourceRect = std::unique_ptr<RECT>(new RECT);
	this->sourceRect->left = this->frameWidth * this->frame;
	this->sourceRect->top = 0;
	this->sourceRect->right = this->frameWidth * (this->frame + 1);
	this->sourceRect->bottom = this->frameHeight;


}

void HeroSprite::Update(int tickTotal, int tickDelta, float timeDelta, Rect windowBounds, PlatformLoader& platform1, PlatformLoader& platform2, PlatformLoader& platform3, PlatformLoader& platform4, PlatformLoader& platform5, PlatformLoader& platform6, PlatformLoader& door, PlatformLoader& ladder, PressurePlate& plate1, PressurePlate& plate2, PressurePlate& plate3, Hearts& heart_Asset, PainKiller1& painKiller_Asset, PlatformLoader& chainLift)
{

	if(this->lives > 0 && this->painKillers == 0 && this->heroDead == true)
	{
		this->painKillers += 1;
	}

	//Calculate the current position
	//this->position = this->position  + (timeDelta * this->velocity);

	Vector2 translation = tickDelta * velocity;
	this->position = this->position + translation;

	GameController oldGamepad = this->gamepad;  
	this->gamepad.Update(); 

	//Sprite animation

	bool frameChanged = false;  //Local variable to track frame changes

	if ( tickTotal >= this->lastFrameChange + this->frameRate)
	{
		//Time to change frame
		this->frame = this->frame + 1;
		frameChanged = true;

		if (this->frame >= this->sheetLength)
		{
			this->frame= 0;
		}

		this->sourceRect->left = this->frameWidth * this->frame;
		this->sourceRect->right = this->frameWidth * (this->frame + 1); 

	}

	if ( frameChanged )
	{
		this->lastFrameChange = tickTotal;
	}



	//Adjust the bounding box
	this->boundingBox.X = this->position.x;
	this->boundingBox.Y = this->position.y;

	if ( this->boundingBox.Bottom >= windowBounds.Bottom)// && velocity.y > 0.0f )
	{
		this->heroDead = true;
		this->lives--;
		this->velocity.y = 0.0f;
		this->acceleration.y = 0.0f;
		//this->position.y = windowBounds.Bottom - this->frameScaledHeight;
		this->jumping = false;
	}
	else if ( this->boundingBox.Right >= windowBounds.Right )
	{
		this->velocity.x = 0;
		this->position.x = windowBounds.Right - this->boundingBox.Width;
	}
	else if ( this->boundingBox.Left <= windowBounds.Left )
	{
		this->velocity.x = 0;
		this->position.x = windowBounds.Left;
	}


	//Check for collisions
	if (this->jumping)
	{
		//The hero is moving in two dimensions, and its vertical motion is affected
		//  by acceleration due to gravity, so we must use different equations of motion
		//  to calculate the distance travelled

		//First calculate the current velocity of the sprite
		Vector2 initialVelocity = this->velocity;
		this->velocity = initialVelocity + (tickDelta * this->acceleration);

		//Second calculate the distance moved
		Vector2 distanceTravelled = 0.5f * (initialVelocity + this->velocity) * tickDelta;

		//Third, calculate the new position of the sprite
		this->position = this->position + distanceTravelled;
		if ( this->boundingBox.Top <= windowBounds.Top )
		{
			//Ball has moved off the top edge of the window
			this->velocity.y = 10; //Change direction of motion
			this->position.y = windowBounds.Top;      //Ensure sprite is fully within the window
			/* NOTE We could also adjust the bounding box, but this gets done in the next
			* game loop after adjusting the position, so there is no need.
			* */
		}
		if ( this->boundingBox.Bottom >= windowBounds.Height)// && velocity.y > 0.0f )
		{
			this->heroDead = true;
			this->lives--;
			this->HeroDead();
			this->velocity.y = 0.0f;
			this->acceleration.y = 0.0f;
			//this->position.y = windowBounds.Height - this->boundingBox.Height;
			this->jumping = false;
		}
		if ( this->boundingBox.Right >= windowBounds.Width )
		{
			this->velocity.x = 0;
			this->position.x = windowBounds.Width - this->boundingBox.Width;
		}
		if ( this->boundingBox.Left <= windowBounds.Left )
		{
			this->position.x = 0;
			this->velocity.x = 0;

		}

	}

	//Level Platform Collision

	if ( platform1.IntersectsWith(this->boundingBox) && this->velocity.y > 0.0f)// && this->position.y <= platform1.GetBoundingBox().Top - this->frameScaledHeight)
	{
		this->velocity.y = 0.0f;
		this->position.y = platform1.GetBoundingBox().Top - this->frameScaledHeight;
		this->jumping = false;
	}
	if ( platform1.IntersectsWith(this->boundingBox) && this->velocity.y < 0.0f)
	{
		this->velocity.y = 0.0f;
		this->position.y = platform1.GetBoundingBox().Bottom + 1.0f;
	}
	if ( platform2.IntersectsWith(this->boundingBox) && this->velocity.y > 0.0f)
	{
		this->velocity.y = 0.0f;
		this->position.y = platform2.GetBoundingBox().Top - this->frameScaledHeight;
		this->jumping = false;
	}
	if ( platform2.IntersectsWith(this->boundingBox) && this->velocity.y < 0.0f)
	{
		this->velocity.y = 0.0f;
		this->position.y = platform2.GetBoundingBox().Bottom + 1.0f;
	}

	if ( platform3.IntersectsWith(this->boundingBox) && this->velocity.y > 0.0f)
	{
		this->velocity.y = 0.0f;
		this->position.y = platform3.GetBoundingBox().Top - this->frameScaledHeight;
		this->jumping = false;
	}
	if ( platform3.IntersectsWith(this->boundingBox) && this->velocity.y < 0.0f)
	{
		this->velocity.y = 0.0f;
		this->position.y = platform3.GetBoundingBox().Bottom + 1.0f;
	}

	if ( platform4.IntersectsWith(this->boundingBox) && this->velocity.y > 0.0f)
	{
		this->velocity.y = 0.0f;
		this->position.y = platform4.GetBoundingBox().Top - this->frameScaledHeight;
		this->jumping = false;
	}
	if ( platform4.IntersectsWith(this->boundingBox) && this->velocity.y < 0.0f)
	{
		this->velocity.y = 0.0f;
		this->position.y = platform4.GetBoundingBox().Bottom + 1.0f;
	}

	if ( platform5.IntersectsWith(this->boundingBox) && this->velocity.y > 0.0f)
	{
		this->velocity.y = 0.0f;
		this->position.y = platform5.GetBoundingBox().Top - this->frameScaledHeight;
		this->jumping = false;
	}
	if ( platform5.IntersectsWith(this->boundingBox) && this->velocity.y < 0.0f)
	{
		this->velocity.y = 0.0f;
		this->position.y = platform5.GetBoundingBox().Bottom + 1.0f;
	}
	if ( platform6.IntersectsWith(this->boundingBox) && this->velocity.y > 0.0f)
	{
		this->velocity.y = 0.0f;
		this->position.y = platform6.GetBoundingBox().Top - this->frameScaledHeight;
		this->jumping = false;
	}
	if ( platform6.IntersectsWith(this->boundingBox) && this->velocity.y < 0.0f)
	{
		this->velocity.y = 0.0f;
		this->position.y = platform6.GetBoundingBox().Bottom + 1.0f;
	}

	if ( ladder.IntersectsWith(this->boundingBox) && this->highlightEnemy == false)
	{
		Vector2 leftThumbstick = gamepad.GetThumbstick(XINPUT_GAMEPAD_LEFT_THUMB);
		this->position.y = this->position.y + (leftThumbstick.y * this->speed * timeDelta);
	}

	//*************************************************************************
	if(plate1.IntersectsWith(this->boundingBox))
	{
		this->position.y = plate1.GetBoundingBox().Top - this->frameScaledHeight;
		this->velocity.y = 0.0f;
		this->velocity.x = 0.0f;
		this->jumping = false;


	}
	if(plate2.IntersectsWith(this->boundingBox))
	{
		this->position.y = plate2.GetBoundingBox().Top - this->frameScaledHeight;
		this->velocity.y = 0.0f;
		this->velocity.x = 0.0f;
		this->jumping = false;
	}
	if(plate3.IntersectsWith(this->boundingBox))
	{
		this->position.y = plate3.GetBoundingBox().Top - this->frameScaledHeight;
		this->velocity.y = 0.0f;
		this->velocity.x = 0.0f;
		this->jumping = false;
	}
	//************************************************************************

	if(heart_Asset.IntersectsWith(this->boundingBox) && this->lives < 3)
	{
		heart_Asset.SetVisiblity(false);
		heart_Asset.Reset(Vector2(0.0f, 1080.0f));
		heart_Asset.SetBoundingBox(Windows::Foundation::Rect (0.0f, 1080.0f, 0.0f, 0.0f));
		this->lives++;

	}
	if(painKiller_Asset.IntersectsWith(this->boundingBox) && this->painKillers < 3)
	{
		painKiller_Asset.SetVisiblity(false);
		painKiller_Asset.Reset(Vector2(0.0f, 1080.0f));
		painKiller_Asset.SetBoundingBox(Windows::Foundation::Rect (0.0f, 1080.0f, 0.0f, 0.0f));
		this->painKillers++;
	}

	//Detect if colides with exit door and switch levels
	if ( door.IntersectsWith(this->boundingBox) && this->doorLocked == false)
	{
		this->nextLevel = true;
	}
	else
	{

	}

	if (this->jumping == false && boundingBox.Bottom < windowBounds.Bottom && !platform1.IntersectsWith(this->boundingBox) && !platform2.IntersectsWith(this->boundingBox) && !platform3.IntersectsWith(this->boundingBox) && !platform4.IntersectsWith(this->boundingBox) && !platform5.IntersectsWith(this->boundingBox) && !platform6.IntersectsWith(this->boundingBox) && !ladder.IntersectsWith(this->boundingBox) && !plate1.IntersectsWith(this->boundingBox) && !plate2.IntersectsWith(this->boundingBox) && !plate3.IntersectsWith(this->boundingBox))
	{
		this->velocity.y = 10.0f;
		this->jumping = true;
	}

	if ( chainLift.IntersectsWith(this->boundingBox) && this->velocity.y >= 0.0f)
	{
		this->velocity.y = 0.0f;
		this->position.y = chainLift.GetBoundingBox().Top - this->frameScaledHeight;
		this->jumping = false;
	}

	if( this->gamepad.IsButtonDown(XINPUT_GAMEPAD_X) 
			&& !oldGamepad.IsButtonDown(XINPUT_GAMEPAD_X))
		{
			this->lives -= 1;
		}
		else
		{

		}

	//Gamepad Input


	if(this->enemyDead == false)
	{


	}

	if (highlightEnemy == false)
	{
		Vector2 leftThumbstick = gamepad.GetThumbstick(XINPUT_GAMEPAD_LEFT_THUMB);
		this->position.x = this->position.x + (leftThumbstick.x * this->speed * timeDelta);

		if (leftThumbstick.x * this->speed * timeDelta > 0 )
		{
			this->spriteEffect = SpriteEffects_None;
		}
		if (leftThumbstick.x * this->speed * timeDelta < 0 )
		{
			this->spriteEffect = SpriteEffects_FlipHorizontally;
		}

		//Code for making the hero jump
		if (this->jumping)
		{

			//Calculate the current velocity of the sprite
			Vector2 initialVelocity = this->velocity;
			this->velocity = initialVelocity + (tickDelta * this->acceleration);

			//Calculate the distance moved
			Vector2 distanceTravelled = 0.5f * (initialVelocity + this->velocity) * tickDelta;

			//Calculate the new position of the sprite
			this->position = this->position + distanceTravelled;
		}



		else if( this->gamepad.IsButtonDown (XINPUT_GAMEPAD_A)
			&& !oldGamepad.IsButtonDown (XINPUT_GAMEPAD_A))
		{
			//Jump
			this->position.y = this->position.y - 1.0f;
			this->velocity.y = this->jumpStrength;
			this->jumping = true;	
			this->acceleration = this->accelerationDueToGravity;
		}

		else
		{
			//Constant velocity, so use the orginal calculation
			Vector2 distanceTravelled = tickDelta * this->velocity;
			this->position = this->position + distanceTravelled;
		}
	}
	else
	{
		
	}

}

void HeroSprite::Draw()
{
	XMMATRIX cameraMatrix = Matrix::CreateTranslation(0, 0, 0);
	cameraMatrix *=
		Matrix::CreateTranslation(0,0,0) * 
		Matrix::CreateScale(this->scaleX, this->scaleY, 1.0);
	this->spriteBatch->Begin(SpriteSortMode_Deferred, this->commonStates->NonPremultiplied(), nullptr, nullptr, nullptr, nullptr, cameraMatrix);
	this->spriteBatch->Draw(this->texture.Get(), this->position, this->sourceRect.get(), this->tint, 0.0f, SimpleMath::Vector2(0.0f, 0.0f), 0.75f, this->spriteEffect, 0.0f);
	this->spriteBatch->End();

}

void HeroSprite::Reset(DirectX::SimpleMath::Vector2 startPosition, bool unhighlightEnemy, bool reviveEnemy)
{
	this->position = startPosition;
	this->highlightEnemy = unhighlightEnemy;
	this->enemyDead = reviveEnemy;

	this->boundingBox.X = this->position.x;
	this->boundingBox.Y = this->position.y;
	this->nextLevel = false;
	this->heroDead = false;
}

bool HeroSprite::NextLevel()
{
	return this->nextLevel;
}

bool HeroSprite::HeroDead()
{
	//this->lives--;
	return this->heroDead;
}

void HeroSprite::HighlightEnemy(bool highlightEnemy)
{
	this->highlightEnemy = highlightEnemy;
}

void HeroSprite::NewSheetLength(int sheetLength)
{
	this->sheetLength = sheetLength;
}


//************************************************************************** Brandon
int HeroSprite::IntersectsPad1(PressurePlate& plate1)
{
	if(plate1.IntersectsWith(this->boundingBox))
	{
		return activatedPad1 = 1;
	}

	else
	{
		return activatedPad1 = 0;
	}

}

int HeroSprite::IntersectsPad2(PressurePlate& plate2)
{
	if(plate2.IntersectsWith(this->boundingBox))
	{
		return activatedPad2 = 1;
	}

	else
	{
		return activatedPad2 = 0;
	}

}

int HeroSprite::IntersectsPad3(PressurePlate& plate3)
{
	if(plate3.IntersectsWith(this->boundingBox))
	{
		return activatedPad3 = 1;
	}

	else
	{
		return activatedPad3 = 0;
	}

}


//***************************************************
int HeroSprite::ReturnLives() 
{
	return this->lives;
}

int HeroSprite::ReturnPainKillers()
{
	return this->painKillers;
}
//***************************************************** Jake Evans

int HeroSprite::DeathByEnemy1(Enemy& enemy1)
{
	if(enemy1.IntersectsWith(this->boundingBox))
	{
		this->lives--;
		this->heroDead = true;


	}
	return this->lives;		
}

int HeroSprite::DeathByEnemy2(Enemy& enemy2)
{
	if(enemy2.IntersectsWith(this->boundingBox))
	{
		this->lives--;
		this->heroDead = true;

	}
	return this->lives;		
}

int HeroSprite::DeathByEnemy3(Enemy& enemy3)
{
	if(enemy3.IntersectsWith(this->boundingBox))
	{
		this->lives--;
		this->heroDead = true;

	}
	return this->lives;		
}

void HeroSprite::PainKillers()
{

	this->painKillers--;




}

void HeroSprite::ResetLives()
{
	this->lives = 3;
}

void HeroSprite::ResetPainkillers()
{
	this->painKillers = 3;
}

void HeroSprite::LockDoor(bool lockDoor)
{
	this->doorLocked = lockDoor;
}
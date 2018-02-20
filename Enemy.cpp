#include "pch.h"
#include "Enemy.h"
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

Enemy::Enemy(void) : 
	//Sprite sheet texture info:
	// Sheet length = 2304
	// Sheet height = 384
	// Sprite width = 384
	texture ( nullptr )
	, arrow ( nullptr )
	, position (0.0f, 532.0f )
	, velocity ( 0.0f, 0.0f )
	, tint ( 1.0f, 1.0f, 1.0f, 1.0f )
	, boundingBox (0.0f, 0.0f, 124.0f, 166.0f )
	, sourceRect ( nullptr )
	, level ( 1 )
	, spriteBatch ( nullptr )
	, commonStates ( nullptr )
	, frameWidth ( 176 )
	, frameScaledWidth ( 124 )
	, frameHeight ( 236 )
	, frameScaledHeight ( 166 )
	, frame ( 0 )
	, sheetLength ( 6 )
	, spriteEffect ( SpriteEffects_None )
	, frameRate ( 4 )
	, accelerationDueToGravity ( 0.0f, 0.6f )
	, jumping ( false )
	, jumpDistance ( 60 )
	, acceleration ( 0.0f, 0.0f )
	, gamepad ( )
	, indicator ( false )
	, halfArrow ( 16 )
	, arrowHeight ( 20 )
	, arrowColor ( 1.0f, 1.0f, 1.0f, 0.0f )
	, highlightEnemy ( false )
	, enemyDead ( false )
	, enemySelected ( true )
	, speed ( 360.0f )
	//*************************************** Brandon
	, activatedPad1 (0)
	, activatedPad2 (0)
	, activatedPad3 (0)
	, detectionBoundingBox (0.0f, 0.0f, 624.0f, 166.0f)
	, scaleX ( 1.0 )
	, scaleY ( 1.0 )
	//AI variables 
	, enemyNumber ( 0 )
	, xVelocity(5.0f) 
	, yVelocity(-5.0f)
	, jumpStrength ( -10.0f )
	, arrowPosition ( 0.0f, 0.0f )
{
}


Enemy::~Enemy(void)
{
}

void Enemy::Initialize(wstring filePathName, 
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

	ThrowIfFailed(
		CreateWICTextureFromFile(d3dDevice.Get(), d3dDeviceContext.Get(),
		L"Assets/arrow.png", 0, &this->arrow)
		);

	this->sourceRect = std::unique_ptr<RECT>(new RECT);
	this->sourceRect->left = this->frameWidth * this->frame;
	this->sourceRect->top = 0;
	this->sourceRect->right = this->frameWidth * (this->frame + 1);
	this->sourceRect->bottom = this->frameHeight;
	//this->headTexture = (L"Assets/bugdeathspriteheadplaceholder.png", this->d3dDevice, this->d3dDeviceContext);

	if (this->level == 1)
	{
		this->spriteEffect = SpriteEffects_FlipHorizontally;
	}
	if (this->level == 2)
	{
		this->spriteEffect = SpriteEffects_FlipHorizontally;
	}
	if (this->level == 3)
	{
		this->spriteEffect = SpriteEffects_FlipHorizontally;
	}
	if (this->level == 4)
	{
		this->spriteEffect = SpriteEffects_FlipHorizontally;
	}
	if (this->level == 5)
	{
		this->spriteEffect = SpriteEffects_None;
	}
	if (this->level == 6)
	{
		this->spriteEffect = SpriteEffects_None;
	}
	if (this->level == 7)
	{
		this->spriteEffect = SpriteEffects_FlipHorizontally;
	}
	if (this->level == 8)
	{
		this->spriteEffect = SpriteEffects_None;
	}
	if (this->level == 9)
	{
		this->spriteEffect = SpriteEffects_FlipHorizontally;
	}
}

void Enemy::Update(int tickTotal, int tickDelta, float timeDelta, Rect windowBounds, PlatformLoader& platform1, PlatformLoader& platform2, PlatformLoader& platform3, PlatformLoader& platform4, PlatformLoader& platform5, PlatformLoader& platform6, PlatformLoader& ladder, PressurePlate& plate1, PressurePlate& plate2, PressurePlate& plate3)
{
	//Calculate the current position
	//this->position = this->position  + (timeDelta * this->velocity);



	Vector2 translation = tickDelta * velocity;
	this->position = this->position + translation;

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

	if (this->spriteEffect == SpriteEffects_None && this->enemyDead == false)
	{
		this->detectionBoundingBox.X = this->position.x;
		this->detectionBoundingBox.Y = this->position.y;
	}
	else if (this->spriteEffect == SpriteEffects_FlipHorizontally && this->enemyDead == false)
	{
		this->detectionBoundingBox.X = this->position.x - 500.0f;
		this->detectionBoundingBox.Y = this->position.y;
	}
	else if (this->enemyDead == true)
	{
		this->detectionBoundingBox.X = 0.0f - 700.0f;
		this->detectionBoundingBox.Y = 0.0f - 200.0f;
	}

	if ( this->boundingBox.Bottom >= windowBounds.Bottom)// && velocity.y > 0.0f )
	{

		this->velocity.y = 0.0f;
		//this->acceleration.y = 0.0f;
		this->position.y = windowBounds.Bottom - this->frameScaledHeight;
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
		//The enemy is moving in two dimensions, and its vertical motion is affected
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
			this->velocity.y = 0; //Change direction of motion
			this->position.y = windowBounds.Top;      //Ensure sprite is fully within the window
			/* NOTE We could also adjust the bounding box, but this gets done in the next
			* game loop after adjusting the position, so there is no need.
			* */
		}

		if ( this->boundingBox.Bottom >= windowBounds.Height)// && velocity.y > 0.0f )
		{

			this->velocity.y = 0.0f;
			//this->acceleration.y = 0.0f;
			this->position.y = windowBounds.Height - this->boundingBox.Height;
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

	if (this->jumping == false && boundingBox.Bottom < windowBounds.Bottom && !platform1.IntersectsWith(this->boundingBox) && !platform2.IntersectsWith(this->boundingBox) && !platform3.IntersectsWith(this->boundingBox) && !platform4.IntersectsWith(this->boundingBox) && !platform5.IntersectsWith(this->boundingBox) && !platform6.IntersectsWith(this->boundingBox) && !plate1.IntersectsWith(this->boundingBox) && !plate2.IntersectsWith(this->boundingBox) && !plate3.IntersectsWith(this->boundingBox))
	{
		this->velocity.y = 10.0f;
		this->jumping = true;
	}


	//Gamepad Input
	GameController oldGamepad = this->gamepad;  
	this->gamepad.Update(); 

	//Highlight enemy to control
	if (enemyDead == false)
	{


		if (highlightEnemy == true)
		{
			this->arrowPosition.x = this->position.x + 46;
			this->arrowPosition.y = this->position.y - 16;

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

			//Code for making the Enemy jump
			if (this->jumping)
			{
				//First calculate the current velocity of the sprite
				Vector2 initialVelocity = this->velocity;
				this->velocity = initialVelocity + (tickDelta * this->acceleration);

				//Second calculate the distance moved
				Vector2 distanceTravelled = 0.5f * (initialVelocity + this->velocity) * tickDelta;

				//Third, calculate the new position of the sprite
				this->position = this->position + distanceTravelled;

				if( this->gamepad.IsButtonDown (XINPUT_GAMEPAD_A)
					&& !oldGamepad.IsButtonDown (XINPUT_GAMEPAD_A) && jumping == true )
				{
					//Jump
					this->position.y = this->position.y - 1.0f;
					this->velocity.y = this->jumpStrength;
					this->jumping = true;	
					this->acceleration = this->accelerationDueToGravity;
					Vector2 distanceTravelled = tickDelta * this->velocity;
					this->position = this->position + distanceTravelled;
				}
			}

			else if( this->gamepad.IsButtonDown (XINPUT_GAMEPAD_A)
				&& !oldGamepad.IsButtonDown (XINPUT_GAMEPAD_A) )
			{
				//Jump
				this->position.y = this->position.y - 1.0f;
				this->velocity.y = -10.0f;
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



		// Put the enemy AI in here 
		else
		{ 
			if (this->level == 1) 
			{
				if(this->enemyNumber == 1)
				{

					if(this->spriteEffect == SpriteEffects_None && this->position.x >= 1267.0f)
					{
						this->spriteEffect = SpriteEffects_FlipHorizontally;
					}

					if(this->position.x < 1200.0f && this->spriteEffect == SpriteEffects_FlipHorizontally)
					{
						float distanceToMove = tickDelta * xVelocity; 
						this->position.x = position.x - distanceToMove;

						if(this->position.x <= 50.0f && this->spriteEffect == SpriteEffects_FlipHorizontally)
						{
							float distanceToMove = tickDelta * xVelocity; 
							this->position.x = position.x - distanceToMove;

							this->spriteEffect = SpriteEffects_None;
						}
					}

					if(this->position.x < 1200.0f && this->spriteEffect == SpriteEffects_None)
					{
						float distanceToMove = tickDelta * xVelocity; 
						this->position.x = position.x + distanceToMove;

						if(this->position.x >= 1200.0f && this->spriteEffect == SpriteEffects_None)
						{
							float distanceToMove = tickDelta * xVelocity; 
							this->position.x = position.x - distanceToMove;

							this->spriteEffect = SpriteEffects_FlipHorizontally;
						}
					}
				}
			}
			//******************************************************************* 
			else if (this->level == 2) 
			{    

				if(this->enemyNumber == 2) 
				{ 
					float distanceToMove = tickDelta * xVelocity; 
					this->position.x = position.x + distanceToMove; 
					if(position.x > 1050.0f && xVelocity > 0.0f) 
					{ 
						this->xVelocity = this->xVelocity * -1.0f; 
						this->spriteEffect = SpriteEffects_FlipHorizontally; 
					} 
					if(this->position.x <  812.0f && xVelocity < 0.0f) 
					{ 
						this->xVelocity = this->xVelocity * -1.0f; 
						this->spriteEffect = SpriteEffects_None; 

					} 
				} 
				if(this->enemyNumber == 1) 
				{
					float distanceToMove = tickDelta * xVelocity; 
					this->position.x = position.x + distanceToMove; 
					if(this->position.x + this->frameWidth >= windowBounds.Width && xVelocity > 0.0f)
					{
						this->xVelocity = this->xVelocity * -1.0f;
						this->spriteEffect = SpriteEffects_FlipHorizontally;
					}
					if(this->position.x < 713.0f && xVelocity < 0.0f)
					{
						this->xVelocity = this->xVelocity * -1.0f;
						this->spriteEffect = SpriteEffects_None;
					}

				} 

			} 
			else if (this->level == 3) 
			{ 
				if(enemyNumber == 1) 
				{ 
					float distanceToMove = tickDelta * this->xVelocity; 
					this->position.x = this->position.x + distanceToMove; 

					//if(this->position.x + this->frameWidth >=  windowBounds.Width && xVelocity > 0.0f) 
					//{ 
					//	//the player has moved off the right edge of the game window 
					//	this->xVelocity = this->xVelocity * -1.0f; 
					//	this->spriteEffect = SpriteEffects_FlipHorizontally; 
					//} 

					if(this->position.x >= 1299.0f && xVelocity > 0.0f) 
					{ 
						this->xVelocity = xVelocity * -1.0f; 
					} 

					if (this->xVelocity < 0 && this->position.x <= 0 && xVelocity < 0.0f) 
					{ 
						//the player has returned to the starting point 
						this->xVelocity = xVelocity * -1; 
					} 

				} 


			} 
			else if (this->level == 4) 

			{ 


				if(enemyNumber == 1)
				{
					float distanceToMove = tickDelta * this->xVelocity; 
					this->position.x = this->position.x + distanceToMove;

					if(this->position.x + this->frameWidth >=  windowBounds.Width && xVelocity > 0.0f) 
					{ 
						//the player has moved off the right edge of the game window 
						this->xVelocity = this->xVelocity * -1.0f; 
						this->spriteEffect = SpriteEffects_FlipHorizontally; 
					} 
					if(this->position.x <= 1090.0f && xVelocity < 0.0f) 
					{ 
						this->xVelocity = this->xVelocity * -1.0f; 
						this->spriteEffect = SpriteEffects_None; 
					} 

				}

				if(enemyNumber == 2)
				{  float distanceToMove = tickDelta * this->xVelocity; 
				this->position.x = this->position.x + distanceToMove;
				if (this->position.x > 1312.0f && xVelocity > 0.0f) 
				{ 

					this->xVelocity = xVelocity * -1; 
					this->spriteEffect = SpriteEffects_FlipHorizontally; 
				}
				if(this->position.x <= 588.0f && xVelocity < 0.0f)
				{
					this->xVelocity = xVelocity *- 1;
					this->spriteEffect = SpriteEffects_None; 

				}

				}
			} 
			else if (this->level == 5) 
			{ 

				//if(enemyNumber == 1)
				//{
				//	float distanceToMove = tickDelta * this->xVelocity; 
				//	this->position.x = this->position.x + distanceToMove;

				//	if(this->position.x + this->frameWidth >=  windowBounds.Width && xVelocity > 0.0f) 
				//	{ 
				//		//the player has moved off the right edge of the game window 
				//		this->xVelocity = this->xVelocity * -1.0f; 
				//		this->spriteEffect = SpriteEffects_FlipHorizontally; 
				//	} 
				//	if(this->position.x <= 1090.0f && xVelocity < 0.0f) 
				//	{ 
				//		this->xVelocity = this->xVelocity * -1.0f; 
				//		this->spriteEffect = SpriteEffects_None; 
				//	} 

				//}

				if(enemyNumber == 1)
					this->jumping = true;
				{  float distanceToMove = tickDelta * this->yVelocity; 
				this->position.y = this->position.y + distanceToMove;
				if (this->position.y >= 992.0f - this->frameScaledHeight && yVelocity > 0.0f) 
				{ 
					this->spriteEffect = SpriteEffects_None;
					this->yVelocity = yVelocity * -1; 	 
				}
				if(this->position.y <= 300.0f && yVelocity < 0.0f)
				{
					this->yVelocity = yVelocity *- 1;
					this->spriteEffect = SpriteEffects_None; 
				}

				}
				if(enemyNumber == 2)
					this->jumping = true;
				{  float distanceToMove = tickDelta * this->yVelocity; 
				this->position.y = this->position.y + distanceToMove;
				if (this->position.y >= 992.0f - this->frameScaledHeight && yVelocity > 0.0f) 
				{ 
					this->spriteEffect = SpriteEffects_None;
					this->yVelocity = yVelocity * -1; 	 
				}
				if(this->position.y <= 300.0f && yVelocity < 0.0f)
				{
					this->yVelocity = yVelocity *- 1;
					this->spriteEffect = SpriteEffects_None;
				}

				}
				if(enemyNumber == 3)
					this->jumping = true;
				{  float distanceToMove = tickDelta * this->yVelocity; 
				this->position.y = this->position.y + distanceToMove;
				if (this->position.y >= 992.0f - this->frameScaledHeight && yVelocity > 0.0f) 
				{ 
					this->spriteEffect = SpriteEffects_None;
					this->yVelocity = yVelocity * -1; 

				}
				if(this->position.y <= 300.0f && yVelocity < 0.0f)
				{
					this->yVelocity = yVelocity *- 1;
					this->spriteEffect = SpriteEffects_None; 

				}

				}



			} 
			else if (this->level == 6) 
			{ 
				if(this->enemyNumber == 1)
				{
					float distanceToMove = tickDelta * this->xVelocity; 
					this->position.x = this->position.x + distanceToMove;
					{
						if(position.x > 795.0f && xVelocity > 0.0f)
						{
							this->xVelocity = this->xVelocity * -1.0f;
							this->spriteEffect = SpriteEffects_FlipHorizontally;
						}
						if(position.x < 0 && xVelocity < 0.0f)
						{
							this->xVelocity = this->xVelocity * -1.0f;
							this->spriteEffect = SpriteEffects_None;
						}


					}

				}
				if(this->enemyNumber == 2)
				{
					float distanceToMove = tickDelta * this->xVelocity; 
					this->position.x = this->position.x + distanceToMove;
					if(this->position.x + frameWidth >= windowBounds.Width && xVelocity > 0.0f)
					{
						this->xVelocity = this->xVelocity * -1.0f;
						this->spriteEffect = SpriteEffects_FlipHorizontally;
					}
					if(this->position.x < 993.0f && xVelocity < 0.0f)
					{
						this->xVelocity = this->xVelocity * -1.0f;
						this->spriteEffect = SpriteEffects_None;
					}


				}

			} 
			else if (this->level == 7) 
			{ 
				if(enemyNumber == 1)
				{
					float distanceToMove = tickDelta * this->xVelocity; 
					this->position.x = this->position.x + distanceToMove;

					if(position.x + frameWidth >= windowBounds.Width && xVelocity > 0.0f)
					{
						this->xVelocity = this->xVelocity * -1.0f;
						this->spriteEffect = SpriteEffects_FlipHorizontally;
					}
					if(position.x < 678.0f && xVelocity < 0.0f)
					{
						this->xVelocity = this->xVelocity * -1.0f;
						this->spriteEffect = SpriteEffects_None;
					}
				}


				if(enemyNumber == 2)
				{
					float distanceToMove = tickDelta * this->xVelocity; 
					this->position.x = this->position.x + distanceToMove;

					if(position.x + frameWidth >= windowBounds.Width && xVelocity > 0.0f)
					{
						this->xVelocity = this->xVelocity * -1.0f;
						this->spriteEffect = SpriteEffects_FlipHorizontally;
					}

					if(position.x < 746.0f && xVelocity < 0.0f)
					{
						this->xVelocity = this->xVelocity * -1.0f;
						this->spriteEffect = SpriteEffects_None;
					}

				}

				if(enemyNumber == 3)
				{
					float distanceToMove = tickDelta * this->xVelocity; 
					this->position.x = this->position.x + distanceToMove;
					if(this->position.x < 0 && xVelocity < 0.0f)
					{
						this->xVelocity = this->xVelocity * -1.0f;
						this->spriteEffect = SpriteEffects_None;
					}

					if(this->position.x > 470.0f && xVelocity > 0.0f)
					{
						this->xVelocity = this->xVelocity * -1.0f;
						this->spriteEffect = SpriteEffects_FlipHorizontally;
					}

				}



			} 
			else if (this->level == 8) 
			{ 
				if(enemyNumber == 1)
				{
					float distanceToMove = tickDelta * this->xVelocity; 
					this->position.x = this->position.x + distanceToMove;

					if(position.x + frameWidth >= windowBounds.Width && xVelocity > 0.0f)
					{
						this->xVelocity = this->xVelocity * -1.0f;
						this->spriteEffect = SpriteEffects_FlipHorizontally;
					}
					if(position.x < 1295.0f && xVelocity < 0.0f)
					{
						this->xVelocity = this->xVelocity * -1.0f;
						this->spriteEffect = SpriteEffects_None;
					}
				}
				if(enemyNumber == 2)
				{
					float distanceToMove = tickDelta * this->xVelocity; 
					this->position.x = this->position.x + distanceToMove;

					if(position.x > 1600.0f && xVelocity > 0.0f)
					{
						this->xVelocity = this->xVelocity * -1.0f;
						this->spriteEffect = SpriteEffects_FlipHorizontally;
					}
					if(position.x < 1185.0f && xVelocity < 0.0f)
					{
						this->xVelocity = this->xVelocity * -1.0f;
						this->spriteEffect = SpriteEffects_None;
					}
				}

			} 
			else if (this->level == 9) 
			{ 
				if(enemyNumber == 1)
				{
					float distanceToMove = tickDelta * this->xVelocity; 
					this->position.x = this->position.x - distanceToMove;

					if(position.x + frameWidth >= windowBounds.Width && xVelocity > 0.0f)
					{
						this->xVelocity = this->xVelocity * -1.0f;
						this->spriteEffect = SpriteEffects_FlipHorizontally;
					}
					if(position.x < 1295.0f && xVelocity < 0.0f)
					{
						this->xVelocity = this->xVelocity * -1.0f;
						this->spriteEffect = SpriteEffects_None;
					}

				}
				if(enemyNumber == 2)
				{
					float distanceToMove = tickDelta * this->xVelocity; 
					this->position.x = this->position.x + distanceToMove;

					if(this->position.x < 0 && xVelocity < 0.0f)
					{
						this->xVelocity = this->xVelocity * -1.0f;
						this->spriteEffect = SpriteEffects_None;
					}
					if(this->position.x > 1400.0f && xVelocity > 0.0f)
					{
						this->xVelocity = this->xVelocity * -1.0f;
						this->spriteEffect = SpriteEffects_FlipHorizontally;
					}

				}
			}
		}
	} 
}



void Enemy::Draw()
{
	XMMATRIX cameraMatrix = Matrix::CreateTranslation(0, 0, 0);
	cameraMatrix *=
		Matrix::CreateTranslation(0,0,0) * 
		Matrix::CreateScale(this->scaleX, this->scaleY, 1.0);
	this->spriteBatch->Begin(SpriteSortMode_Deferred, this->commonStates->NonPremultiplied(), nullptr, nullptr, nullptr, nullptr, cameraMatrix);
	this->spriteBatch->Draw(this->texture.Get(), this->position, this->sourceRect.get(), this->tint, 0.0f, SimpleMath::Vector2(0.0f, 0.0f), 0.7f, this->spriteEffect, 0.0f);
	if (this->highlightEnemy == true)
	{
		this->spriteBatch->Draw(this->arrow.Get(), this->arrowPosition, nullptr, this->tint, 0.0f, SimpleMath::Vector2(0.0f, 0.0f), 1.0f, this->spriteEffect, 0.0f);
	}
	this->spriteBatch->End();
}

void Enemy::Reset(DirectX::SimpleMath::Vector2 startPosition, bool unhighlightEnemy, bool reviveEnemy, int newSheetlength, bool enemySelected, int enemyNumber)
{
	this->position = startPosition;
	this->highlightEnemy = unhighlightEnemy;
	this->enemyDead = reviveEnemy;
	this->sheetLength = newSheetlength;
	this->enemySelected = enemySelected;
	this->enemyNumber = enemyNumber;
	this->detectionBoundingBox.X = this->position.x;
	this->detectionBoundingBox.Y = this->position.y;
	this->boundingBox.X = this->position.x;
	this->boundingBox.Y = this->position.y;
}

void Enemy::HighlightEnemy(bool highlightEnemy)
{
	this->highlightEnemy = highlightEnemy;
}

void Enemy::KillEnemy(int sheetLength, bool enemyDead)
{
	this->sheetLength = 0;
	this->enemyDead = true;
}


Vector2 Enemy::GetPosition()
{
	return this->position;
}

//************************************************************************** Brandon
int Enemy::IntersectsPad1(PressurePlate& plate1)
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

int Enemy::IntersectsPad2(PressurePlate& plate2)
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

int Enemy::IntersectsPad3(PressurePlate& plate3)
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

bool Enemy::IntersectsWith(Windows::Foundation::Rect rectangle)
{
	return this->detectionBoundingBox.IntersectsWith(rectangle);
}

void Enemy::SetLevel(int setLevel)
{
	this->level = setLevel;
}

void Enemy::ClearUnusedEnemies()
{
	this->position = Vector2(0.0f, 1080.0f);
	this->detectionBoundingBox.X = this->position.x;
	this->detectionBoundingBox.Y = this->position.y;
}
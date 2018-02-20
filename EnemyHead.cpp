#include "pch.h"
#include "EnemyHead.h"
#include "Utilities.h"
#include <WICTextureLoader.h>

using namespace Microsoft::WRL;
using namespace DirectX;
using namespace Windows::Foundation;
using namespace DirectX::SimpleMath;
using namespace std;


EnemyHead::EnemyHead(void) : 
	texture ( nullptr )
	, position ( 0.0f, 0.0f )
	, tint ( 1.0f, 1.0f, 1.0f, 1.0f )
	, boundingBox (0.0f, 0.0f, 56.0f, 50.0f )
	, visible ( false )
	, sourceRect ( nullptr )
	, spriteEffect ( SpriteEffects_None )

	, accelerationDueToGravity ( 0.0f, 0.6f )
	, jumping ( false )
	, jumpDistance ( 30 )
	, acceleration ( 0.0f, 0.0f )
	, velocity ( 0.0f, 0.0f )
	, justSpawned ( false )
	, scaleX ( 1.0 )
	, scaleY ( 1.0 )
{
}


EnemyHead::~EnemyHead(void)
{
}

void EnemyHead::Initialize(wstring filePathName 
						   , ComPtr<ID3D11Device1> d3dDevice
						   , ComPtr<ID3D11DeviceContext1> d3dDeviceContext, float scaleX, float scaleY)
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
	this->sourceRect->left = 107.0f;
	this->sourceRect->top = 65.0f;
	this->sourceRect->right = 163.0f;
	this->sourceRect->bottom = 173.0f;

	/*this->velocity.y = -5.0f;
	this->jumping = true;	
	this->acceleration = this->accelerationDueToGravity;
	Vector2 distanceTravelled = tickDelta * this->velocity;
	this->position = this->position + distanceTravelled;*/
}

void EnemyHead::Draw()
{
	XMMATRIX cameraMatrix = Matrix::CreateTranslation(0, 0, 0);
	cameraMatrix *=
		Matrix::CreateTranslation(0,0,0) * 
		Matrix::CreateScale(this->scaleX, this->scaleY, 1.0);
  this->spriteBatch->Begin(SpriteSortMode_Deferred, this->commonStates->NonPremultiplied(), nullptr, nullptr, nullptr, nullptr, cameraMatrix);
	this->spriteBatch->Draw(this->texture.Get(), this->position, this->sourceRect.get(), this->tint, 0.0f, SimpleMath::Vector2(0.0f, 0.0f), 0.7f, this->spriteEffect, 0.0f);
	this->spriteBatch->End();


}

void EnemyHead::Reset(DirectX::SimpleMath::Vector2 startPosition)
{
	this->position = startPosition;
	this->justSpawned = true;
}

bool EnemyHead::IntersectsWith(Windows::Foundation::Rect rectangle)
{
	return this->boundingBox.IntersectsWith(rectangle);
}

void EnemyHead::SetBoundingBox(Windows::Foundation::Rect rectangle)
{
	this->boundingBox = rectangle;
}


void EnemyHead::Update(int tickTotal, int tickDelta, float timeDelta, Rect windowBounds, PlatformLoader& platform1, PlatformLoader& platform2, PlatformLoader& platform3, PlatformLoader& platform4, PlatformLoader& platform5, PlatformLoader& platform6, PlatformLoader& ladder, PressurePlate& plate1, PressurePlate& plate2, PressurePlate& plate3)
{
	//Calculate the current position
	//this->position = this->position  + (timeDelta * this->velocity);

	if( this->justSpawned == true )
	{
		//
		//this->jumping = true;	
		this->velocity.y = -10.0f;
		this->velocity.x = -10.0f;
		this->justSpawned = false;
	}
		this->acceleration = this->accelerationDueToGravity;
		Vector2 distanceTravelled = tickDelta * this->velocity;
		this->position = this->position + distanceTravelled;
		
	//}
	//Code for making the hero jump
			//if (this->jumping)
			//{
			//	//First calculate the current velocity of the sprite
			//	Vector2 initialVelocity = this->velocity;
			//	this->velocity = initialVelocity + (tickDelta * this->acceleration);

			//	//Second calculate the distance moved
			//	Vector2 distanceTravelled = 0.5f * (initialVelocity + this->velocity) * tickDelta;

			//	//Third, calculate the new position of the sprite
			//	this->position = this->position + distanceTravelled;
			//	
			//	if( this->justSpawned == true && jumping == true )
			//	{
			//		//Jump
			//		this->velocity.y = -10.0f;
			//		this->jumping = true;	
			//		this->acceleration = this->accelerationDueToGravity;
			//		Vector2 distanceTravelled = tickDelta * this->velocity;
			//		this->position = this->position + distanceTravelled;
			//	}
			//}

			//else if( this->justSpawned == true )
			//{
			//	//Jump
			//	this->velocity.y = -10.0f;
			//	this->jumping = true;	
			//	this->acceleration = this->accelerationDueToGravity;
			//}
	//Level Platform Collision

	if ( platform1.IntersectsWith(this->boundingBox) && this->velocity.y > 0.0f)
	{
		this->velocity.y = 0.0f;
		this->velocity.x = 0.0f;
		this->jumping = false;
	}
	if ( platform2.IntersectsWith(this->boundingBox) && this->velocity.y > 0.0f)
	{
		this->velocity.y = 0.0f;
		this->velocity.x = 0.0f;
		this->jumping = false;
	}

	if ( platform3.IntersectsWith(this->boundingBox) && this->velocity.y > 0.0f)
	{
		this->velocity.y = 0.0f;
		this->velocity.x = 0.0f;
		this->jumping = false;
	}

	if ( platform4.IntersectsWith(this->boundingBox) && this->velocity.y > 0.0f)
	{
		this->velocity.y = 0.0f;
		this->velocity.x = 0.0f;
		this->jumping = false;
	}

	if ( platform5.IntersectsWith(this->boundingBox) && this->velocity.y > 0.0f)
	{
		this->velocity.y = 0.0f;
		this->velocity.x = 0.0f;
		this->jumping = false;
	}
	if ( platform6.IntersectsWith(this->boundingBox) && this->velocity.y > 0.0f)
	{
		this->velocity.y = 0.0f;
		this->velocity.x = 0.0f;
		this->jumping = false;
	}

	//************************************************************************* Brandon
	if(plate1.IntersectsWith(this->boundingBox))
	{
		this->velocity.y = 0.0f;
		this->velocity.x = 0.0f;
		this->jumping = false;


	}
	if(plate2.IntersectsWith(this->boundingBox))
	{
		this->velocity.y = 0.0f;
		this->velocity.x = 0.0f;
		this->jumping = false;
	}
	if(plate3.IntersectsWith(this->boundingBox))
	{
		this->velocity.y = 0.0f;
		this->velocity.x = 0.0f;
		this->jumping = false;
	}
	//************************************************************************


	//Vector2 translation = tickDelta * velocity;
	//this->position = this->position + translation;

}


void EnemyHead::SetJustSpawned(bool justSpawned)
{
	this->justSpawned = justSpawned;
	//this->jumping = true;
}
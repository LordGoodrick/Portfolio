#include "pch.h"
#include "Bullet.h"
#include "Utilities.h"
#include <WICTextureLoader.h>

using namespace Microsoft::WRL;
using namespace DirectX;
using namespace Windows::Foundation;
using namespace DirectX::SimpleMath;
using namespace std;


Bullet::Bullet(void) : 
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
{
}


Bullet::~Bullet(void)
{
}

void Bullet::Initialize(wstring filePathName 
						   , ComPtr<ID3D11Device1> d3dDevice
						   , ComPtr<ID3D11DeviceContext1> d3dDeviceContext)
{
	ThrowIfFailed(
		CreateWICTextureFromFile(d3dDevice.Get(), d3dDeviceContext.Get(),
		filePathName.c_str(), 0, &this->texture)
		);

	this->spriteBatch = std::unique_ptr<SpriteBatch>(new SpriteBatch(d3dDeviceContext.Get()));
	this->commonStates = std::unique_ptr<CommonStates>(new CommonStates(d3dDevice.Get()));

	this->sourceRect = std::unique_ptr<RECT>(new RECT);
	this->sourceRect->left = 107.0f;
	this->sourceRect->top = 65.0f;
	this->sourceRect->right = 163.0f;
	this->sourceRect->bottom = 173.0f;
}

void Bullet::Draw()
{
	XMMATRIX cameraMatrix = Matrix::CreateTranslation(0, 0, 0);
	this->spriteBatch->Draw(this->texture.Get(), this->position, this->sourceRect.get(), this->tint, 0.0f, SimpleMath::Vector2(0.0f, 0.0f), 0.7f, this->spriteEffect, 0.0f);
	this->spriteBatch->End();


}

void Bullet::Reset(DirectX::SimpleMath::Vector2 startPosition)
{
	this->position = startPosition;
	this->justSpawned = true;
}

void Bullet::Update(int tickTotal, int tickDelta, float timeDelta, Rect windowBounds, PlatformLoader& platform1, PlatformLoader& platform2, PlatformLoader& platform3, PlatformLoader& platform4, PlatformLoader& platform5, PlatformLoader& platform6, PlatformLoader& ladder, PressurePlate& plate1, PressurePlate& plate2, PressurePlate& plate3)
{
	

}


void Bullet::SetJustSpawned(bool justSpawned)
{
	this->justSpawned = justSpawned;
	//this->jumping = true;
}
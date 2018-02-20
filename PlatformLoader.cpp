#include "pch.h"
#include "platformLoader.h"
#include "Utilities.h"
#include <WICTextureLoader.h>

using namespace Microsoft::WRL;
using namespace DirectX;
using namespace Windows::Foundation;
using namespace DirectX::SimpleMath;
using namespace std;


PlatformLoader::PlatformLoader(void) : 
    texture ( nullptr )
  , position ( 0.0f, 0.0f )
  , tint ( 1.0f, 1.0f, 1.0f, 1.0f )
  , boundingBox (0.0f, 0.0f, 128.0f, 32.0f )
  , visible ( false )
  , scaleX ( 1.0 )
  , scaleY ( 1.0 )
  , velocity ( 0.0f, 0.0f)
  , liftPosition (900.0f, 460.0f)
{
}


PlatformLoader::~PlatformLoader(void)
{
}

void PlatformLoader::Initialize(wstring filePathName 
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

}

void PlatformLoader::Draw()
{
	XMMATRIX cameraMatrix = Matrix::CreateTranslation(0, 0, 0);
	cameraMatrix *=
		Matrix::CreateTranslation(0,0,0) * 
		Matrix::CreateScale(this->scaleX, this->scaleY, 1.0);
  this->spriteBatch->Begin(SpriteSortMode_Deferred, this->commonStates->NonPremultiplied(), nullptr, nullptr, nullptr, nullptr, cameraMatrix);
	this->spriteBatch->Draw(this->texture.Get(), this->position, this->tint);
	this->spriteBatch->End();

}

void PlatformLoader::Reset(DirectX::SimpleMath::Vector2 startPosition)
{
  this->position = startPosition;
}

bool PlatformLoader::IntersectsWith(Windows::Foundation::Rect rectangle)
{
	return this->boundingBox.IntersectsWith(rectangle);
}

void PlatformLoader::ResetLift()
{
  //this->liftPosition = this->position;
  this->liftPosition.y = 460.0f;
}

void PlatformLoader::SetBoundingBox(Windows::Foundation::Rect rectangle)
{
  this->boundingBox = rectangle;
}

Rect PlatformLoader::GetBoundingBox()
{
	return this->boundingBox;
}
//****************************************************** Jack
void PlatformLoader::LowerLift(int tickTotal, int tickDelta, float timeDelta)
{
	Vector2 translation = tickDelta * velocity;
	this->position = this->position + translation;
	this->liftPosition = this->liftPosition + translation;

	this->velocity.y = 2.0f;

	this->boundingBox.Y = liftPosition.y;

	if(this->boundingBox.Y >= 835.0f)// && this->position.y >= -45.0f)
	{
		this->velocity.y = 0;
		boundingBox.Y = 835.0f;
	}
}

void PlatformLoader::RaiseLift(int tickTotal, int tickDelta, float timeDelta)
{
	Vector2 translation = tickDelta * velocity;
	this->position = this->position + translation;
	this->liftPosition = this->liftPosition + translation;

	this->velocity.y = -2.0f;

	boundingBox.Y = liftPosition.y;

	if(this->boundingBox.Y <= 460.0f)// && this->position.y <= -415.0f)
	{
		this->velocity.y = 0;
		this->boundingBox.Y = 460.0f;
		this->position.y = -415.0f;
	}
}
//************************************************************
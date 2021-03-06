#include "pch.h"
#include "PressurePlate.h"
#include "Utilities.h"
#include <WICTextureLoader.h>

using namespace Microsoft::WRL;
using namespace DirectX;
using namespace Windows::Foundation;
using namespace DirectX::SimpleMath;
using namespace std;


PressurePlate::PressurePlate(void) : 
    texture ( nullptr )
  , position ( 0.0f, 0.0f )
  , tint ( 1.0f, 1.0f, 1.0f, 1.0f )
  , boundingBox (0.0f, 0.0f, 128.0f, 32.0f )
  , visible ( false )
  , scaleX ( 1.0 )
  , scaleY ( 1.0 )
{
}


PressurePlate::~PressurePlate(void)
{
}

void PressurePlate::Initialize(wstring filePathName 
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

void PressurePlate::Draw()
{
	XMMATRIX cameraMatrix = Matrix::CreateTranslation(0, 0, 0);
	cameraMatrix *=
		Matrix::CreateTranslation(0,0,0) * 
		Matrix::CreateScale(this->scaleX, this->scaleY, 1.0);
  this->spriteBatch->Begin(SpriteSortMode_Deferred, this->commonStates->NonPremultiplied(), nullptr, nullptr, nullptr, nullptr, cameraMatrix);
	this->spriteBatch->Draw(this->texture.Get(), this->position, this->tint);
	this->spriteBatch->End();

}

void PressurePlate::Reset(DirectX::SimpleMath::Vector2 startPosition)
{
  this->position = startPosition;
}

bool PressurePlate::IntersectsWith(Windows::Foundation::Rect rectangle)
{
	return this->boundingBox.IntersectsWith(rectangle);
}

void PressurePlate::SetBoundingBox(Windows::Foundation::Rect rectangle)
{
  this->boundingBox = rectangle;
}

Rect PressurePlate::GetBoundingBox()
{
	return this->boundingBox;
}
//******************************************************************************************************************
#include "pch.h"
#include "PainKiller1.h"
#include "Utilities.h"
#include <WICTextureLoader.h>

using namespace Microsoft::WRL;
using namespace DirectX;
using namespace Windows::Foundation;
using namespace DirectX::SimpleMath;
using namespace std;


PainKiller1::PainKiller1(void) : 
    texture ( nullptr )
  , position ( 0.0f, 0.0f )
  , tint ( 1.0f, 1.0f, 1.0f, 1.0f )
  , painKillers ( 3 )
  , scaleX ( 1.0 )
  , scaleY ( 1.0 )
  , visible ( true )
{
}


PainKiller1::~PainKiller1(void)
{
}

void PainKiller1::Initialize(wstring filePathName 
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

void PainKiller1::Draw()
{
	XMMATRIX cameraMatrix = Matrix::CreateTranslation(0, 0, 0);
	cameraMatrix *=
		Matrix::CreateTranslation(0,0,0) * 
		Matrix::CreateScale(this->scaleX, this->scaleY, 1.0);
  this->spriteBatch->Begin(SpriteSortMode_Deferred, this->commonStates->NonPremultiplied(), nullptr, nullptr, nullptr, nullptr, cameraMatrix);
  if(this->visible == true)
	{
	this->spriteBatch->Draw(this->texture.Get(), this->position, this->tint);
	}
	else
	{
	}
	this->spriteBatch->End();

}

void PainKiller1::Reset(DirectX::SimpleMath::Vector2 startPosition)
{
  this->position = startPosition;
}

bool PainKiller1::IntersectsWith(Windows::Foundation::Rect rectangle)
{
	return this->boundingBox.IntersectsWith(rectangle);
}

bool PainKiller1::SetVisiblity(bool visibility)
{
	this->visible = visibility;
	return this->visible;
}

void PainKiller1::SetBoundingBox(Windows::Foundation::Rect rectangle)
{
	this->boundingBox = rectangle;
	this->boundingBox.X = position.x;
	this->boundingBox.Y = position.y;
}

//*********************************************************************************************************************** Jake Evans
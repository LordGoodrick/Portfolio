//**********************************************************************************************
#include "pch.h"
#include "Hud.h"
#include "Utilities.h"
#include <WICTextureLoader.h>

using namespace Microsoft::WRL;
using namespace DirectX;
using namespace Windows::Foundation;
using namespace DirectX::SimpleMath;
using namespace std;


Hud::Hud(void) : 
    texture ( nullptr )
  , position ( 0.0f, 0.0f )
  , tint ( 1.0f, 1.0f, 1.0f, 1.0f )
  , scaleX ( 1.0 )
  , scaleY ( 1.0 )
  
{
}


Hud::~Hud(void)
{
}

void Hud::Initialize(wstring filePathName 
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

void Hud::Draw()
{
	XMMATRIX cameraMatrix = Matrix::CreateTranslation(0, 0, 0);
	cameraMatrix *=
		Matrix::CreateTranslation(0,0,0) * 
		Matrix::CreateScale(this->scaleX, this->scaleY, 1.0);
  this->spriteBatch->Begin(SpriteSortMode_Deferred, this->commonStates->NonPremultiplied(), nullptr, nullptr, nullptr, nullptr, cameraMatrix);
	this->spriteBatch->Draw(this->texture.Get(), this->position, this->tint);
	this->spriteBatch->End();

}

void Hud::Reset(DirectX::SimpleMath::Vector2 startPosition)
{
  this->position = startPosition;
}

//void Hud::SetSourceRect(Windows::Foundation::Rect rectangle)
//{
//  this->SetSourceRect = rectangle;
//}

//***************************************************************************************Jake Evans
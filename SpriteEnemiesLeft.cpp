//************************************************************
#include "pch.h"
#include "SpriteEnemiesLeft.h"
#include "Utilities.h"
#include <WICTextureLoader.h>

using namespace Microsoft::WRL;
using namespace DirectX;
using namespace Windows::Foundation;
using namespace DirectX::SimpleMath;
using namespace std;


SpriteEnemiesLeft::SpriteEnemiesLeft(void) : 
    texture ( nullptr )
  , position ( 0.0f, 0.0f )
  , tint ( 1.0f, 1.0f, 1.0f, 1.0f )
  
  
{
}


SpriteEnemiesLeft::~SpriteEnemiesLeft(void)
{
}

void SpriteEnemiesLeft::Initialize(wstring filePathName 
            , ComPtr<ID3D11Device1> d3dDevice
            , ComPtr<ID3D11DeviceContext1> d3dDeviceContext)
{
  ThrowIfFailed(
		CreateWICTextureFromFile(d3dDevice.Get(), d3dDeviceContext.Get(),
		filePathName.c_str(), 0, &this->texture)
		);

	this->spriteBatch = std::unique_ptr<SpriteBatch>(new SpriteBatch(d3dDeviceContext.Get()));
	this->commonStates = std::unique_ptr<CommonStates>(new CommonStates(d3dDevice.Get()));

}

void SpriteEnemiesLeft::Draw()
{
	XMMATRIX cameraMatrix = Matrix::CreateTranslation(0, 0, 0);
	this->spriteBatch->Draw(this->texture.Get(), this->position, this->tint);
	this->spriteBatch->End();

}

void SpriteEnemiesLeft::Reset(DirectX::SimpleMath::Vector2 startPosition)
{
  this->position = startPosition;
}

//************************************************************************************************************* Jake Evans
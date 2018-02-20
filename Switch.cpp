#include "pch.h"
#include "Switch.h"
#include "Utilities.h"
#include <WICTextureLoader.h>

using namespace Microsoft::WRL;
using namespace DirectX;
using namespace Windows::Foundation;
using namespace DirectX::SimpleMath;
using namespace std;


Switch::Switch(void) : 
    texture ( nullptr )
  , position ( 0.0f, 0.0f )
  , tint ( 1.0f, 1.0f, 1.0f, 1.0f )
  , boundingBox (0.0f, 0.0f, 128.0f, 32.0f )
  , visible ( false )
{
}


Switch::~Switch(void)
{
}

void Switch::Initialize(wstring filePathName 
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

void Switch::Draw()
{
	XMMATRIX cameraMatrix = Matrix::CreateTranslation(0, 0, 0);
	this->spriteBatch->Draw(this->texture.Get(), this->position, this->tint);
	this->spriteBatch->End();

}

void Switch::Reset(DirectX::SimpleMath::Vector2 startPosition)
{
  this->position = startPosition;
}

bool Switch::IntersectsWith(Windows::Foundation::Rect rectangle)
{
	return this->boundingBox.IntersectsWith(rectangle);
}

void Switch::SetBoundingBox(Windows::Foundation::Rect rectangle)
{
  this->boundingBox = rectangle;
}
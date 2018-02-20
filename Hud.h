#pragma once
//**************************************************************************************************************

//DirectX Tool Kit
#include <SimpleMath.h>
#include <SpriteBatch.h>
#include <CommonStates.h>


class Hud
{
public:
   Hud(void);
  ~Hud(void);

  void Initialize(std::wstring filePathName 
    , Microsoft::WRL::ComPtr<ID3D11Device1> d3dDevice
    , Microsoft::WRL::ComPtr<ID3D11DeviceContext1> d3dDeviceContext, float scaleX, float scaleY);
  void Draw();

  void Reset(DirectX::SimpleMath::Vector2 startPosition);
  /*void SetSourceRect(Windows::Foundation::Rect rectangle);*/
  

private:
  Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture;
  DirectX::SimpleMath::Vector2 position;  //Pixels
  DirectX::SimpleMath::Color tint;
  float scaleX;
  float scaleY;

  std::unique_ptr<DirectX::SpriteBatch> spriteBatch;   
  std::unique_ptr<DirectX::CommonStates> commonStates;

};
//************************************************************************************************************ Jake Evans
#pragma once
//************************************************************************

//DirectX Tool Kit
#include <SimpleMath.h>
#include <SpriteBatch.h>
#include <CommonStates.h>


class PainKiller1
{
public:
   PainKiller1(void);
  ~PainKiller1(void);

  void Initialize(std::wstring filePathName 
    , Microsoft::WRL::ComPtr<ID3D11Device1> d3dDevice
    , Microsoft::WRL::ComPtr<ID3D11DeviceContext1> d3dDeviceContext, float scaleX, float scaleY);
  void Draw();

  void Reset(DirectX::SimpleMath::Vector2 startPosition);
  
  bool IntersectsWith(Windows::Foundation::Rect rectangle);
  void SetBoundingBox(Windows::Foundation::Rect rectangle);
  bool SetVisiblity(bool visibility);

private:
  Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture;
  DirectX::SimpleMath::Vector2 position;  //Pixels
  DirectX::SimpleMath::Color tint;
  Windows::Foundation::Rect boundingBox;
  float scaleX;
  float scaleY;

  std::unique_ptr<DirectX::SpriteBatch> spriteBatch;   
  std::unique_ptr<DirectX::CommonStates> commonStates;

  int painKillers;
  bool visible;
};

//***************************************************************************************************** Jake Evans
#pragma once

//Standard Library
#include <string>
//DirectX Tool Kit
#include <SimpleMath.h>
#include <SpriteBatch.h>
#include <CommonStates.h>

class PressurePlate
{
public:
  PressurePlate(void);
  ~PressurePlate(void);

  void Initialize(std::wstring filePathName 
    , Microsoft::WRL::ComPtr<ID3D11Device1> d3dDevice
    , Microsoft::WRL::ComPtr<ID3D11DeviceContext1> d3dDeviceContext, float scaleX, float scaleY);
  void Draw();
  void Update();
  void Reset(DirectX::SimpleMath::Vector2 startPosition);
  bool IntersectsWith(Windows::Foundation::Rect rectangle);
  bool ColideTop(Windows::Foundation::Rect platformTop);
  void SetBoundingBox(Windows::Foundation::Rect rectangle);
  Windows::Foundation::Rect GetBoundingBox();

private:
  Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture;
  DirectX::SimpleMath::Vector2 position;  //Pixels
  DirectX::SimpleMath::Color tint;
  Windows::Foundation::Rect boundingBox;
  bool visible;
  float scaleX;
  float scaleY;
  std::unique_ptr<DirectX::SpriteBatch> spriteBatch;   
  std::unique_ptr<DirectX::CommonStates> commonStates;

};
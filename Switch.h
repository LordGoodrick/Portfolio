#pragma once

//Standard Library
#include <string>
//DirectX Tool Kit
#include <SimpleMath.h>
#include <SpriteBatch.h>
#include <CommonStates.h>

class Switch
{
public:
  Switch(void);
  ~Switch(void);

  void Initialize(std::wstring filePathName 
    , Microsoft::WRL::ComPtr<ID3D11Device1> d3dDevice
    , Microsoft::WRL::ComPtr<ID3D11DeviceContext1> d3dDeviceContext);
  void Draw();
  void Update();
  void Reset(DirectX::SimpleMath::Vector2 startPosition);
  bool IntersectsWith(Windows::Foundation::Rect rectangle);
  bool ColideTop(Windows::Foundation::Rect platformTop);
  void SetBoundingBox(Windows::Foundation::Rect rectangle);

private:
  Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture;
  DirectX::SimpleMath::Vector2 position;  //Pixels
  DirectX::SimpleMath::Color tint;
  Windows::Foundation::Rect boundingBox;
  bool visible;
  std::unique_ptr<DirectX::SpriteBatch> spriteBatch;   
  std::unique_ptr<DirectX::CommonStates> commonStates;

};
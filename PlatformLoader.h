#pragma once

//Standard Library
#include <string>
//DirectX Tool Kit
#include <SimpleMath.h>
#include <SpriteBatch.h>
#include <CommonStates.h>

class PlatformLoader
{
public:
  PlatformLoader(void);
  ~PlatformLoader(void);

  void Initialize(std::wstring filePathName 
    , Microsoft::WRL::ComPtr<ID3D11Device1> d3dDevice
    , Microsoft::WRL::ComPtr<ID3D11DeviceContext1> d3dDeviceContext, float scaleX, float scaleY);
  void Draw();
  void Update();
  void LowerLift(int tickTotal, int tickDelta, float timeDelta);
  void RaiseLift(int tickTotal, int tickDelta, float timeDelta);
  void Reset(DirectX::SimpleMath::Vector2 startPosition);
  void ResetLift();
  bool IntersectsWith(Windows::Foundation::Rect rectangle);
  void SetBoundingBox(Windows::Foundation::Rect rectangle);
  Windows::Foundation::Rect GetBoundingBox();
  

private:
  Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture;
  DirectX::SimpleMath::Vector2 position;  //Pixels
  DirectX::SimpleMath::Color tint;
  Windows::Foundation::Rect boundingBox;
  bool visible;
  std::unique_ptr<DirectX::SpriteBatch> spriteBatch;   
  std::unique_ptr<DirectX::CommonStates> commonStates;
  float scaleX;
  float scaleY;
  DirectX::SimpleMath::Vector2 velocity;
  DirectX::SimpleMath::Vector2 liftPosition;

};
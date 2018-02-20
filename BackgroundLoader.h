#pragma once

#include <SimpleMath.h>
#include <string.h>
#include <SpriteBatch.h>
#include <CommonStates.h>
#include "HeroSprite.h"

class BackgroundLoader
{
public:
  BackgroundLoader(void);
  ~BackgroundLoader(void);

  void Initialize(std::wstring filePathName, 
            Microsoft::WRL::ComPtr<ID3D11Device1> d3dDevice,
            Microsoft::WRL::ComPtr<ID3D11DeviceContext1> d3dDeviceContext, float scaleX, float scaleY);
  void Draw();
  void DrawPlatform1();
  void Update(float timeDelta, Windows::Foundation::Rect windowBounds);
  void Reset(DirectX::SimpleMath::Vector2 startPosition);

private:
  Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture;
  DirectX::SimpleMath::Vector2 position;  //Pixels
  DirectX::SimpleMath::Color tint;
  Windows::Foundation::Rect boundingBox;
  int lives;
  DirectX::SimpleMath::Vector2 heroPosition;
  float scaleX;
  float scaleY;

  // DirectX Tool Kit objects
  std::unique_ptr<DirectX::SpriteBatch> spriteBatch;   
  std::unique_ptr<DirectX::CommonStates> commonStates; 

};
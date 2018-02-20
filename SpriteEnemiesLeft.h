#pragma once
//********************************************************************************

//DirectX Tool Kit
#include <SimpleMath.h>
#include <SpriteBatch.h>
#include <CommonStates.h>


class SpriteEnemiesLeft
{
public:
   SpriteEnemiesLeft(void);
  ~SpriteEnemiesLeft(void);

  void Initialize(std::wstring filePathName 
    , Microsoft::WRL::ComPtr<ID3D11Device1> d3dDevice
    , Microsoft::WRL::ComPtr<ID3D11DeviceContext1> d3dDeviceContext);
  void Draw();

  void Reset(DirectX::SimpleMath::Vector2 startPosition);
  

private:
  Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture;
  DirectX::SimpleMath::Vector2 position;  //Pixels
  DirectX::SimpleMath::Color tint;
  

  std::unique_ptr<DirectX::SpriteBatch> spriteBatch;   
  std::unique_ptr<DirectX::CommonStates> commonStates;
  //****************************************************************************** Jake Evans
};
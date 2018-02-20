#pragma once

//Standard Library
#include <string>
//DirectX Tool Kit
#include <SimpleMath.h>
#include <SpriteBatch.h>
#include <CommonStates.h>
#include "PlatformLoader.h"
#include "PressurePlate.h"

using namespace std;
using namespace Microsoft::WRL;
using namespace DirectX;
using namespace Windows::Foundation;
using namespace DirectX::SimpleMath;

class Bullet
{
public:
  Bullet(void);
  ~Bullet(void);

  void Initialize(std::wstring filePathName 
    , Microsoft::WRL::ComPtr<ID3D11Device1> d3dDevice
    , Microsoft::WRL::ComPtr<ID3D11DeviceContext1> d3dDeviceContext);
  void Draw();
  void Update(int tickTotal, int tickDelta, float timeDelta, Rect windowBounds, PlatformLoader& platform1, PlatformLoader& platform2, PlatformLoader& platform3, PlatformLoader& platform4, PlatformLoader& platform5, PlatformLoader& platform6, PlatformLoader& ladder, PressurePlate& plate1, PressurePlate& plate2, PressurePlate& plate3);
  void Reset(DirectX::SimpleMath::Vector2 startPosition);
  void SetJustSpawned(bool justSpawned);


  bool IntersectsWith(Windows::Foundation::Rect rectangle);
  void SetBoundingBox(Windows::Foundation::Rect rectangle);

private:
  Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture;
  DirectX::SimpleMath::Vector2 position;  //Pixels
  DirectX::SimpleMath::Color tint;
  Windows::Foundation::Rect boundingBox;
  std::unique_ptr<RECT> sourceRect;
  DirectX::SpriteEffects spriteEffect;

  DirectX::SimpleMath::Vector2 acceleration; //In pixels per tick per tick
  bool jumping;
  int jumpDistance;
  bool justSpawned;

  //Physical Constants
    DirectX::SimpleMath::Vector2 accelerationDueToGravity;  //In pixels per tick per tick
	DirectX::SimpleMath::Vector2 velocity;  //Pixels per second

  bool visible;
  std::unique_ptr<DirectX::SpriteBatch> spriteBatch;   
  std::unique_ptr<DirectX::CommonStates> commonStates;

};
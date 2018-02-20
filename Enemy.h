#pragma once

#include <SimpleMath.h>
#include <string.h>
#include <SpriteBatch.h>
#include <CommonStates.h>
#include "GameController.h"
#include "PlatformLoader.h"
#include "PressurePlate.h"

class Enemy
{
public:
  Enemy(void);
  ~Enemy(void);

  void Initialize(std::wstring filePathName, Microsoft::WRL::ComPtr<ID3D11Device1> d3dDevice, Microsoft::WRL::ComPtr<ID3D11DeviceContext1> d3dDeviceContext, float scaleX, float scaleY);
  void Draw();
  void Update(int tickTotal, int tickDelta, float timeDelta, Windows::Foundation::Rect windowBounds, PlatformLoader& platform1, PlatformLoader& platform2, PlatformLoader& platform3, PlatformLoader& platform4, PlatformLoader& platform5, PlatformLoader& platform6, PlatformLoader& ladder, PressurePlate& plate1, PressurePlate& plate2, PressurePlate& plate3);
  void Reset(DirectX::SimpleMath::Vector2 startPosition, bool unhighlightEnemy, bool reviveEnemy, int newSheetLength, bool enemySelected, int enemyNumber);
  void HighlightEnemy(bool highlightEnemy);
  void KillEnemy(int sheetLength, bool enemyDead);
  DirectX::SimpleMath::Vector2 GetPosition();
  int IntersectsPad1(PressurePlate& plate1);
  int IntersectsPad2(PressurePlate& plate2);
  int IntersectsPad3(PressurePlate& plate3);
  bool IntersectsWith(Windows::Foundation::Rect rectangle);
  void SetLevel(int setLevel);
  void ClearUnusedEnemies();

private:
  Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture;
  Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> headTexture;
  DirectX::SimpleMath::Vector2 position;  //Pixels
  DirectX::SimpleMath::Vector2 velocity;  //Pixels per second
  DirectX::SimpleMath::Color tint;
  Windows::Foundation::Rect boundingBox;
  int level;
  std::unique_ptr<RECT> sourceRect;
  const int frameHeight; 
  const int frameScaledHeight;
  const int frameWidth;
  const int frameScaledWidth;
  int sheetLength;
  int frame;
  int lastFrameChange;    
  int frameRate; 
  DirectX::SpriteEffects spriteEffect;
  DirectX::SimpleMath::Vector2 acceleration; //In pixels per tick per tick
  bool jumping;
  int jumpDistance;
  bool indicator;
  Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> arrow;
    Microsoft::WRL::ComPtr<ID3D11Device1> d3dDevice;
  Microsoft::WRL::ComPtr<ID3D11DeviceContext1> d3dDeviceContext;
  DirectX::SimpleMath::Vector2 arrowPosition;
  const int halfArrow;
  const int arrowHeight;
  DirectX::SimpleMath::Color arrowColor;
  bool highlightEnemy;
  bool enemyDead;
  bool enemySelected;
  Windows::Foundation::Rect detectionBoundingBox;
  const float speed;
  float scaleX;
  float scaleY;
  int enemyNumber;
  float xVelocity; 
  float yVelocity; 
  float jumpStrength;

  //********************************
  int activatedPad1;
  int activatedPad2;
  int activatedPad3;
  //*********************************


  //Gamepad variables
  //GamepadState state;
  GameController gamepad;


  //Physical Constants
    DirectX::SimpleMath::Vector2 accelerationDueToGravity;  //In pixels per tick per tick

  // DirectX Tool Kit objects
  std::unique_ptr<DirectX::SpriteBatch> spriteBatch;   
  std::unique_ptr<DirectX::CommonStates> commonStates; 

};


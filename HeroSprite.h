#pragma once

#include <SimpleMath.h>
#include <string.h>
#include <SpriteBatch.h>
#include <CommonStates.h>
#include "GameController.h"
#include "PlatformLoader.h"
#include "PressurePlate.h"
#include "Enemy.h"
#include "Hearts.h"
#include "PainKiller1.h"

class HeroSprite
{
public:
  HeroSprite(void);
  ~HeroSprite(void);

  void Initialize(std::wstring filePathName, 
            Microsoft::WRL::ComPtr<ID3D11Device1> d3dDevice,
            Microsoft::WRL::ComPtr<ID3D11DeviceContext1> d3dDeviceContext, float scaleX, float scaleY);
  void Draw();
  void Update(int tickTotal, int tickDelta, float timeDelta, Windows::Foundation::Rect windowBounds, PlatformLoader& platform1, PlatformLoader& platform2, PlatformLoader& platform3, PlatformLoader& platform4, PlatformLoader& platform5, PlatformLoader& platform6, PlatformLoader& door, PlatformLoader& ladder, PressurePlate& plate1, PressurePlate& plate2, PressurePlate& plate3, Hearts& heart_Asset, PainKiller1& painKiller_Asset, PlatformLoader& chainLift);
  void Reset(DirectX::SimpleMath::Vector2 startPosition, bool unhighlightEnemy, bool reviveEnemy);
  bool IntersectsWith(Windows::Foundation::Rect rectangle);
  bool NextLevel();
  bool HeroDead();
  void ResetLives();
  void ResetPainkillers();
  void LockDoor(bool lockDoor);
    //*****************************************************
  int ReturnLives();
  int ReturnPainKillers();
  //**************************************************** Jake Evans
  void HighlightEnemy(bool highlightEnemy);
  void NewSheetLength(int sheetLength);
  int IntersectsPad1(PressurePlate& plate1);
  int IntersectsPad2(PressurePlate& plate2);
  int IntersectsPad3(PressurePlate& plate3);
  int DeathByEnemy1(Enemy& enemy1);
  int DeathByEnemy2(Enemy& enemy2);
  int DeathByEnemy3(Enemy& enemy3);
  void PainKillers();
  const float speed;

private:
  Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture;
  DirectX::SimpleMath::Vector2 position;  //Pixels
  DirectX::SimpleMath::Vector2 velocity;  //Pixels per second
  DirectX::SimpleMath::Color tint;
  Windows::Foundation::Rect boundingBox;
  
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
  bool highlightEnemy;
  bool enemyDead;
  bool nextLevel;
  bool heroDead;
  int enemyNumber;
  bool mindControlling;
  bool doorLocked;
  float scaleX;
  float scaleY;
  float jumpStrength;

   //******************************** Brandon
  int activatedPad1;
  int activatedPad2;
  int activatedPad3;
  //*********************************


  //*****************************************************
  int lives;
  int painKillers;
  //****************************************************** Jake Evans

  //Gamepad variables
  //GamepadState state;
  GameController gamepad;
  


  //Physical Constants
    DirectX::SimpleMath::Vector2 accelerationDueToGravity;  //In pixels per tick per tick

  // DirectX Tool Kit objects
  std::unique_ptr<DirectX::SpriteBatch> spriteBatch;   
  std::unique_ptr<DirectX::CommonStates> commonStates; 

};

//  (c) Martyn Prigmore 2013 and beyond. All rights reserved.
//
//  Declares classes and helper methods we need to use DirectX for games programming.
//
//  Notes
//  1)  In header files I use fully qualified identifiers.  
//
//  Design simplifications for a teaching tool
//  ******************************************
//  This code is intended as a teaching tool for first year students of games 
//  programming.  It provides a very simple starting point for a Windows Store 
//  DirectX game. The design makes some simplyfying assumptions that are not 
//  realistic for a proper game, but are reasonable in a teaching tool.

//Include DirectXTK libraries
#include <CommonStates.h>
#include <SimpleMath.h>
#include <SpriteBatch.h>
#include <SpriteFont.h>
//Include C++ standard libraries
#include <vector>

#include "GameController.h"
#include "BackgroundLoader.h"
#include "PlatformLoader.h"
#include "HeroSprite.h"
#include "Enemy.h"
#include "EnemyHead.h"
#include "Bullet.h"

//*********************************************Brandon
#include "PressurePlate.h"
//****************************************************

//*********************************
#include "Hud.h"
#include "Hearts.h"
#include "PainKiller1.h"
#include "SpriteEnemiesLeft.h"
//************************************** Jake Evans

///<summary>Helper enumerated type: enumerates the possible game states.</summary>
enum class GameState
{
  MainMenu
  , PlayingGame
  , GameOver
  , GamePaused
};

enum class LevelState
{
  MainMenu
  , LevelOne
  , LevelTwo
  , LevelThree
  , LevelFour
  , LevelFive
  , LevelSix
  , LevelSeven
  , LevelEight
  , LevelNine
  , LevelTen
};

///<summary>Simple class for implementing a sprite based (i.e. 2D) game</summary>
///<remarks>
///The class is defined as a plain old C++ class, but to work in a Windows Store
///app it must use the C++ Component Extensions (C++/CX). 
///To work with DirectX component object model (COM) objects it must also use the 
///Windows Runtime C++ Template Library (WRL). These are advanced topics, but 
///don't impact on our use of this class to develop a sprite game.
///Further information at:
///http://msdn.microsoft.com/en-us/library/windows/apps/hh438466(v=vs.110).aspx
///http://msdn.microsoft.com/en-us/library/windows/apps/hh699871(v=vs.120).aspx
///</remarks>
class SpriteGame
{
public:
  SpriteGame();
  virtual ~SpriteGame();

  ///<summary>Creates resources that are independent of the Direct3D device</summary>
  ///<remarks>
  ///For example, could set up Direct2D, DirectWrite and Windows Imaging Component 
  ///(WIC) in this method.
  ///</remarks>
  virtual void CreateDeviceIndependentResources();

  ///<summary>Creates the Direct3D device and related resources</summary>
  virtual void CreateDeviceResources();

  ///<summary>Creates resources that depend on the size of the window<summary>
  ///<remarks>
  ///These resources need to the re-initialized when the window changes size.
  ///</remarks>
  virtual void CreateWindowSizeDependentResources();

  ///<summary>Initializes the Direct3D objects and prepares them for use</summary>
  virtual void Initialize(Windows::UI::Core::CoreWindow^ ourWindow);

  ///<summary>Updates the state of game objects.</summary>
  ///<param name='tickTotal'>The total number of ticks since the game started.</param>
  ///<param name='tickDelta'>The number of ticks that have elapsed since the last update.</param>
  ///<param name='tickTotal'>The total time, in seconds, since the game started.</param>
  ///<param name='tickDelta'>The number of seconds that have elapsed since the last update.</param>
  ///<remarks>
  ///<para>
  ///In BasicTimer.h the TICKS_PER_SECOND macro defines how many ticks occur in
  ///a single second.  The default value is 24.  
  ///</para>
  ///<para>
  ///Strictly speaking tickTotal is the number of ticks since the *timer* was last
  ///reset.  For us, this is effectively the same as "since the game started".
  ///</para>
  ///</remarks>
  virtual void Update(int tickTotal, int tickDelta, float timeTotal, float timeDelta);

  ///<summary>Renders (i.e. draws) the game graphics for a single animation frame</summary>
  virtual void Render();

  ///<summary> Presents the rendered animation frame to the monitor.</summary>
  virtual void Present();
  

protected:
  ///<summary>Helper method: convert measurements in DIPs to measurements in physical pixels</summary>
  ///<remarks>
  ///Direct2D uses Device Independent Pixels (DIPs) as its basic unit.  One DIP 
  ///is defined as 1/96 of a logical inch.  
  ///Each monitor will have a Dots Per (Logical) Inch setting: it's DPI.  With 
  ///a DPI of 96, one DIP is equal to one physical pixel.  However, with a higher 
  ///DPI this breaks down.  E.g. at a DPI of 120 one DIP is equal to 1.25 physical
  ///pixels. 
  ///Direct2D uses the DPI to automatically convert DIPs to physical pixels when
  ///it draws things to the screen.  The CoreWindow object also uses DIPs to 
  ///specify its bounds and pointer position.
  ///However in Direct3D the swap chain and render target need to be specified in 
  ///physical pixels hence this method!
  ///</remarks>
  float ConvertDipsToPixels(float dips);
  bool EnemyDead(bool enemyDead);
  void LevelReset();
  void LoadLevel1();
  void DrawLevel1();
  void LoadLevel2();
  void DrawLevel2();
  void LoadLevel3();
  void DrawLevel3();
  void LoadLevel4();
  void DrawLevel4();
  void LoadLevel5();
  void DrawLevel5();
  void LoadLevel6();
  void DrawLevel6();
  void LoadLevel7();
  void DrawLevel7();
  void LoadLevel8();
  void DrawLevel8();
  void LoadLevel9();
  void DrawLevel9();
  void LoadLevel10();
  void DrawLevel10();

  //The CoreWindow object for this app
  Platform::Agile<Windows::UI::Core::CoreWindow> coreWindow;

  //The DirectX objects
  //Note: These objects are automatically freed when their last reference goes 
  //  out of scope when the app terminates; see http://msdn.microsoft.com/en-us/library/windows/apps/hh780565.aspx.
  Microsoft::WRL::ComPtr<ID3D11Device1> d3dDevice;
  Microsoft::WRL::ComPtr<ID3D11DeviceContext1> d3dDeviceContext;
  Microsoft::WRL::ComPtr<IDXGISwapChain1> swapChain;
  Microsoft::WRL::ComPtr<ID3D11RenderTargetView> backBufferTarget;

  //Window and graphics hardware properties, cached for convenient access
  D3D_FEATURE_LEVEL d3dFeatureLevel;
  Windows::Foundation::Size backBufferTargetSize; 
  Windows::Foundation::Rect windowBounds;
  Windows::Graphics::Display::DisplayOrientations displayOrientation;

  //Screen Size used to detect the size of the screen for scaling the game to a specific screen.
  Windows::Foundation::Rect screenSize;

  // DirectX Tool Kit objects
  std::unique_ptr<DirectX::SpriteBatch> spriteBatch;   
  std::unique_ptr<DirectX::CommonStates> commonStates;
  std::unique_ptr<DirectX::SpriteFont> spriteFont;
  DirectX::SpriteEffects spriteEffect;

  //Game World
  GameState state;
  LevelState levelState;
  GameController gamepad;
  const DirectX::SimpleMath::Color menuOptionColourSelected;
  const DirectX::SimpleMath::Color menuOptionColourDefault;
  std::vector<DirectX::SimpleMath::Color> mainMenuOptionColour;
  std::vector<DirectX::SimpleMath::Color> pauseMenuOptionColour;
  std::vector<DirectX::SimpleMath::Color> gameOverMenuOptionColour; 

  DirectX::SimpleMath::Vector2 heroPosition;  //Pixels
  DirectX::SimpleMath::Vector2 heroVelocity;  //Pixels per second
  const int frameWidth;
  bool highlightEnemy;
  bool killedEnemy;
  DirectX::SimpleMath::Vector2 enemyDeathPosition;
  void DeathPosition(DirectX::SimpleMath::Vector2 deathPosition);
  int currentLevel;
  int enemySelected;
  int enemyQuantity;
  float scaleX;
  float scaleY;
  bool steppedOnPlate;

  //**********************************************************
  Hud hud;
  Hearts hearts;
  SpriteEnemiesLeft spriteEnemiesLeft;
  int lives;
  int painKillers;
  Hearts heart1;
  Hearts heart2;
  Hearts heart3;
  Hearts heart_Asset;
  PainKiller1 painKiller1;
  PainKiller1 painKiller2;
  PainKiller1 painKiller3;
  PainKiller1 painKiller_Asset;
  //************************************************************ Jake Evans

  Enemy enemy1;
  Enemy enemy2;
  Enemy enemy3;
  EnemyHead head1;
  EnemyHead head2;
  EnemyHead head3;
  HeroSprite hero;
  
  /*BackgroundLoader level1Background;
  PlatformLoader level1Platform1;
  PlatformLoader level1Platform2;*/

  BackgroundLoader background;
  PlatformLoader platform1;
  PlatformLoader platform2;
  PlatformLoader platform3;
  PlatformLoader platform4;
  PlatformLoader platform5;
  PlatformLoader platform6;
  PlatformLoader door;
  PlatformLoader ladder;
  PressurePlate plate1;
  PressurePlate plate2;
  PressurePlate plate3;
  Bullet bullet;
  
  PlatformLoader chainLift;
};



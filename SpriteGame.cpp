//  (c) Martyn Prigmore 2013 and beyond. All rights reserved.
//
//  Defines the classes and helper methods declared in SpriteGame.h 
//
//  Notes
//  1)  In source code files I use namespaces to avoid the need for fully qualified identifiers.  

//Note: pch.h is a set of pre-compiled headers, providing access to the major system
//  headers.  In our case this includes various DirectX header files.
#include "pch.h"
#include <Utilities.h>

#include <SpriteGame.h>
//Include DirectXTK libraries
#include <WICTextureLoader.h>

#include <GameController.h>

using namespace Microsoft::WRL;
using namespace Windows::UI::Core;
using namespace Windows::Graphics::Display;
using namespace DirectX;
using namespace Platform;
using namespace DirectX::SimpleMath;

SpriteGame::SpriteGame() :
	d3dDevice ( nullptr )
	, d3dDeviceContext ( nullptr )
	, swapChain ( nullptr )
	, backBufferTarget ( nullptr )
	, d3dFeatureLevel ( D3D_FEATURE_LEVEL_11_1 )
	, spriteBatch ( nullptr )
	, commonStates ( nullptr )
	, spriteFont ( nullptr )
	// Initialize the Game World objects
	, state ( GameState::MainMenu )
	, levelState ( LevelState::LevelOne )
	, gamepad ( )
	, menuOptionColourDefault ( DirectX::Colors::AntiqueWhite.v )  
	, menuOptionColourSelected ( DirectX::Colors::GreenYellow.v ) 
	, mainMenuOptionColour ( 2, menuOptionColourDefault ) 
	, pauseMenuOptionColour ( 2, menuOptionColourDefault )
	, gameOverMenuOptionColour ( 1, menuOptionColourDefault )
	, background ( )
	, platform1 ( )
	, platform2 ( )
	, platform3 ( )
	, platform4 ( )
	, platform5 ( )
	, platform6 ( )
	, door ( )
	, ladder ( )
	, hero ( )
	, frameWidth ( )
	, heroVelocity ( 0.0f, 0.0f )
	, heroPosition ( 0.0f, 0.0f )
	, enemy1 ( )
	, enemy2 ( )
	, enemy3 ( )
	, highlightEnemy ( false )
	, killedEnemy ( false )
	, enemyDeathPosition ( 0.0f )
	, head1 ( )
	, head2 ( )
	, head3 ( )
	, currentLevel ( 1 )
	, enemySelected ( 0 )
	, enemyQuantity ( 0 )
	, bullet ( )
	, scaleX ( 0.65f )
	, scaleY ( 0.65f )
	//*********************************************
	, hud ( )
	, hearts ( )
	, painKiller1 ( )
	, painKiller2 ( )
	, painKiller3 ( )
	, spriteEnemiesLeft ( )
	, lives ( 3 )
	, painKillers ( 3 )
	, heart_Asset ( )
	, painKiller_Asset ( )
	//******************************************** Jake Evans

	//********************************************* Brandon
	, plate1 (  )
	, plate2 (  )
	, plate3 (  )
	//********************************************

	, chainLift ( )
	, steppedOnPlate ( false )
{

}

SpriteGame::~SpriteGame()
{
}

void SpriteGame::CreateDeviceIndependentResources() 
{
	//None for a basic game
}

void SpriteGame::CreateDeviceResources()
{
	UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;  

	D3D_FEATURE_LEVEL featureLevels[] = 
	{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
	};
	unsigned int totalFeatureLevels = ARRAYSIZE ( featureLevels );

	//First, get a D3D11 device and device context
	ComPtr<ID3D11Device> device;
	ComPtr<ID3D11DeviceContext> context;
	ThrowIfFailed(
		D3D11CreateDevice(
		nullptr, 
		D3D_DRIVER_TYPE_HARDWARE, 
		nullptr,
		creationFlags,
		featureLevels,
		totalFeatureLevels,
		D3D11_SDK_VERSION,
		&device,
		&this->d3dFeatureLevel,
		&context
		)
		);

	//Second, convert the D3D11 objects to D3D11.1
	ThrowIfFailed(
		device.As(&this->d3dDevice)
		);

	ThrowIfFailed(
		context.As(&this->d3dDeviceContext)
		);
}

void SpriteGame::CreateWindowSizeDependentResources()
{
	this->screenSize = this->coreWindow->Bounds;  //measured in DIPs

	this->windowBounds.Height = 1080.0f;
	this->windowBounds.Width = 1920.0f;

	float windowHeightPhysicalPixels = ConvertDipsToPixels(this->windowBounds.Height);
	float windowWidthPhysicalPixels = ConvertDipsToPixels(this->windowBounds.Width);

	//Swap height and width if the window is not in landscape
	this->displayOrientation = DisplayProperties::CurrentOrientation;
	if (this->displayOrientation == DisplayOrientations::Portrait
		|| this->displayOrientation == DisplayOrientations::PortraitFlipped)
	{
		this->backBufferTargetSize.Height = windowWidthPhysicalPixels;
		this->backBufferTargetSize.Width = windowHeightPhysicalPixels;
	}
	else
	{
		this->backBufferTargetSize.Height = windowHeightPhysicalPixels;
		this->backBufferTargetSize.Width = windowWidthPhysicalPixels;
	}

	if (this->swapChain != nullptr)
	{
		//Resize the existing swap chain
		ThrowIfFailed(
			this->swapChain->ResizeBuffers(
			2,
			static_cast<UINT>(this->backBufferTargetSize.Width),
			static_cast<UINT>(this->backBufferTargetSize.Height),
			DXGI_FORMAT_B8G8R8A8_UNORM,
			0
			)
			);
	}
	else
	{
		//Create a swap chain
		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {0};
		swapChainDesc.Width = static_cast<UINT>(this->backBufferTargetSize.Width);
		swapChainDesc.Height = static_cast<UINT>(this->backBufferTargetSize.Height);
		swapChainDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		swapChainDesc.Stereo = false;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = 2;
		swapChainDesc.Scaling = DXGI_SCALING_NONE;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
		swapChainDesc.Flags = 0;

		ComPtr<IDXGIDevice1> dxgiDevice;
		ThrowIfFailed(
			this->d3dDevice.As(&dxgiDevice)
			);

		ComPtr<IDXGIAdapter> dxgiAdapter; 
		ThrowIfFailed(
			dxgiDevice->GetAdapter(&dxgiAdapter)
			);

		ComPtr<IDXGIFactory2> dxgiFactory;
		ThrowIfFailed(
			dxgiAdapter->GetParent(__uuidof(IDXGIFactory2), &dxgiFactory)
			);

		Windows::UI::Core::CoreWindow^ ourWindow = this->coreWindow.Get();
		ThrowIfFailed(
			dxgiFactory->CreateSwapChainForCoreWindow(
			this->d3dDevice.Get(),
			reinterpret_cast<IUnknown*>(ourWindow),
			&swapChainDesc,
			nullptr,
			&this->swapChain
			)
			);

		ThrowIfFailed(
			dxgiDevice->SetMaximumFrameLatency(1)
			);
	}

	//Create the render target view from the swap chain back buffer
	ComPtr<ID3D11Texture2D> backBuffer;
	ThrowIfFailed(
		this->swapChain->GetBuffer(
		0,
		__uuidof(ID3D11Texture2D),
		&backBuffer
		)
		);

	ThrowIfFailed(
		this->d3dDevice->CreateRenderTargetView(
		backBuffer.Get(),
		nullptr,
		&this->backBufferTarget
		)
		);

	if (screenSize.Height == 1080.0f)
	{
		this->scaleX = 1.0f;
		this->scaleY = 1.0f;
		//Set the rendering viewport to target the entire window
		CD3D11_VIEWPORT viewport(
			0.0f,
			0.0f,
			this->backBufferTargetSize.Width,
			this->backBufferTargetSize.Height
			);
		this->d3dDeviceContext->RSSetViewports(1, &viewport);
	}
	else if (screenSize.Height == 1024)
	{
		this->scaleX = 0.668f;
		this->scaleY = 0.668f;
		//Set the rendering viewport to target the entire window
		CD3D11_VIEWPORT viewport(
			0.0f,
			150.0f,
			this->backBufferTargetSize.Width,
			this->backBufferTargetSize.Height
			);
		this->d3dDeviceContext->RSSetViewports(1, &viewport);
	}
	else if (screenSize.Height == 1200.0f)
	{
		this->scaleX = 1.0f;
		this->scaleY = 1.0f;
		//Set the rendering viewport to target the entire window
		CD3D11_VIEWPORT viewport(
			0.0f,
			0.0f,
			this->backBufferTargetSize.Width,
			this->backBufferTargetSize.Height
			);
		this->d3dDeviceContext->RSSetViewports(1, &viewport);
	}
	else if (screenSize.Height == 1440.0f)
	{
		this->scaleX = 1.3f;
		this->scaleY = 1.3f;
		//Set the rendering viewport to target the entire window
		CD3D11_VIEWPORT viewport(
			0.0f,
			0.0f,
			this->backBufferTargetSize.Width,
			this->backBufferTargetSize.Height
			);
		this->d3dDeviceContext->RSSetViewports(1, &viewport);
	}
	else
	{
		this->scaleX = 0.5f;
		this->scaleY = 0.5f;
		//Set the rendering viewport to target the entire window
		CD3D11_VIEWPORT viewport(
			0.0f,
			0.0f,
			this->backBufferTargetSize.Width,
			this->backBufferTargetSize.Height
			);
		this->d3dDeviceContext->RSSetViewports(1, &viewport);
	}

}

void SpriteGame::Initialize(CoreWindow^ ourWindow)
{
	this->coreWindow = ourWindow;

	CreateDeviceIndependentResources();
	CreateDeviceResources();
	CreateWindowSizeDependentResources();

	//Initialize member variables
	this->spriteBatch = std::unique_ptr<SpriteBatch>(new SpriteBatch(this->d3dDeviceContext.Get()));
	this->commonStates = std::unique_ptr<CommonStates>(new CommonStates(this->d3dDevice.Get()));
	this->spriteFont = std::unique_ptr<SpriteFont>(new SpriteFont(this->d3dDevice.Get(), L"Assets/italic.spritefont"));

	if (this->windowBounds.Width == 1280)
	{
		this->scaleX = 0.75f;
		this->scaleY = 0.94f;
	}


	switch (this->levelState)
	{
	case LevelState::LevelOne:
		this->currentLevel = 1;
		this->LevelReset();
		this->LoadLevel1();
		break;
	case LevelState::LevelTwo:
		this->currentLevel = 2;
		this->LevelReset();
		this->LoadLevel2();
		break;
	case LevelState::LevelThree:
		this->currentLevel = 3;
		this->LevelReset();
		this->LoadLevel3();
		break;
	case LevelState::LevelFour:
		this->currentLevel = 4;
		this->LevelReset();
		this->LoadLevel4();
		break;
	case LevelState::LevelFive:
		this->currentLevel = 5;
		this->LevelReset();
		this->LoadLevel5();
		break;
	case LevelState::LevelSix:
		this->currentLevel = 6;
		this->LevelReset();
		this->LoadLevel6();
		break;
	case LevelState::LevelSeven:
		this->currentLevel = 7;
		this->LevelReset();
		this->LoadLevel7();
		break;
	case LevelState::LevelEight:
		this->currentLevel = 8;
		this->LevelReset();
		this->LoadLevel8();
		break;
	case LevelState::LevelNine:
		this->currentLevel = 9;
		this->LevelReset();
		this->LoadLevel9();
		break;
	case LevelState::LevelTen:
		this->currentLevel = 10;
		this->LevelReset();
		this->LoadLevel10();
		break;
	}



}

void SpriteGame::Update(int tickTotal, int tickDelta, float timeTotal, float timeDelta)
{  
	GameController oldGamepad = this->gamepad;  
	this->gamepad.Update(); 

	//***************************************************
	this->lives = hero.ReturnLives();
	this->painKillers = hero.ReturnPainKillers();

	
	//************************************************************** Jake Evans

	//********************************************************************************************************** Brandon
	if(hero.IntersectsPad1(plate1) == 1)
	{
		this->plate1.Initialize(L"Assets/Pressurepaddown.png", this->d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
	}

	if(hero.IntersectsPad2(plate2) == 1)
	{
		this->plate2.Initialize(L"Assets/Pressurepaddown.png", this->d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
	}

	if(hero.IntersectsPad3(plate3) == 1)
	{
		this->plate3.Initialize(L"Assets/Pressurepaddown.png", this->d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
	}


	//*************************************************************************************************************

	if (this->currentLevel == 1)
	{
		if(enemy1.IntersectsPad1(plate1) == 1)
		{
			this->chainLift.LowerLift(tickTotal, tickDelta, timeDelta);
			this->steppedOnPlate = true;
		}
		else if (!enemy1.IntersectsPad1(plate1) == 1 && this->steppedOnPlate == true)
		{
			this->chainLift.RaiseLift(tickTotal, tickDelta, timeDelta);
			//this->steppedOnPlate = false;
		}
		else
		{

		}
	}

	else if (this->currentLevel == 3)
	{
		if(enemy1.IntersectsPad1(plate1) == 1 && enemy2.IntersectsPad2(plate2) == 1)
		{
			//this->plate2.Initialize(L"Assets/Pressurepaddown.png", this->d3dDevice, this->d3dDeviceContext);
			this->door.Initialize(L"Assets/OpenDoor.png", this->d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
			this->hero.LockDoor(false);
			this->door.Reset(Vector2(1824.0f, 830.0f));
		}
		else if(enemy1.IntersectsPad2(plate2) == 1 && enemy2.IntersectsPad1(plate1) == 1)
		{
			//this->plate2.Initialize(L"Assets/Pressurepaddown.png", this->d3dDevice, this->d3dDeviceContext);
			this->door.Initialize(L"Assets/OpenDoor.png", this->d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
			this->hero.LockDoor(false);
			this->door.Reset(Vector2(1824.0f, 830.0f));
		}
		else
		{
			this->hero.LockDoor(true);
			this->door.Initialize(L"Assets/ClosedDoor.png", this->d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
			this->door.Reset(Vector2(1901.0f, 830.0f));
		}
	}
	else
	{
		this->hero.LockDoor(false);
	}

		if(this->currentLevel == 2)
	{
		if(enemy1.IntersectsPad1(plate1) == 1)
		{
			//this->plate2.Initialize(L"Assets/Pressurepaddown.png", this->d3dDevice, this->d3dDeviceContext);
			this->door.Initialize(L"Assets/OpenDoor.png", this->d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
			this->hero.LockDoor(false);
			this->door.Reset(Vector2(1870.0f, 800.0f));
			this->chainLift.LowerLift(tickTotal, tickDelta, timeDelta);
		}
		else
		{
			this->hero.LockDoor(true);
			this->chainLift.RaiseLift(tickTotal, tickDelta, timeDelta);
		}
		

	}

	//********************************************************************************************************** Brandon
	if(enemy1.IntersectsPad1(plate1) == 1)
	{
		this->plate1.Initialize(L"Assets/Pressurepaddown.png", this->d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);

	}


	if(enemy1.IntersectsPad2(plate2) == 1)
	{
		this->plate2.Initialize(L"Assets/Pressurepaddown.png", this->d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);

	}

	if(enemy1.IntersectsPad3(plate3) == 1)
	{
		this->plate3.Initialize(L"Assets/Pressurepaddown.png", this->d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);

	}

	if(enemy2.IntersectsPad1(plate1) == 1)
	{
		this->plate1.Initialize(L"Assets/Pressurepaddown.png", this->d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);

	}


	if(enemy2.IntersectsPad2(plate2) == 1)
	{
		this->plate2.Initialize(L"Assets/Pressurepaddown.png", this->d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);

	}

	if(enemy2.IntersectsPad3(plate3) == 1)
	{
		this->plate3.Initialize(L"Assets/Pressurepaddown.png", this->d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);

	}

	if(enemy3.IntersectsPad1(plate1) == 1)
	{
		this->plate1.Initialize(L"Assets/Pressurepaddown.png", this->d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);

	}


	if(enemy3.IntersectsPad2(plate2) == 1)
	{
		this->plate2.Initialize(L"Assets/Pressurepaddown.png", this->d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);

	}

	if(enemy3.IntersectsPad3(plate3) == 1)
	{
		this->plate3.Initialize(L"Assets/Pressurepaddown.png", this->d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);

	}

	if(enemy1.IntersectsPad1(plate1) == 0 && enemy2.IntersectsPad1(plate1) == 0 && enemy3.IntersectsPad1(plate1) == 0 && hero.IntersectsPad1(plate1) == 0)
	{
		this->plate1.Initialize(L"Assets/Pressurepadup.png", this->d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);

	}

	if(enemy1.IntersectsPad2(plate2) == 0 && enemy2.IntersectsPad2(plate2) == 0 && enemy3.IntersectsPad2(plate2) == 0 && hero.IntersectsPad2(plate2) == 0)
	{
		this->plate2.Initialize(L"Assets/Pressurepadup.png", this->d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);

	}

	if(enemy1.IntersectsPad3(plate3) == 0 && enemy2.IntersectsPad3(plate3) == 0 && enemy3.IntersectsPad3(plate3) == 0 && hero.IntersectsPad3(plate3) == 0)
	{
		this->plate3.Initialize(L"Assets/Pressurepadup.png", this->d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);

	}
	//*************************************************************************************************************

	if(this->gamepad.IsConnected())
	{
		if (this->highlightEnemy == false)
		{
			if (this->currentLevel == 1)
			{
				this->hero.DeathByEnemy1(enemy1);
			}
			else if (this->currentLevel == 2)
			{
				this->hero.DeathByEnemy1(enemy1);
				this->hero.DeathByEnemy2(enemy2);
			}
			else if (this->currentLevel == 3)
			{
				this->hero.DeathByEnemy1(enemy1);
				this->hero.DeathByEnemy2(enemy2);
			}
			else if (this->currentLevel == 4)
			{
				this->hero.DeathByEnemy1(enemy1);
				this->hero.DeathByEnemy2(enemy2);
			}
			else if (this->currentLevel == 5)
			{
				this->hero.DeathByEnemy1(enemy1);
				this->hero.DeathByEnemy2(enemy2);
				this->hero.DeathByEnemy3(enemy3);
			}
			else if (this->currentLevel == 6)
			{
				this->hero.DeathByEnemy1(enemy1);
				this->hero.DeathByEnemy2(enemy2);
			}
			else if (this->currentLevel == 7)
			{
				this->hero.DeathByEnemy1(enemy1);
				this->hero.DeathByEnemy2(enemy2);
				this->hero.DeathByEnemy3(enemy3);
			}
			else if (this->currentLevel == 8)
			{
				this->hero.DeathByEnemy1(enemy1);
				this->hero.DeathByEnemy2(enemy2);
			}
			else if (this->currentLevel == 9)
			{
				this->hero.DeathByEnemy1(enemy1);
			}
			else if (this->currentLevel == 10)
			{

			}

		}


		if(this->gamepad.IsButtonDown(XINPUT_GAMEPAD_RIGHT_SHOULDER)   
			&& !oldGamepad.IsButtonDown(XINPUT_GAMEPAD_RIGHT_SHOULDER) && this->highlightEnemy == false && this->enemyQuantity != 0 
			&& this->painKillers > 0)
		{
			this->hero.Initialize(L"Assets/HeroMindControl.png", this->d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
			this->hero.NewSheetLength(2);
			this->highlightEnemy = true;
			this->enemySelected = 1;
			this->hero.HighlightEnemy(true);
			this->enemy1.HighlightEnemy(true);
		} 
		else if(this->gamepad.IsButtonDown(XINPUT_GAMEPAD_RIGHT_SHOULDER)   
			&& !oldGamepad.IsButtonDown(XINPUT_GAMEPAD_RIGHT_SHOULDER) && this->highlightEnemy == true
			&& this->painKillers > 0)
		{
			if (this->enemySelected < this->enemyQuantity && this->enemySelected == 1)
			{
				this->enemySelected = 2;
				this->enemy1.HighlightEnemy(false);
				this->enemy2.HighlightEnemy(true);
			}
			else if (this->enemySelected < this->enemyQuantity && this->enemySelected == 2)
			{
				this->enemySelected = 3;
				this->enemy2.HighlightEnemy(false);
				this->enemy3.HighlightEnemy(true);
			}
			else if (this->enemySelected == this->enemyQuantity)
			{
				this->hero.Initialize(L"Assets/HeroJumpIdle.png", this->d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
				this->hero.NewSheetLength(6);
				this->highlightEnemy = false;
				this->hero.HighlightEnemy(false);
				this->enemy1.HighlightEnemy(false);
				this->enemy2.HighlightEnemy(false);
				this->enemy3.HighlightEnemy(false);
			}
		} 

		if(this->gamepad.IsButtonDown(XINPUT_GAMEPAD_LEFT_SHOULDER)   
			&& !oldGamepad.IsButtonDown(XINPUT_GAMEPAD_LEFT_SHOULDER) && this->highlightEnemy == false && this->enemyQuantity != 0
			&& this->painKillers > 0)
		{
			this->hero.Initialize(L"Assets/HeroMindControl.png", this->d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
			this->hero.NewSheetLength(2);
			this->highlightEnemy = true;
			this->enemySelected = this->enemyQuantity;
			this->hero.HighlightEnemy(true);
			if (this->enemyQuantity == 1)
			{
				this->enemy1.HighlightEnemy(true);
			}
			else if (this->enemyQuantity == 2)
			{
				this->enemy2.HighlightEnemy(true);
			}
			else if (this->enemyQuantity == 3)
			{
				this->enemy3.HighlightEnemy(true);
			}
		} 
		else if(this->gamepad.IsButtonDown(XINPUT_GAMEPAD_LEFT_SHOULDER)   
			&& !oldGamepad.IsButtonDown(XINPUT_GAMEPAD_LEFT_SHOULDER) && this->highlightEnemy == true
			&& this->painKillers > 0)
		{
			if (this->enemySelected > 0 && this->enemySelected == 3)
			{
				this->enemySelected = 2;
				this->enemy3.HighlightEnemy(false);
				this->enemy2.HighlightEnemy(true);
			}
			else if (this->enemySelected > 0 && this->enemySelected == 2)
			{
				this->enemySelected = 1;
				this->enemy2.HighlightEnemy(false);
				this->enemy1.HighlightEnemy(true);
			}
			else if (this->enemySelected > 0 && this->enemySelected == 1)
			{
				this->hero.Initialize(L"Assets/HeroJumpIdle.png", this->d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
				this->hero.NewSheetLength(6);
				this->enemySelected = 0;
				this->highlightEnemy = false;
				this->hero.HighlightEnemy(false);
				this->enemy1.HighlightEnemy(false);
				this->enemy2.HighlightEnemy(false);
				this->enemy3.HighlightEnemy(false);
			}
		}

		if (highlightEnemy == true && this->enemySelected == 1 && this->gamepad.IsButtonDown (XINPUT_GAMEPAD_Y)
			&& !oldGamepad.IsButtonDown (XINPUT_GAMEPAD_Y) && this->painKillers > 0)
		{
			this->enemy1.KillEnemy(0, true);
			this->enemy1.Initialize(L"Assets/BugDeathSpriteBody.png", this->d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
			this->head1.Initialize(L"Assets/BugDeathSpriteHead.png", this->d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
			this->head1.SetJustSpawned(true);
			this->head1.Reset(this->enemy1.GetPosition());
			this->head1.Draw();
			this->hero.PainKillers();
			this->highlightEnemy = false;
			this->hero.HighlightEnemy(false);
			this->hero.NewSheetLength(6);
			this->hero.Initialize(L"Assets/HeroJumpIdle.png", this->d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
			this->enemy1.HighlightEnemy(false);
			this->enemy2.HighlightEnemy(false);
			this->enemy3.HighlightEnemy(false);
			this->enemySelected = 0;
		}
		else if (highlightEnemy == true && this->enemySelected == 2 && this->gamepad.IsButtonDown (XINPUT_GAMEPAD_Y)
			&& !oldGamepad.IsButtonDown (XINPUT_GAMEPAD_Y) && this->painKillers > 0)
		{
			this->enemy2.KillEnemy(0, true);
			this->enemy2.Initialize(L"Assets/BugDeathSpriteBody.png", this->d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
			this->head2.Initialize(L"Assets/BugDeathSpriteHead.png", this->d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
			this->head2.SetJustSpawned(true);
			this->head2.Reset(this->enemy2.GetPosition());
			this->head2.Draw();
			this->hero.PainKillers();
			this->highlightEnemy = false;
			this->hero.HighlightEnemy(false);
			this->hero.NewSheetLength(6);
			this->hero.Initialize(L"Assets/HeroJumpIdle.png", this->d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
			this->enemy1.HighlightEnemy(false);
			this->enemy2.HighlightEnemy(false);
			this->enemy3.HighlightEnemy(false);
			this->enemySelected = 0;
		}
		else if (highlightEnemy == true && this->enemySelected == 3 && this->gamepad.IsButtonDown (XINPUT_GAMEPAD_Y)
			&& !oldGamepad.IsButtonDown (XINPUT_GAMEPAD_Y) && this->painKillers > 0)
		{
			this->enemy3.KillEnemy(0, true);
			this->enemy3.Initialize(L"Assets/BugDeathSpriteBody.png", this->d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
			this->head3.Initialize(L"Assets/BugDeathSpriteHead.png", this->d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
			this->head3.SetJustSpawned(true);
			this->head3.Reset(this->enemy3.GetPosition());
			this->head3.Draw();
			this->hero.PainKillers();
			this->highlightEnemy = false;
			this->hero.HighlightEnemy(false);
			this->hero.NewSheetLength(6);
			this->hero.Initialize(L"Assets/HeroJumpIdle.png", this->d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
			this->enemy1.HighlightEnemy(false);
			this->enemy2.HighlightEnemy(false);
			this->enemy3.HighlightEnemy(false);
			this->enemySelected = 0;
		}

		if (highlightEnemy == false && this->gamepad.IsButtonDown (XINPUT_GAMEPAD_DPAD_RIGHT))
		{
			this->hero.Initialize(L"Assets/HeroWalkSprite.png", this->d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
		}
		else if (highlightEnemy == false && this->gamepad.IsButtonDown (XINPUT_GAMEPAD_DPAD_LEFT))
		{
			this->hero.Initialize(L"Assets/HeroWalkSprite.png", this->d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
		}
		else
		{

		}

		switch (this->state)
		{
		case GameState::MainMenu:
			//Do main menu updates
			/*this->LevelReset();
			this->LoadLevel1();*/
			this->hero.ResetLives();
			this->hero.ResetPainkillers();
			if ( this->gamepad.IsButtonDown(XINPUT_GAMEPAD_START ) )
			{
				//First menu option is "Press [Start] to play"
				this->mainMenuOptionColour[0] = this->menuOptionColourSelected;
				this->mainMenuOptionColour[1] = this->menuOptionColourDefault;
			}
			else if ( this->gamepad.IsButtonDown(XINPUT_GAMEPAD_BACK ) )
			{
				//Second menu option is "Press [Back] to quit
				this->mainMenuOptionColour[0] = this->menuOptionColourDefault;
				this->mainMenuOptionColour[1] = this->menuOptionColourSelected;
			}
			else
			{
				this->mainMenuOptionColour[0] = this->menuOptionColourDefault;
				this->mainMenuOptionColour[1] = this->menuOptionColourDefault;
			}
			if(!this->gamepad.IsButtonDown(XINPUT_GAMEPAD_START)  
				&& oldGamepad.IsButtonDown(XINPUT_GAMEPAD_START))   
			{
				this->state = GameState::PlayingGame;
			}      
			if(!this->gamepad.IsButtonDown(XINPUT_GAMEPAD_BACK)  
				&& oldGamepad.IsButtonDown(XINPUT_GAMEPAD_BACK))   
			{
				this->state = GameState::GameOver;
			}      
			break;
		case GameState::PlayingGame:
			//Do gameplay updates
			if(!this->gamepad.IsButtonDown(XINPUT_GAMEPAD_BACK)   
				&& oldGamepad.IsButtonDown(XINPUT_GAMEPAD_BACK)) 
			{
				this->state = GameState::MainMenu;
			}
			if(!this->gamepad.IsButtonDown(XINPUT_GAMEPAD_START)   
				&& oldGamepad.IsButtonDown(XINPUT_GAMEPAD_START)) 
			{
				this->state = GameState::GamePaused;
			}
			//Game Over if all lives are lost
			if(this->lives == 0) 
			{
				this->state = GameState::GameOver;
				this->hero.ResetLives();
				this->levelState = LevelState::LevelOne;
				this->LevelReset();
				this->LoadLevel1();
				this->currentLevel = 1;
				this->painKillers = 3;
			}
			//Control the sprite

			switch (this->levelState)
			{
			case LevelState::LevelOne:
				//Put sprite elements into the game
				this->hero.Update(tickTotal, tickDelta, timeDelta, this->windowBounds, this->platform1, this->platform2, this->platform3, this->platform4, this->platform5, this->platform6, this->door, this->ladder, this->plate1, this->plate2, this->plate3, this->heart_Asset, this->painKiller_Asset, this->chainLift);
				this->enemy1.Update(tickTotal, tickDelta, timeDelta, this->windowBounds, this->platform1, this->platform2, this->platform3, this->platform4, this->platform5, this->platform6, this->ladder, this->plate1, this->plate2, this->plate3);
				this->enemy2.Update(tickTotal, tickDelta, timeDelta, this->windowBounds, this->platform1, this->platform2, this->platform3, this->platform4, this->platform5, this->platform6, this->ladder, this->plate1, this->plate2, this->plate3);
				this->enemy3.Update(tickTotal, tickDelta, timeDelta, this->windowBounds, this->platform1, this->platform2, this->platform3, this->platform4, this->platform5, this->platform6, this->ladder, this->plate1, this->plate2, this->plate3);

				this->head1.Update(tickTotal, tickDelta, timeDelta, this->windowBounds, this->platform1, this->platform2, this->platform3, this->platform4, this->platform5, this->platform6, this->ladder, this->plate1, this->plate2, this->plate3);
				this->head2.Update(tickTotal, tickDelta, timeDelta, this->windowBounds, this->platform1, this->platform2, this->platform3, this->platform4, this->platform5, this->platform6, this->ladder, this->plate1, this->plate2, this->plate3);
				this->head3.Update(tickTotal, tickDelta, timeDelta, this->windowBounds, this->platform1, this->platform2, this->platform3, this->platform4, this->platform5, this->platform6, this->ladder, this->plate1, this->plate2, this->plate3);


				if(!this->gamepad.IsButtonDown(XINPUT_GAMEPAD_X)   
					&& oldGamepad.IsButtonDown(XINPUT_GAMEPAD_X))
				{
					//this->levelState = LevelState::LevelTwo;
					this->LevelReset();
					this->LoadLevel1();
					
				} 
				if (this->hero.NextLevel())
				{
					this->levelState = LevelState::LevelTwo;
					this->LevelReset();
					this->LoadLevel2();
				}
				if (this->hero.HeroDead())
				{
					this->LevelReset();
					this->LoadLevel1();
				}

				break;

			case LevelState::LevelTwo:
				//Put sprite elements into the game
				this->hero.Update(tickTotal, tickDelta, timeDelta, this->windowBounds, this->platform1, this->platform2, this->platform3, this->platform4, this->platform5, this->platform6, this->door, this->ladder, this->plate1, this->plate2, this->plate3, this->heart_Asset, this->painKiller_Asset, this->chainLift);
				this->enemy1.Update(tickTotal, tickDelta, timeDelta, this->windowBounds, this->platform1, this->platform2, this->platform3, this->platform4, this->platform5, this->platform6, this->ladder, this->plate1, this->plate2, this->plate3);
				this->enemy2.Update(tickTotal, tickDelta, timeDelta, this->windowBounds, this->platform1, this->platform2, this->platform3, this->platform4, this->platform5, this->platform6, this->ladder, this->plate1, this->plate2, this->plate3);
				this->enemy3.Update(tickTotal, tickDelta, timeDelta, this->windowBounds, this->platform1, this->platform2, this->platform3, this->platform4, this->platform5, this->platform6, this->ladder, this->plate1, this->plate2, this->plate3);

				this->head1.Update(tickTotal, tickDelta, timeDelta, this->windowBounds, this->platform1, this->platform2, this->platform3, this->platform4, this->platform5, this->platform6, this->ladder, this->plate1, this->plate2, this->plate3);
				this->head2.Update(tickTotal, tickDelta, timeDelta, this->windowBounds, this->platform1, this->platform2, this->platform3, this->platform4, this->platform5, this->platform6, this->ladder, this->plate1, this->plate2, this->plate3);
				this->head3.Update(tickTotal, tickDelta, timeDelta, this->windowBounds, this->platform1, this->platform2, this->platform3, this->platform4, this->platform5, this->platform6, this->ladder, this->plate1, this->plate2, this->plate3);



				if(!this->gamepad.IsButtonDown(XINPUT_GAMEPAD_X)   
					&& oldGamepad.IsButtonDown(XINPUT_GAMEPAD_X))
				{
					this->levelState = LevelState::LevelTwo;
					this->LevelReset();
					this->LoadLevel2();
				} 
				if (this->hero.NextLevel())
				{
					this->levelState = LevelState::LevelThree;
					this->LevelReset();
					this->LoadLevel3();
				}
				if (this->hero.HeroDead())
				{
					this->LevelReset();
					this->LoadLevel2();
				}

				break;

			case LevelState::LevelThree:
				//Put sprite elements into the game
				this->hero.Update(tickTotal, tickDelta, timeDelta, this->windowBounds, this->platform1, this->platform2, this->platform3, this->platform4, this->platform5, this->platform6, this->door, this->ladder, this->plate1, this->plate2, this->plate3, this->heart_Asset, this->painKiller_Asset, this->chainLift);
				this->enemy1.Update(tickTotal, tickDelta, timeDelta, this->windowBounds, this->platform1, this->platform2, this->platform3, this->platform4, this->platform5, this->platform6, this->ladder, this->plate1, this->plate2, this->plate3);
				this->enemy2.Update(tickTotal, tickDelta, timeDelta, this->windowBounds, this->platform1, this->platform2, this->platform3, this->platform4, this->platform5, this->platform6, this->ladder, this->plate1, this->plate2, this->plate3);
				this->enemy3.Update(tickTotal, tickDelta, timeDelta, this->windowBounds, this->platform1, this->platform2, this->platform3, this->platform4, this->platform5, this->platform6, this->ladder, this->plate1, this->plate2, this->plate3);

				this->head1.Update(tickTotal, tickDelta, timeDelta, this->windowBounds, this->platform1, this->platform2, this->platform3, this->platform4, this->platform5, this->platform6, this->ladder, this->plate1, this->plate2, this->plate3);
				this->head2.Update(tickTotal, tickDelta, timeDelta, this->windowBounds, this->platform1, this->platform2, this->platform3, this->platform4, this->platform5, this->platform6, this->ladder, this->plate1, this->plate2, this->plate3);
				this->head3.Update(tickTotal, tickDelta, timeDelta, this->windowBounds, this->platform1, this->platform2, this->platform3, this->platform4, this->platform5, this->platform6, this->ladder, this->plate1, this->plate2, this->plate3);



				if(!this->gamepad.IsButtonDown(XINPUT_GAMEPAD_X)   
					&& oldGamepad.IsButtonDown(XINPUT_GAMEPAD_X))
				{
					this->levelState = LevelState::LevelThree;
					this->LevelReset();
					this->LoadLevel3();
				} 
				if (this->hero.NextLevel())
				{
					this->levelState = LevelState::LevelFour;
					this->LevelReset();
					this->LoadLevel4();
				}
				if (this->hero.HeroDead())
				{
					this->LevelReset();
					this->LoadLevel3();
				}

				break;

			case LevelState::LevelFour:
				//Put sprite elements into the game
				this->hero.Update(tickTotal, tickDelta, timeDelta, this->windowBounds, this->platform1, this->platform2, this->platform3, this->platform4, this->platform5, this->platform6, this->door, this->ladder, this->plate1, this->plate2, this->plate3, this->heart_Asset, this->painKiller_Asset, this->chainLift);
				this->enemy1.Update(tickTotal, tickDelta, timeDelta, this->windowBounds, this->platform1, this->platform2, this->platform3, this->platform4, this->platform5, this->platform6, this->ladder, this->plate1, this->plate2, this->plate3);
				this->enemy2.Update(tickTotal, tickDelta, timeDelta, this->windowBounds, this->platform1, this->platform2, this->platform3, this->platform4, this->platform5, this->platform6, this->ladder, this->plate1, this->plate2, this->plate3);
				this->enemy3.Update(tickTotal, tickDelta, timeDelta, this->windowBounds, this->platform1, this->platform2, this->platform3, this->platform4, this->platform5, this->platform6, this->ladder, this->plate1, this->plate2, this->plate3);

				this->head1.Update(tickTotal, tickDelta, timeDelta, this->windowBounds, this->platform1, this->platform2, this->platform3, this->platform4, this->platform5, this->platform6, this->ladder, this->plate1, this->plate2, this->plate3);
				this->head2.Update(tickTotal, tickDelta, timeDelta, this->windowBounds, this->platform1, this->platform2, this->platform3, this->platform4, this->platform5, this->platform6, this->ladder, this->plate1, this->plate2, this->plate3);
				this->head3.Update(tickTotal, tickDelta, timeDelta, this->windowBounds, this->platform1, this->platform2, this->platform3, this->platform4, this->platform5, this->platform6, this->ladder, this->plate1, this->plate2, this->plate3);



				if(!this->gamepad.IsButtonDown(XINPUT_GAMEPAD_X)   
					&& oldGamepad.IsButtonDown(XINPUT_GAMEPAD_X))
				{
					this->levelState = LevelState::LevelFour;
					this->LevelReset();
					this->LoadLevel4();
				} 
				if (this->hero.NextLevel())
				{
					this->levelState = LevelState::LevelFive;
					this->LevelReset();
					this->LoadLevel5();
				}
				if (this->hero.HeroDead())
				{
					this->LevelReset();
					this->LoadLevel4();
				}

				break;

			case LevelState::LevelFive:
				//Put sprite elements into the game
				this->hero.Update(tickTotal, tickDelta, timeDelta, this->windowBounds, this->platform1, this->platform2, this->platform3, this->platform4, this->platform5, this->platform6, this->door, this->ladder, this->plate1, this->plate2, this->plate3, this->heart_Asset, this->painKiller_Asset, this->chainLift);
				this->enemy1.Update(tickTotal, tickDelta, timeDelta, this->windowBounds, this->platform1, this->platform2, this->platform3, this->platform4, this->platform5, this->platform6, this->ladder, this->plate1, this->plate2, this->plate3);
				this->enemy2.Update(tickTotal, tickDelta, timeDelta, this->windowBounds, this->platform1, this->platform2, this->platform3, this->platform4, this->platform5, this->platform6, this->ladder, this->plate1, this->plate2, this->plate3);
				this->enemy3.Update(tickTotal, tickDelta, timeDelta, this->windowBounds, this->platform1, this->platform2, this->platform3, this->platform4, this->platform5, this->platform6, this->ladder, this->plate1, this->plate2, this->plate3);

				this->head1.Update(tickTotal, tickDelta, timeDelta, this->windowBounds, this->platform1, this->platform2, this->platform3, this->platform4, this->platform5, this->platform6, this->ladder, this->plate1, this->plate2, this->plate3);
				this->head2.Update(tickTotal, tickDelta, timeDelta, this->windowBounds, this->platform1, this->platform2, this->platform3, this->platform4, this->platform5, this->platform6, this->ladder, this->plate1, this->plate2, this->plate3);
				this->head3.Update(tickTotal, tickDelta, timeDelta, this->windowBounds, this->platform1, this->platform2, this->platform3, this->platform4, this->platform5, this->platform6, this->ladder, this->plate1, this->plate2, this->plate3);



				if(!this->gamepad.IsButtonDown(XINPUT_GAMEPAD_X)   
					&& oldGamepad.IsButtonDown(XINPUT_GAMEPAD_X))
				{
					this->levelState = LevelState::LevelFive;
					this->LevelReset();
					this->LoadLevel5();
				} 
				if (this->hero.NextLevel())
				{
					this->levelState = LevelState::LevelSix;
					this->LevelReset();
					this->LoadLevel6();
				}
				if (this->hero.HeroDead())
				{
					this->LevelReset();
					this->LoadLevel5();
				}

				break;

			case LevelState::LevelSix:
				//Put sprite elements into the game
				this->hero.Update(tickTotal, tickDelta, timeDelta, this->windowBounds, this->platform1, this->platform2, this->platform3, this->platform4, this->platform5, this->platform6, this->door, this->ladder, this->plate1, this->plate2, this->plate3, this->heart_Asset, this->painKiller_Asset, this->chainLift);
				this->enemy1.Update(tickTotal, tickDelta, timeDelta, this->windowBounds, this->platform1, this->platform2, this->platform3, this->platform4, this->platform5, this->platform6, this->ladder, this->plate1, this->plate2, this->plate3);
				this->enemy2.Update(tickTotal, tickDelta, timeDelta, this->windowBounds, this->platform1, this->platform2, this->platform3, this->platform4, this->platform5, this->platform6, this->ladder, this->plate1, this->plate2, this->plate3);
				this->enemy3.Update(tickTotal, tickDelta, timeDelta, this->windowBounds, this->platform1, this->platform2, this->platform3, this->platform4, this->platform5, this->platform6, this->ladder, this->plate1, this->plate2, this->plate3);

				this->head1.Update(tickTotal, tickDelta, timeDelta, this->windowBounds, this->platform1, this->platform2, this->platform3, this->platform4, this->platform5, this->platform6, this->ladder, this->plate1, this->plate2, this->plate3);
				this->head2.Update(tickTotal, tickDelta, timeDelta, this->windowBounds, this->platform1, this->platform2, this->platform3, this->platform4, this->platform5, this->platform6, this->ladder, this->plate1, this->plate2, this->plate3);
				this->head3.Update(tickTotal, tickDelta, timeDelta, this->windowBounds, this->platform1, this->platform2, this->platform3, this->platform4, this->platform5, this->platform6, this->ladder, this->plate1, this->plate2, this->plate3);



				if(!this->gamepad.IsButtonDown(XINPUT_GAMEPAD_X)   
					&& oldGamepad.IsButtonDown(XINPUT_GAMEPAD_X))
				{
					this->levelState = LevelState::LevelSix;
					this->LevelReset();
					this->LoadLevel6();
				} 
				if (this->hero.NextLevel())
				{
					this->levelState = LevelState::LevelSeven;
					this->LevelReset();
					this->LoadLevel7();
				}
				if (this->hero.HeroDead())
				{
					this->LevelReset();
					this->LoadLevel6();
				}

				break;

			case LevelState::LevelSeven:
				//Put sprite elements into the game
				this->hero.Update(tickTotal, tickDelta, timeDelta, this->windowBounds, this->platform1, this->platform2, this->platform3, this->platform4, this->platform5, this->platform6, this->door, this->ladder, this->plate1, this->plate2, this->plate3, this->heart_Asset, this->painKiller_Asset, this->chainLift);
				this->enemy1.Update(tickTotal, tickDelta, timeDelta, this->windowBounds, this->platform1, this->platform2, this->platform3, this->platform4, this->platform5, this->platform6, this->ladder, this->plate1, this->plate2, this->plate3);
				this->enemy2.Update(tickTotal, tickDelta, timeDelta, this->windowBounds, this->platform1, this->platform2, this->platform3, this->platform4, this->platform5, this->platform6, this->ladder, this->plate1, this->plate2, this->plate3);
				this->enemy3.Update(tickTotal, tickDelta, timeDelta, this->windowBounds, this->platform1, this->platform2, this->platform3, this->platform4, this->platform5, this->platform6, this->ladder, this->plate1, this->plate2, this->plate3);

				this->head1.Update(tickTotal, tickDelta, timeDelta, this->windowBounds, this->platform1, this->platform2, this->platform3, this->platform4, this->platform5, this->platform6, this->ladder, this->plate1, this->plate2, this->plate3);
				this->head2.Update(tickTotal, tickDelta, timeDelta, this->windowBounds, this->platform1, this->platform2, this->platform3, this->platform4, this->platform5, this->platform6, this->ladder, this->plate1, this->plate2, this->plate3);
				this->head3.Update(tickTotal, tickDelta, timeDelta, this->windowBounds, this->platform1, this->platform2, this->platform3, this->platform4, this->platform5, this->platform6, this->ladder, this->plate1, this->plate2, this->plate3);


				if(!this->gamepad.IsButtonDown(XINPUT_GAMEPAD_X)   
					&& oldGamepad.IsButtonDown(XINPUT_GAMEPAD_X))
				{
					this->levelState = LevelState::LevelSeven;
					this->LevelReset();
					this->LoadLevel7();
				} 
				if (this->hero.NextLevel())
				{
					this->levelState = LevelState::LevelEight;
					this->LevelReset();
					this->LoadLevel8();
				}
				if (this->hero.HeroDead())
				{
					this->LevelReset();
					this->LoadLevel7();
				}

				break;

			case LevelState::LevelEight:
				//Put sprite elements into the game
				this->hero.Update(tickTotal, tickDelta, timeDelta, this->windowBounds, this->platform1, this->platform2, this->platform3, this->platform4, this->platform5, this->platform6, this->door, this->ladder, this->plate1, this->plate2, this->plate3, this->heart_Asset, this->painKiller_Asset, this->chainLift);
				this->enemy1.Update(tickTotal, tickDelta, timeDelta, this->windowBounds, this->platform1, this->platform2, this->platform3, this->platform4, this->platform5, this->platform6, this->ladder, this->plate1, this->plate2, this->plate3);
				this->enemy2.Update(tickTotal, tickDelta, timeDelta, this->windowBounds, this->platform1, this->platform2, this->platform3, this->platform4, this->platform5, this->platform6, this->ladder, this->plate1, this->plate2, this->plate3);
				this->enemy3.Update(tickTotal, tickDelta, timeDelta, this->windowBounds, this->platform1, this->platform2, this->platform3, this->platform4, this->platform5, this->platform6, this->ladder, this->plate1, this->plate2, this->plate3);

				this->head1.Update(tickTotal, tickDelta, timeDelta, this->windowBounds, this->platform1, this->platform2, this->platform3, this->platform4, this->platform5, this->platform6, this->ladder, this->plate1, this->plate2, this->plate3);
				this->head2.Update(tickTotal, tickDelta, timeDelta, this->windowBounds, this->platform1, this->platform2, this->platform3, this->platform4, this->platform5, this->platform6, this->ladder, this->plate1, this->plate2, this->plate3);
				this->head3.Update(tickTotal, tickDelta, timeDelta, this->windowBounds, this->platform1, this->platform2, this->platform3, this->platform4, this->platform5, this->platform6, this->ladder, this->plate1, this->plate2, this->plate3);



				if(!this->gamepad.IsButtonDown(XINPUT_GAMEPAD_X)   
					&& oldGamepad.IsButtonDown(XINPUT_GAMEPAD_X))
				{
					this->levelState = LevelState::LevelEight;
					this->LevelReset();
					this->LoadLevel8();
				} 
				if (this->hero.NextLevel())
				{
					this->levelState = LevelState::LevelNine;
					this->LevelReset();
					this->LoadLevel9();
				}
				if (this->hero.HeroDead())
				{
					this->LevelReset();
					this->LoadLevel8();
				}

				break;

			case LevelState::LevelNine:
				//Put sprite elements into the game
				this->hero.Update(tickTotal, tickDelta, timeDelta, this->windowBounds, this->platform1, this->platform2, this->platform3, this->platform4, this->platform5, this->platform6, this->door, this->ladder, this->plate1, this->plate2, this->plate3, this->heart_Asset, this->painKiller_Asset, this->chainLift);
				this->enemy1.Update(tickTotal, tickDelta, timeDelta, this->windowBounds, this->platform1, this->platform2, this->platform3, this->platform4, this->platform5, this->platform6, this->ladder, this->plate1, this->plate2, this->plate3);
				this->enemy2.Update(tickTotal, tickDelta, timeDelta, this->windowBounds, this->platform1, this->platform2, this->platform3, this->platform4, this->platform5, this->platform6, this->ladder, this->plate1, this->plate2, this->plate3);
				this->enemy3.Update(tickTotal, tickDelta, timeDelta, this->windowBounds, this->platform1, this->platform2, this->platform3, this->platform4, this->platform5, this->platform6, this->ladder, this->plate1, this->plate2, this->plate3);

				this->head1.Update(tickTotal, tickDelta, timeDelta, this->windowBounds, this->platform1, this->platform2, this->platform3, this->platform4, this->platform5, this->platform6, this->ladder, this->plate1, this->plate2, this->plate3);
				this->head2.Update(tickTotal, tickDelta, timeDelta, this->windowBounds, this->platform1, this->platform2, this->platform3, this->platform4, this->platform5, this->platform6, this->ladder, this->plate1, this->plate2, this->plate3);
				this->head3.Update(tickTotal, tickDelta, timeDelta, this->windowBounds, this->platform1, this->platform2, this->platform3, this->platform4, this->platform5, this->platform6, this->ladder, this->plate1, this->plate2, this->plate3);



				if(!this->gamepad.IsButtonDown(XINPUT_GAMEPAD_X)   
					&& oldGamepad.IsButtonDown(XINPUT_GAMEPAD_X))
				{
					this->levelState = LevelState::LevelNine;
					this->LevelReset();
					this->LoadLevel9();
				} 
				if (this->hero.NextLevel())
				{
					this->levelState = LevelState::LevelTen;
					this->LevelReset();
					this->LoadLevel10();
				}
				if (this->hero.HeroDead())
				{
					this->LevelReset();
					this->LoadLevel9();
				}

				break;

			case LevelState::LevelTen:
				//Put sprite elements into the game
				this->hero.Update(tickTotal, tickDelta, timeDelta, this->windowBounds, this->platform1, this->platform2, this->platform3, this->platform4, this->platform5, this->platform6, this->door, this->ladder, this->plate1, this->plate2, this->plate3, this->heart_Asset, this->painKiller_Asset, this->chainLift);
				this->enemy1.Update(tickTotal, tickDelta, timeDelta, this->windowBounds, this->platform1, this->platform2, this->platform3, this->platform4, this->platform5, this->platform6, this->ladder, this->plate1, this->plate2, this->plate3);
				this->enemy2.Update(tickTotal, tickDelta, timeDelta, this->windowBounds, this->platform1, this->platform2, this->platform3, this->platform4, this->platform5, this->platform6, this->ladder, this->plate1, this->plate2, this->plate3);
				this->enemy3.Update(tickTotal, tickDelta, timeDelta, this->windowBounds, this->platform1, this->platform2, this->platform3, this->platform4, this->platform5, this->platform6, this->ladder, this->plate1, this->plate2, this->plate3);

				this->head1.Update(tickTotal, tickDelta, timeDelta, this->windowBounds, this->platform1, this->platform2, this->platform3, this->platform4, this->platform5, this->platform6, this->ladder, this->plate1, this->plate2, this->plate3);
				this->head2.Update(tickTotal, tickDelta, timeDelta, this->windowBounds, this->platform1, this->platform2, this->platform3, this->platform4, this->platform5, this->platform6, this->ladder, this->plate1, this->plate2, this->plate3);
				this->head3.Update(tickTotal, tickDelta, timeDelta, this->windowBounds, this->platform1, this->platform2, this->platform3, this->platform4, this->platform5, this->platform6, this->ladder, this->plate1, this->plate2, this->plate3);


				if(!this->gamepad.IsButtonDown(XINPUT_GAMEPAD_X)   
					&& oldGamepad.IsButtonDown(XINPUT_GAMEPAD_X))
				{
					this->levelState = LevelState::LevelTen;
					this->LevelReset();
					this->LoadLevel10();
				} 
				if (this->hero.NextLevel())
				{
					this->levelState = LevelState::LevelOne;
					this->LevelReset();
					this->LoadLevel1();
				}
				if (this->hero.HeroDead())
				{
					this->LevelReset();
					this->LoadLevel10();
				}

				break;
			}

		case GameState::GameOver:
			//Do game over updates
			if ( this->gamepad.IsButtonDown(XINPUT_GAMEPAD_START ) )
			{
				//First menu option is "Press [Start] to play"
				this->gameOverMenuOptionColour[0] = this->menuOptionColourSelected;
			}
			else
			{
				this->gameOverMenuOptionColour[0] = this->menuOptionColourDefault;
			}
			if(!this->gamepad.IsButtonDown(XINPUT_GAMEPAD_START)   
				&& oldGamepad.IsButtonDown(XINPUT_GAMEPAD_START))
			{
				this->state = GameState::MainMenu;
			}      
			break;
		case GameState::GamePaused:
			//Do game paused updates
			if ( this->gamepad.IsButtonDown(XINPUT_GAMEPAD_START ) )
			{
				//First menu option is "Press [Start] to play"
				this->pauseMenuOptionColour[0] = this->menuOptionColourSelected;
				this->pauseMenuOptionColour[1] = this->menuOptionColourDefault;
			}
			else if ( this->gamepad.IsButtonDown(XINPUT_GAMEPAD_BACK ) )
			{
				//Second menu option is "Press [Back] to quit
				this->pauseMenuOptionColour[0] = this->menuOptionColourDefault;
				this->pauseMenuOptionColour[1] = this->menuOptionColourSelected;
				this->levelState = LevelState::LevelOne;
				this->LevelReset();
				this->LoadLevel1();
			}
			else
			{
				this->pauseMenuOptionColour[0] = this->menuOptionColourDefault;
				this->pauseMenuOptionColour[1] = this->menuOptionColourDefault;
			}
			if(!this->gamepad.IsButtonDown(XINPUT_GAMEPAD_START)   
				&& oldGamepad.IsButtonDown(XINPUT_GAMEPAD_START))
			{
				this->state = GameState::PlayingGame;
			}      
			if(!this->gamepad.IsButtonDown(XINPUT_GAMEPAD_BACK)   
				&& oldGamepad.IsButtonDown(XINPUT_GAMEPAD_BACK))
			{
				this->state = GameState::GameOver;
			}      
			break;
		}
	}

}

void SpriteGame::Render()
{
	//Clear the back buffer of everything you drew to it last time
	this->d3dDeviceContext->ClearRenderTargetView(
		this->backBufferTarget.Get(),
		DirectX::Colors::Black
		);

	//Set the back buffer as our current target for rendering to
	this->d3dDeviceContext->OMSetRenderTargets(
		1,
		this->backBufferTarget.GetAddressOf(),
		nullptr
		);

	//Render your sprites to the backbuffer
	XMMATRIX cameraMatrix = Matrix::CreateTranslation(0, 0, 0);
	cameraMatrix *=
		Matrix::CreateTranslation(0,0,0) * 
		Matrix::CreateScale(this->scaleX, this->scaleY, 1.0);
	this->spriteBatch->Begin(SpriteSortMode_Deferred, commonStates->NonPremultiplied(), nullptr, nullptr, nullptr, nullptr, cameraMatrix);

	switch (state)
	{
	case GameState::MainMenu:
		//Draw main menu text
		this->spriteFont->DrawString(
			this->spriteBatch.get(), L"Headache", 
			Vector2((windowBounds.Width/2 - 500), 100), DirectX::Colors::White, 0.0f, SimpleMath::Vector2(0.0f, 0.0f), 5.0f, SpriteEffects_None, 0.0f
			);
		this->spriteFont->DrawString(
			this->spriteBatch.get(), L"Press [Start] to play", 
			Vector2((windowBounds.Width/2 - 250), 700), this->mainMenuOptionColour[0]
		);
		this->spriteFont->DrawString(
			this->spriteBatch.get(), L"Press [Back] to quit", 
			Vector2((windowBounds.Width/2 - 250), 800), this->mainMenuOptionColour[1]
		);
		break;
	case GameState::PlayingGame:

		switch (levelState)
		{

		case LevelState::LevelOne:
			//Level Sprites
			this->DrawLevel1();

			break;

		case LevelState::LevelTwo:
			this->DrawLevel2();

			break;

		case LevelState::LevelThree:
			this->DrawLevel3();

			break;
		case LevelState::LevelFour:
			this->DrawLevel4();

			break;
		case LevelState::LevelFive:
			this->DrawLevel5();

			break;
		case LevelState::LevelSix:
			this->DrawLevel6();

			break;
		case LevelState::LevelSeven:
			this->DrawLevel7();

			break;
		case LevelState::LevelEight:
			this->DrawLevel8();

			break;
		case LevelState::LevelNine:
			this->DrawLevel9();

			break;
		case LevelState::LevelTen:
			this->DrawLevel10();

			break;
		}
		break;
	case GameState::GameOver:
		//Draw the game over text
		this->spriteFont->DrawString(
			this->spriteBatch.get(), L"GAME OVER", 
			Vector2(10, 10), DirectX::Colors::AntiqueWhite
			);
		this->spriteFont->DrawString(
			this->spriteBatch.get(), L"Press [Start] to play again", 
			Vector2(10, 60), this->gameOverMenuOptionColour[0]
		);
		break;
	case GameState::GamePaused:
		//Draw the game paused text
		this->spriteFont->DrawString(
			this->spriteBatch.get(), L"GAME PAUSED", 
			Vector2(10, 10), DirectX::Colors::AntiqueWhite
			);
		this->spriteFont->DrawString(
			this->spriteBatch.get(), L"Press [Start] to resume play", 
			Vector2(10, 60), this->pauseMenuOptionColour[0]
		);
		this->spriteFont->DrawString(
			this->spriteBatch.get(), L"Press [Back] to quit", 
			Vector2(10, 110), this->pauseMenuOptionColour[1]
		);
		break;
	default:
		//Unexpected GameState - show error message
		this->spriteFont->DrawString(
			this->spriteBatch.get(), 
			L"The game encountered an unexpected GameState value.  \nPlease contact the developer.", 
			Vector2(10, 200), DirectX::Colors::Red
			);
	}

	if(!this->gamepad.IsConnected())
	{
		//Draw the "No device connected" overlay
		this->spriteFont->DrawString(
			this->spriteBatch.get(), L"Please connect an Xbox gamepad", 
			Vector2(10, 300), DirectX::Colors::Red
			);
	}

	this->spriteBatch->End();

}

void SpriteGame::Present()
{
	DXGI_PRESENT_PARAMETERS parameters = {0};
	parameters.DirtyRectsCount = 0;
	parameters.pDirtyRects = nullptr;
	parameters.pScrollRect = nullptr;
	parameters.pScrollOffset = nullptr;

	HRESULT hr = this->swapChain->Present1(1, 0, &parameters);

	this->d3dDeviceContext->DiscardView(this->backBufferTarget.Get());

	ThrowIfFailed(hr);
}

float SpriteGame::ConvertDipsToPixels(float dips)
{
	static const float dipsPerLogicalInch = 96.0f;
	return floor(dips * DisplayProperties::LogicalDpi / dipsPerLogicalInch  + 0.5f); // Round to nearest integer.
}

void SpriteGame::LevelReset()
{
	//Reset Platforms

	this->platform1.SetBoundingBox(Windows::Foundation::Rect (0.0f, 1080.0f, 0.0f, 0.0f));
	this->platform2.SetBoundingBox(Windows::Foundation::Rect (0.0f, 1080.0f, 0.0f, 0.0f));
	this->platform3.SetBoundingBox(Windows::Foundation::Rect (0.0f, 1080.0f, 0.0f, 0.0f));
	this->platform4.SetBoundingBox(Windows::Foundation::Rect (0.0f, 1080.0f, 0.0f, 0.0f));
	this->platform5.SetBoundingBox(Windows::Foundation::Rect (0.0f, 1080.0f, 0.0f, 0.0f));
	this->platform6.SetBoundingBox(Windows::Foundation::Rect (0.0f, 1080.0f, 0.0f, 0.0f));
	this->chainLift.SetBoundingBox(Windows::Foundation::Rect (0.0f, 1080.0f, 0.0f, 0.0f));
	this->door.SetBoundingBox(Windows::Foundation::Rect (0.0f, 1080.0f, 0.0f, 0.0f));
	this->hero.LockDoor(false);
	this->ladder.SetBoundingBox(Windows::Foundation::Rect (0.0f, 1080.0f, 0.0f, 0.0f));
	this->ladder.Initialize(L"Assets/BlankPlatform.png", this->d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);

	heart_Asset.SetVisiblity(true);
	painKiller_Asset.SetVisiblity(true);

	//Set pressure plates coded by Brandon,
	this->plate1.SetBoundingBox(Windows::Foundation::Rect(0.0f, 1080.0f, 0.0f, 0.0f));
	this->plate1.Initialize(L"Assets/BlankPlatform.png", this->d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
	this->plate1.Reset(Vector2(160.0f,585.0f));


	this->plate2.SetBoundingBox(Windows::Foundation::Rect(0.0f, 1080.0f, 0.0f, 0.0f));
	this->plate2.Initialize(L"Assets/BlankPlatform.png", this->d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
	this->plate2.Reset(Vector2(500.0f, 585.0f));

	this->plate3.SetBoundingBox(Windows::Foundation::Rect(0.0f, 1080.0f, 0.0f, 0.0f));
	this->plate3.Initialize(L"Assets/BlankPlatform.png", this->d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
	this->plate3.Reset(Vector2(819.0f, 975.0f));

	this->chainLift.SetBoundingBox(Windows::Foundation::Rect (0.0f, 1080.0f, 0.0f, 0.0f));
	this->chainLift.Reset(Vector2 (0.0f, 1080.0f));
	this->chainLift.ResetLift();
	this->steppedOnPlate = false;

	//Set Hero
	this->hero.Reset(Vector2( 0.0f, 0.0f), false, false);

	//Set Enemies
	this->enemy1.Initialize(L"Assets/BlankPlatform.png", this->d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
	this->enemy2.Initialize(L"Assets/BlankPlatform.png", this->d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
	this->enemy3.Initialize(L"Assets/BlankPlatform.png", this->d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
	this->head1.Initialize(L"Assets/BlankPlatform.png", this->d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
	this->head2.Initialize(L"Assets/BlankPlatform.png", this->d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
	this->head3.Initialize(L"Assets/BlankPlatform.png", this->d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
	this->enemy1.Reset(Vector2( 0.0f - 50.0f, 0.0f - 50.0f ), false, false, 6, false, 0);
	this->enemy2.Reset(Vector2( 0.0f - 50.0f, 0.0f - 50.0f ), false, false, 6, false, 0);
	this->enemy3.Reset(Vector2( 0.0f - 50.0f, 0.0f - 50.0f ), false, false, 6, false, 0);
	this->enemy1.ClearUnusedEnemies();
	this->enemy2.ClearUnusedEnemies();
	this->enemy3.ClearUnusedEnemies();
	this->enemyQuantity = 0;
	//set heart bounding box
	this->heart_Asset.SetBoundingBox(Windows::Foundation::Rect (0.0f, 1080.0f, 0.0f, 0.0f));
	//set Painkiller boundingbox
	this->painKiller_Asset.SetBoundingBox(Windows::Foundation::Rect (0.0f, 1080.0f, 0.0f, 0.0f));
}

void SpriteGame::LoadLevel1()
{
	this->killedEnemy = false;
	this->highlightEnemy = false;
	this->currentLevel = 1;
	//Set Background
	this->background.Initialize(L"Assets/Level1Background.png", this->d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);


	//Set Platforms
	this->platform1.SetBoundingBox(Windows::Foundation::Rect (0.0f, 936.0f, 1326.0f, 144.0f));
	this->platform2.SetBoundingBox(Windows::Foundation::Rect (1267.0f, 449.0f, windowBounds.Right, 58.0f));
	this->door.SetBoundingBox(Windows::Foundation::Rect (1860.0f, 315.0f, windowBounds.Right, 69.0f));
	this->door.Initialize(L"Assets/ClosedDoor.png", this->d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
	this->door.Reset(Vector2(1901.0f, 265.0f));
	//Set Hero
	this->hero.Initialize(L"Assets/HeroJumpIdle.png", this->d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
	this->hero.Reset(Vector2( 0.0f, this->windowBounds.Height - 177.0f - 144.0f - 120.0f), false, false);

	//Set Enemies
	this->enemy1.Initialize(L"Assets/bugwalksprite.png", this->d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
	this->enemy1.Reset(Vector2( (windowBounds.Right - 200.0f), (449.0f - 166.0f)), false, false, 6, false, 1);
	this->enemy1.SetLevel(1);
	this->enemy2.SetLevel(1);
	this->enemy3.SetLevel(1);
	this->head1.Initialize(L"Assets/BlankPlatform.png", this->d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);

	/*this->ladder.Initialize(L"Assets/ladder2.png", this->d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);*/

	this->enemyQuantity = 1;
	this->enemySelected = 0;

	this->plate1.SetBoundingBox(Windows::Foundation::Rect(1325.0f, 415.0f, 207.0f, 34.0f));
	this->plate1.Initialize(L"Assets/pressurepadup.png", this->d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
	this->plate1.Reset(Vector2(1325.0f,415.0f));

	this->chainLift.SetBoundingBox(Windows::Foundation::Rect (908.0f, 460.0f, 210.0f, 25.0f));
	this->chainLift.Reset(Vector2 (908.0f, -415.0f));
	this->chainLift.ResetLift();
	this->chainLift.Initialize(L"Assets/ChainLift.png", this->d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);

	//****************************************************************************

	this->hud.Initialize(L"Assets/Hud_01.png", this-> d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);

	this->hud.Reset(Vector2(0.0f, windowBounds.Top));

	this->hearts.Initialize(L"Assets/Heart_Asset.png", this-> d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
	this->hearts.Reset(Vector2(350.0, windowBounds.Top + 15.0f));

	this->painKiller1.Initialize(L"Assets/PainKiller_Asset.png", this-> d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
	this->painKiller1.Reset(Vector2(920.0, windowBounds.Top + 15.0f));

	this->painKiller2.Initialize(L"Assets/PainKiller_Asset.png", this-> d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
	this->painKiller2.Reset(Vector2(982.0, windowBounds.Top + 15.0f));

	this->painKiller3.Initialize(L"Assets/PainKiller_Asset.png", this-> d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
	this->painKiller3.Reset(Vector2(1044.0, windowBounds.Top + 15.0f));

	this->spriteEnemiesLeft.Initialize(L"Assets/SpriteEnemiesLeft.png", this-> d3dDevice, this->d3dDeviceContext);
	this->spriteEnemiesLeft.Reset(Vector2(1510.0, windowBounds.Top + 20.0f));

	this->heart1.Initialize(L"Assets/Heart_Asset.png", this-> d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
	this->heart1.Reset(Vector2(420.0, windowBounds.Top + 20.0f));

	this->heart2.Initialize(L"Assets/Heart_Asset.png", this-> d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
	this->heart2.Reset(Vector2(486.0, windowBounds.Top + 20.0f));

	this->heart3.Initialize(L"Assets/Heart_Asset.png", this-> d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
	this->heart3.Reset(Vector2(552.0, windowBounds.Top + 20.0f));

	this->heart_Asset.Initialize(L"Assets/Heart_Asset.png", this-> d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
	this->heart_Asset.Reset(Vector2(1700.0f, 360.0f));
	this->heart_Asset.SetBoundingBox(Windows::Foundation::Rect (0.0f, 0.0f, 48.0f, 28.0f));

	this->painKiller_Asset.Initialize(L"Assets/PainKiller_Asset.png", this-> d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
	this->painKiller_Asset.Reset(Vector2(950.0f, 848.0f));
	this->painKiller_Asset.SetBoundingBox(Windows::Foundation::Rect (0.0f, 0.0f, 59.0f, 91.0f));

	//************************************************************************************** Jake Evans


}
void SpriteGame::DrawLevel1()
{

	//Level Sprites
	this->background.Draw();
	/*this->ladder.Draw();*/
	this->plate1.Draw();
	this->chainLift.Draw();
	//*************************************************************************************
	this->hud.Draw();
	if (this->lives == 3 )
	{
		heart1.Draw();
		heart2.Draw();
		heart3.Draw();
	}
	else if (lives == 2)
	{
		heart1.Draw();
		heart2.Draw();
	}
	else if (lives == 1)
	{
		heart1.Draw();
	}

	if (this->painKillers == 3 )
	{
		painKiller1.Draw();
		painKiller2.Draw();
		painKiller3.Draw();
	}
	else if (painKillers == 2)
	{
		painKiller1.Draw();
		painKiller2.Draw();
	}
	else if (painKillers == 1)
	{
		painKiller1.Draw();
	}
	
	//************************************************************************************ Jake Evans

	//Interactive Sprites

	this->heart_Asset.Draw();
	this->painKiller_Asset.Draw();
	this->hero.Draw();
	this->enemy1.Draw();
	this->head1.Draw();
	this->door.Draw();
}


void SpriteGame::LoadLevel2()
{
	this->killedEnemy = false;
	this->highlightEnemy = false;
	this->currentLevel = 2;
	//Set Background
	this->background.Initialize(L"Assets/Level2Background.png", this->d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
	this->door.Initialize(L"Assets/ClosedDoor.png", this->d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);

	//Set Platforms
	this->platform1.SetBoundingBox(Windows::Foundation::Rect (0.0f, 701.0f, 308.0f, 38.0f));
	this->platform2.SetBoundingBox(Windows::Foundation::Rect (801.0f, 701.0f, 324.0f, 46.0f));
	this->platform3.SetBoundingBox(Windows::Foundation::Rect (1603.0f, 701.0f, 308.0f, 38.0f));
	this->platform4.SetBoundingBox(Windows::Foundation::Rect (0.0f, 970.0f, 394.0f, 110.0f));
	this->platform5.SetBoundingBox(Windows::Foundation::Rect (653.0f, 970.0f, 1264.0f, 110.0f));

	this->chainLift.SetBoundingBox(Windows::Foundation::Rect (365.0f, 460.0f, 210.0f, 25.0f));
	this->chainLift.Reset(Vector2 (365.0f, -415.0f));
	this->chainLift.ResetLift();
	this->chainLift.Initialize(L"Assets/ChainLift.png", this->d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);


	this->door.SetBoundingBox(Windows::Foundation::Rect (1860.0f, 840.0f, windowBounds.Right, 69.0f));
	this->door.Reset(Vector2(1901.0f, 790.0f));
	//Set Hero
	this->hero.Initialize(L"Assets/HeroJumpIdle.png", this->d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
	this->hero.Reset(Vector2( windowBounds.Left, (970.0f - 177.0f)), false, false);

	this->plate1.SetBoundingBox(Windows::Foundation::Rect(1683.0f,666.0f, 207.0f, 34.0f));
	this->plate1.Initialize(L"Assets/pressurepadup.png", this->d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
	this->plate1.Reset(Vector2(1683.0f,666.0f));
	//Set Enemies 
	this->enemy1.Initialize(L"Assets/bugwalksprite.png", this->d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY); 
	this->enemy1.Reset(Vector2( (windowBounds.Right - 200.0f), (970.0f - 166.0f)), false, false, 6, false,1); 
	this->enemy1.SetLevel(2); 
	this->enemy2.Initialize(L"Assets/bugwalksprite.png", this->d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY); 
	this->enemy2.Reset(Vector2(1016.0f, 546.0f), false, false, 6, false,2); 
	this->enemy2.SetLevel(2); 
	this->enemy3.SetLevel(2); 
	this->head1.Initialize(L"Assets/BlankPlatform.png", this->d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY); 
	this->head2.Initialize(L"Assets/BlankPlatform.png", this->d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY); 

	this->enemyQuantity = 2; 
	this->enemySelected = 0;
	//****************************************************************************

	this->hud.Initialize(L"Assets/Hud_01.png", this-> d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);

	this->hud.Reset(Vector2(0.0f, windowBounds.Top));

	this->hearts.Initialize(L"Assets/Heart_Asset.png", this-> d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
	this->hearts.Reset(Vector2(350.0, windowBounds.Top + 15.0f));

	this->painKiller1.Initialize(L"Assets/PainKiller_Asset.png", this-> d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
	this->painKiller1.Reset(Vector2(920.0, windowBounds.Top + 15.0f));

	this->painKiller2.Initialize(L"Assets/PainKiller_Asset.png", this-> d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
	this->painKiller2.Reset(Vector2(982.0, windowBounds.Top + 15.0f));

	this->painKiller3.Initialize(L"Assets/PainKiller_Asset.png", this-> d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
	this->painKiller3.Reset(Vector2(1044.0, windowBounds.Top + 15.0f));

	this->spriteEnemiesLeft.Initialize(L"Assets/SpriteEnemiesLeft.png", this-> d3dDevice, this->d3dDeviceContext);
	this->spriteEnemiesLeft.Reset(Vector2(1510.0, windowBounds.Top + 20.0f));

	this->heart1.Initialize(L"Assets/Heart_Asset.png", this-> d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
	this->heart1.Reset(Vector2(420.0, windowBounds.Top + 20.0f));

	this->heart2.Initialize(L"Assets/Heart_Asset.png", this-> d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
	this->heart2.Reset(Vector2(486.0, windowBounds.Top + 20.0f));

	this->heart3.Initialize(L"Assets/Heart_Asset.png", this-> d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
	this->heart3.Reset(Vector2(552.0, windowBounds.Top + 20.0f));

	this->heart_Asset.Initialize(L"Assets/Heart_Asset.png", this-> d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
	this->heart_Asset.Reset(Vector2(1800.0,  880.0f));
	this->heart_Asset.SetBoundingBox(Windows::Foundation::Rect (0.0f, 0.0f, 48.0f, 28.0f));


	this->painKiller_Asset.Initialize(L"Assets/PainKiller_Asset.png", this-> d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
	this->painKiller_Asset.Reset(Vector2(0.0f, 610.0f));
	this->painKiller_Asset.SetBoundingBox(Windows::Foundation::Rect (0.0f, 0.0f, 59.0f, 91.0f));

	//************************************************************************************** Jake Evans
}
void SpriteGame::DrawLevel2()
{
	//Level Sprites
	this->background.Draw();
	this->chainLift.Draw();
	//*************************************************************************************
	this->hud.Draw();
	if (this->lives == 3 )
	{
		heart1.Draw();
		heart2.Draw();
		heart3.Draw();
	}
	else if (lives == 2)
	{
		heart1.Draw();
		heart2.Draw();
	}
	else if (lives == 1)
	{
		heart1.Draw();
	}

	if (this->painKillers == 3 )
	{
		painKiller1.Draw();
		painKiller2.Draw();
		painKiller3.Draw();
	}
	else if (painKillers == 2)
	{
		painKiller1.Draw();
		painKiller2.Draw();
	}
	else if (painKillers == 1)
	{
		painKiller1.Draw();
	}
	//************************************************************************************ Jake Evans

	//Interactive Sprites

	this->heart_Asset.Draw();
	this->painKiller_Asset.Draw();
	this->hero.Draw();
	this->enemy1.Draw();
	this->head1.Draw();
	this->enemy2.Draw();
	this->head2.Draw();
	this->door.Draw();
	this->plate1.Draw();
}


void SpriteGame::LoadLevel3()
{
	this->killedEnemy = false;
	this->highlightEnemy = false;
	this->currentLevel = 3;
	//Set Background
	this->background.Initialize(L"Assets/Level3Background.png", this->d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);

	//Set Platforms
	this->platform1.SetBoundingBox(Windows::Foundation::Rect (0.0f, 285.0f, 857.0f, 73.0f));
	this->platform2.SetBoundingBox(Windows::Foundation::Rect (0.0f, 618.0f, 851.0f, 75.0f));
	this->platform3.SetBoundingBox(Windows::Foundation::Rect (1060.0f, 618.0f, 855.0f, 73.0f));
	this->platform4.SetBoundingBox(Windows::Foundation::Rect (0.0f, 1011.0f, 1370.0f, 69.0f));
	this->platform5.SetBoundingBox(Windows::Foundation::Rect (1485.0f, 1011.0f, 432.0f, 69.0f));

	this->door.Initialize(L"Assets/ClosedDoor.png", this->d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
	this->door.SetBoundingBox(Windows::Foundation::Rect (1901.0f, 830.0f, 19.0f, 180.0f));
	this->door.Reset(Vector2(1901.0f, 830.0f));
	this->hero.LockDoor(true);

	//Set pressure plates coded by Brandon,
	this->plate1.SetBoundingBox(Windows::Foundation::Rect(160.0f, 585.0f, 207.0f, 34.0f));
	this->plate1.Initialize(L"Assets/pressurepadup.png", this->d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
	this->plate1.Reset(Vector2(160.0f,585.0f));


	this->plate2.SetBoundingBox(Windows::Foundation::Rect(500.0f, 585.0f, 207.0f, 34.0f));
	this->plate2.Initialize(L"Assets/pressurepadup.png", this->d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
	this->plate2.Reset(Vector2(500.0f, 585.0f));

	this->plate3.SetBoundingBox(Windows::Foundation::Rect(819.0f, 975.0f, 207.0f, 34.0f));
	this->plate3.Initialize(L"Assets/pressurepadup.png", this->d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
	this->plate3.Reset(Vector2(819.0f, 975.0f));


	//Set Hero
	this->hero.Initialize(L"Assets/HeroJumpIdle.png", this->d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
	this->hero.Reset(Vector2( 0.0f, (285.0f - 177)), false, false);

	//Set Enemies 
	this->enemy1.Initialize(L"Assets/bugwalksprite.png", this->d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY); 
	this->enemy1.Reset(Vector2(258.0f, 838.0f), false, false, 6, false, 1); 
	this->enemy2.Initialize(L"Assets/bugwalksprite.png", this->d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY); 
	this->enemy2.Reset(Vector2(375.0f, 474.0f), false, false, 6, false, 2); 


	this->enemy1.SetLevel(3); 
	this->enemy2.SetLevel(3); 
	this->enemy3.SetLevel(3); 
	this->head1.Initialize(L"Assets/BlankPlatform.png", this->d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY); 
	this->head2.Initialize(L"Assets/BlankPlatform.png", this->d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY); 


	this->enemyQuantity = 2; 
	this->enemySelected = 0;

	//****************************************************************************

	this->hud.Initialize(L"Assets/Hud_01.png", this-> d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);

	this->hud.Reset(Vector2(0.0f, windowBounds.Top));

	this->hearts.Initialize(L"Assets/Heart_Asset.png", this-> d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
	this->hearts.Reset(Vector2(350.0, windowBounds.Top + 15.0f));

	this->painKiller1.Initialize(L"Assets/PainKiller_Asset.png", this-> d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
	this->painKiller1.Reset(Vector2(920.0, windowBounds.Top + 15.0f));

	this->painKiller2.Initialize(L"Assets/PainKiller_Asset.png", this-> d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
	this->painKiller2.Reset(Vector2(982.0, windowBounds.Top + 15.0f));

	this->painKiller3.Initialize(L"Assets/PainKiller_Asset.png", this-> d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
	this->painKiller3.Reset(Vector2(1044.0, windowBounds.Top + 15.0f));

	this->spriteEnemiesLeft.Initialize(L"Assets/SpriteEnemiesLeft.png", this-> d3dDevice, this->d3dDeviceContext);
	this->spriteEnemiesLeft.Reset(Vector2(1510.0, windowBounds.Top + 20.0f));

	this->heart1.Initialize(L"Assets/Heart_Asset.png", this-> d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
	this->heart1.Reset(Vector2(420.0, windowBounds.Top + 20.0f));

	this->heart2.Initialize(L"Assets/Heart_Asset.png", this-> d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
	this->heart2.Reset(Vector2(486.0, windowBounds.Top + 20.0f));

	this->heart3.Initialize(L"Assets/Heart_Asset.png", this-> d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
	this->heart3.Reset(Vector2(552.0, windowBounds.Top + 20.0f));

	this->heart_Asset.Initialize(L"Assets/Heart_Asset.png", this-> d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
	this->heart_Asset.Reset(Vector2(1300.0, 920.0f));
	this->heart_Asset.SetBoundingBox(Windows::Foundation::Rect (0.0f, 0.0f, 48.0f, 28.0f));


	this->painKiller_Asset.Initialize(L"Assets/PainKiller_Asset.png", this-> d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
	this->painKiller_Asset.Reset(Vector2(1700.0f, 525.0f));
	this->painKiller_Asset.SetBoundingBox(Windows::Foundation::Rect (0.0f, 0.0f, 59.0f, 91.0f));

	//************************************************************************************** Jake Evans
}
void SpriteGame::DrawLevel3()
{
	//Level Sprites
	this->background.Draw();
	this->plate1.Draw();
	this->plate2.Draw();
	this->plate3.Draw();

	//*************************************************************************************
	this->hud.Draw();
	if (this->lives == 3 )
	{
		heart1.Draw();
		heart2.Draw();
		heart3.Draw();
	}
	else if (lives == 2)
	{
		heart1.Draw();
		heart2.Draw();
	}
	else if (lives == 1)
	{
		heart1.Draw();
	}

	if (this->painKillers == 3 )
	{
		painKiller1.Draw();
		painKiller2.Draw();
		painKiller3.Draw();
	}
	else if (painKillers == 2)
	{
		painKiller1.Draw();
		painKiller2.Draw();
	}
	else if (painKillers == 1)
	{
		painKiller1.Draw();
	}
	//************************************************************************************ Jake Evans

	//Interactive Sprites

	this->door.Draw();
	this->heart_Asset.Draw();
	this->painKiller_Asset.Draw();
	this->hero.Draw();
	this->enemy1.Draw();
	this->enemy2.Draw();
	this->head1.Draw();
	this->head2.Draw();
}

void SpriteGame::LoadLevel4()
{
	this->killedEnemy = false;
	this->highlightEnemy = false;
	this->currentLevel = 4;

	//Set Background
	this->background.Initialize(L"Assets/Level4Background.png", this->d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
	this->door.Initialize(L"Assets/ClosedDoor.png", this->d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);

	//Set Platforms
	this->platform1.SetBoundingBox(Windows::Foundation::Rect (0.0f, 568.0f, 907.0f, 74.0f));
	this->platform2.SetBoundingBox(Windows::Foundation::Rect (1098.0f, 568.0f, 882.0f, 76.0f));
	this->platform3.SetBoundingBox(Windows::Foundation::Rect (0.0f, 1007.0f, 342.0f, 32.0f));
	this->platform4.SetBoundingBox(Windows::Foundation::Rect (563.0f, 1007.0f, 853.0f, 74.0f));
	this->platform5.SetBoundingBox(Windows::Foundation::Rect (1640.0f, 1007.0f, 340.0f, 32.0f));

	this->door.SetBoundingBox(Windows::Foundation::Rect (1860.0f, 840.0f, windowBounds.Right, 69.0f));
	//this->door.Initialize(L"Assets/OpenDoor.png", this->d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
	this->door.Reset(Vector2(1901.0f, 820.0f));
	//Set Hero
	this->hero.Initialize(L"Assets/HeroJumpIdle.png", this->d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
	this->hero.Reset(Vector2( 0.0f, (568.0f - 177)), false, false);

	//Set Enemies 
	this->enemy1.Initialize(L"Assets/bugwalksprite.png", this->d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY); 
	this->enemy1.Reset(Vector2(1425.0f, 400.0f), false, false, 6, false,1); 
	this->enemy1.SetLevel(4); 
	this->enemy2.Initialize(L"Assets/bugwalksprite.png", this->d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY); 
	this->enemy2.Reset(Vector2(1118.0f, 750.0f), false, false, 6, false,2); 
	this->enemy2.SetLevel(4); 
	this->enemy3.SetLevel(4); 
	this->head1.Initialize(L"Assets/BlankPlatform.png", this->d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY); 
	this->head2.Initialize(L"Assets/BlankPlatform.png", this->d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY); 
	this->enemyQuantity = 2; 
	this->enemySelected = 0; 

	//****************************************************************************

	this->hud.Initialize(L"Assets/Hud_01.png", this-> d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);

	this->hud.Reset(Vector2(0.0f, windowBounds.Top));

	this->hearts.Initialize(L"Assets/Heart_Asset.png", this-> d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
	this->hearts.Reset(Vector2(350.0, windowBounds.Top + 15.0f));

	this->painKiller1.Initialize(L"Assets/PainKiller_Asset.png", this-> d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
	this->painKiller1.Reset(Vector2(920.0, windowBounds.Top + 15.0f));

	this->painKiller2.Initialize(L"Assets/PainKiller_Asset.png", this-> d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
	this->painKiller2.Reset(Vector2(982.0, windowBounds.Top + 15.0f));

	this->painKiller3.Initialize(L"Assets/PainKiller_Asset.png", this-> d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
	this->painKiller3.Reset(Vector2(1044.0, windowBounds.Top + 15.0f));

	this->spriteEnemiesLeft.Initialize(L"Assets/SpriteEnemiesLeft.png", this-> d3dDevice, this->d3dDeviceContext);
	this->spriteEnemiesLeft.Reset(Vector2(1510.0, windowBounds.Top + 20.0f));

	this->heart1.Initialize(L"Assets/Heart_Asset.png", this-> d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
	this->heart1.Reset(Vector2(420.0, windowBounds.Top + 20.0f));

	this->heart2.Initialize(L"Assets/Heart_Asset.png", this-> d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
	this->heart2.Reset(Vector2(486.0, windowBounds.Top + 20.0f));

	this->heart3.Initialize(L"Assets/Heart_Asset.png", this-> d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
	this->heart3.Reset(Vector2(552.0, windowBounds.Top + 20.0f));

	this->heart_Asset.Initialize(L"Assets/Heart_Asset.png", this-> d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
	this->heart_Asset.Reset(Vector2(0.0, 920.0f));
	this->heart_Asset.SetBoundingBox(Windows::Foundation::Rect (0.0f, 0.0f, 48.0f, 28.0f));

	this->painKiller_Asset.Initialize(L"Assets/PainKiller_Asset.png", this-> d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
	this->painKiller_Asset.Reset(Vector2(800.0f, 480.0f));
	this->painKiller_Asset.SetBoundingBox(Windows::Foundation::Rect (0.0f, 0.0f, 59.0f, 91.0f));

	//************************************************************************************** Jake Evans
}
void SpriteGame::DrawLevel4()
{
	//Level Sprites
	this->background.Draw();

	//*************************************************************************************
	this->hud.Draw();
	if (this->lives == 3 )
	{
		heart1.Draw();
		heart2.Draw();
		heart3.Draw();
	}
	else if (lives == 2)
	{
		heart1.Draw();
		heart2.Draw();
	}
	else if (lives == 1)
	{
		heart1.Draw();
	}

	if (this->painKillers == 3 )
	{
		painKiller1.Draw();
		painKiller2.Draw();
		painKiller3.Draw();
	}
	else if (painKillers == 2)
	{
		painKiller1.Draw();
		painKiller2.Draw();
	}
	else if (painKillers == 1)
	{
		painKiller1.Draw();
	}
	//************************************************************************************ Jake Evans

	//Interactive Sprites

	this->door.Draw();
	this->heart_Asset.Draw();
	this->painKiller_Asset.Draw();
	this->hero.Draw();
	this->enemy1.Draw();
	this->head1.Draw();
	this->enemy2.Draw();
	this->head2.Draw();
}

void SpriteGame::LoadLevel5()
{
	this->killedEnemy = false;
	this->highlightEnemy = false;
	this->currentLevel = 5;
	//Set Background
	this->background.Initialize(L"Assets/Level5Background.png", this->d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
	this->door.Initialize(L"Assets/ClosedDoor.png", this->d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);

	//Set Platforms
	this->platform1.SetBoundingBox(Windows::Foundation::Rect (0.0f, 615.0f, 316.0f, 30.0f));
	this->platform2.SetBoundingBox(Windows::Foundation::Rect (531.0f, 617.0f, 317.0f, 30.0f));
	this->platform3.SetBoundingBox(Windows::Foundation::Rect (1086.0f, 617.0f, 317.0f, 30.0f));
	this->platform4.SetBoundingBox(Windows::Foundation::Rect (1640.0f, 614.0f, 280.0f, 30.0f));
	this->platform5.SetBoundingBox(Windows::Foundation::Rect (0.0f, 991.0f, windowBounds.Right, 89.0f));

	this->door.SetBoundingBox(Windows::Foundation::Rect (1860.0f, 500.0f, windowBounds.Right, 69.0f));
	this->door.Reset(Vector2(1901.0f, 440.0f));
	//Set Hero
	this->hero.Initialize(L"Assets/HeroJumpIdle.png", this->d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
	this->hero.Reset(Vector2( 0.0f, (615.0f - 177)), false, false);

	//Set Enemies 
	this->enemy1.Initialize(L"Assets/bugwalksprite.png", this->d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY); 
	this->enemy1.Reset(Vector2( 347.0f, 770.0f ), false, false, 6, false,1); 

	this->enemy2.Initialize(L"Assets/bugwalksprite.png", this->d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY); 
	this->enemy2.Reset(Vector2( 890.0f, 770.0f ), false, false, 6, false,2); 

	this->enemy3.Initialize(L"Assets/bugwalksprite.png", this->d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY); 
	this->enemy3.Reset(Vector2( 1435.0f, 770.0f ), false, false, 6, false,3); 
	this->enemy1.SetLevel(5); 
	this->enemy2.SetLevel(5); 
	this->enemy3.SetLevel(5); 
	this->head1.Initialize(L"Assets/BlankPlatform.png", this->d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY); 
	this->head2.Initialize(L"Assets/BlankPlatform.png", this->d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY); 
	this->head3.Initialize(L"Assets/BlankPlatform.png", this->d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY); 

	this->enemyQuantity = 3; 
	this->enemySelected = 0;

	//****************************************************************************

	this->hud.Initialize(L"Assets/Hud_01.png", this-> d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);

	this->hud.Reset(Vector2(0.0f, windowBounds.Top));

	this->hearts.Initialize(L"Assets/Heart_Asset.png", this-> d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
	this->hearts.Reset(Vector2(350.0, windowBounds.Top + 15.0f));

	this->painKiller1.Initialize(L"Assets/PainKiller_Asset.png", this-> d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
	this->painKiller1.Reset(Vector2(920.0, windowBounds.Top + 15.0f));

	this->painKiller2.Initialize(L"Assets/PainKiller_Asset.png", this-> d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
	this->painKiller2.Reset(Vector2(982.0, windowBounds.Top + 15.0f));

	this->painKiller3.Initialize(L"Assets/PainKiller_Asset.png", this-> d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
	this->painKiller3.Reset(Vector2(1044.0, windowBounds.Top + 15.0f));

	this->spriteEnemiesLeft.Initialize(L"Assets/SpriteEnemiesLeft.png", this-> d3dDevice, this->d3dDeviceContext);
	this->spriteEnemiesLeft.Reset(Vector2(1510.0, windowBounds.Top + 20.0f));

	this->heart1.Initialize(L"Assets/Heart_Asset.png", this-> d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
	this->heart1.Reset(Vector2(420.0, windowBounds.Top + 20.0f));

	this->heart2.Initialize(L"Assets/Heart_Asset.png", this-> d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
	this->heart2.Reset(Vector2(486.0, windowBounds.Top + 20.0f));

	this->heart3.Initialize(L"Assets/Heart_Asset.png", this-> d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
	this->heart3.Reset(Vector2(552.0, windowBounds.Top + 20.0f));

	this->heart_Asset.Initialize(L"Assets/Heart_Asset.png", this-> d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
	this->heart_Asset.Reset(Vector2(1800.0, 530.0f));
	this->heart_Asset.SetBoundingBox(Windows::Foundation::Rect (0.0f, 0.0f, 48.0f, 28.0f));

	this->painKiller_Asset.Initialize(L"Assets/PainKiller_Asset.png", this-> d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
	this->painKiller_Asset.Reset(Vector2(0.0f, 900.0f));
	this->painKiller_Asset.SetBoundingBox(Windows::Foundation::Rect (0.0f, 0.0f, 59.0f, 91.0f));

	//************************************************************************************** Jake Evans
}
void SpriteGame::DrawLevel5()
{
	//Level Sprites
	this->background.Draw();

	//*************************************************************************************
	this->hud.Draw();
	if (this->lives == 3 )
	{
		heart1.Draw();
		heart2.Draw();
		heart3.Draw();
	}
	else if (lives == 2)
	{
		heart1.Draw();
		heart2.Draw();
	}
	else if (lives == 1)
	{
		heart1.Draw();
	}

	if (this->painKillers == 3 )
	{
		painKiller1.Draw();
		painKiller2.Draw();
		painKiller3.Draw();
	}
	else if (painKillers == 2)
	{
		painKiller1.Draw();
		painKiller2.Draw();
	}
	else if (painKillers == 1)
	{
		painKiller1.Draw();
	}
	//************************************************************************************ Jake Evans

	//Interactive Sprites

	this->heart_Asset.Draw();
	this->painKiller_Asset.Draw();
	this->hero.Draw();
	this->enemy1.Draw();
	this->enemy2.Draw();
	this->enemy3.Draw();
	this->head1.Draw();
	this->head2.Draw();
	this->head3.Draw();
	this->door.Draw();
}

void SpriteGame::LoadLevel6()
{
	this->killedEnemy = false;
	this->highlightEnemy = false;

	this->currentLevel = 6;
	//Set Background
	this->background.Initialize(L"Assets/Level6Background.png", this->d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
	this->door.Initialize(L"Assets/ClosedDoor.png", this->d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);

	//Set Platforms
	this->platform1.SetBoundingBox(Windows::Foundation::Rect (0.0f, 635.0f, 634.0f, 28.0f));
	this->platform2.SetBoundingBox(Windows::Foundation::Rect (0.0f, 991.0f, windowBounds.Right, 89.0f));

	this->door.SetBoundingBox(Windows::Foundation::Rect (1860.0f, 870.0f, windowBounds.Right, 69.0f));
	this->door.Reset(Vector2(1901.0f, 820.0f));
	//Set Hero
	this->hero.Initialize(L"Assets/HeroJumpIdle.png", this->d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
	this->hero.Reset(Vector2( 0.0f, (635.0f - 177)), false, false);

	//Set Enemies 
	this->enemy1.Initialize(L"Assets/bugwalksprite.png", this->d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY); 
	this->enemy1.Reset(Vector2(485.0f, 800.0f), false, false, 6, false,1); 
	this->enemy2.Initialize(L"Assets/bugwalksprite.png", this->d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY); 
	this->enemy2.Reset(Vector2(1347.0f, 801.0f), false, false, 6, false,2); 
	this->enemy1.SetLevel(6); 
	this->enemy2.SetLevel(6); 
	this->enemy3.SetLevel(6); 
	this->head1.Initialize(L"Assets/BlankPlatform.png", this->d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY); 
	this->head1.Initialize(L"Assets/BlankPlatform.png", this->d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY); 
	this->enemyQuantity = 2; 
	this->enemySelected = 0;

	//****************************************************************************

	this->hud.Initialize(L"Assets/Hud_01.png", this-> d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);

	this->hud.Reset(Vector2(0.0f, windowBounds.Top));

	this->hearts.Initialize(L"Assets/Heart_Asset.png", this-> d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
	this->hearts.Reset(Vector2(350.0, windowBounds.Top + 15.0f));

	this->painKiller1.Initialize(L"Assets/PainKiller_Asset.png", this-> d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
	this->painKiller1.Reset(Vector2(920.0, windowBounds.Top + 15.0f));

	this->painKiller2.Initialize(L"Assets/PainKiller_Asset.png", this-> d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
	this->painKiller2.Reset(Vector2(982.0, windowBounds.Top + 15.0f));

	this->painKiller3.Initialize(L"Assets/PainKiller_Asset.png", this-> d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
	this->painKiller3.Reset(Vector2(1044.0, windowBounds.Top + 15.0f));

	this->spriteEnemiesLeft.Initialize(L"Assets/SpriteEnemiesLeft.png", this-> d3dDevice, this->d3dDeviceContext);
	this->spriteEnemiesLeft.Reset(Vector2(1510.0, windowBounds.Top + 20.0f));

	this->heart1.Initialize(L"Assets/Heart_Asset.png", this-> d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
	this->heart1.Reset(Vector2(420.0, windowBounds.Top + 20.0f));

	this->heart2.Initialize(L"Assets/Heart_Asset.png", this-> d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
	this->heart2.Reset(Vector2(486.0, windowBounds.Top + 20.0f));

	this->heart3.Initialize(L"Assets/Heart_Asset.png", this-> d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
	this->heart3.Reset(Vector2(552.0, windowBounds.Top + 20.0f));

	this->heart_Asset.Initialize(L"Assets/Heart_Asset.png", this-> d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
	this->heart_Asset.Reset(Vector2(1600.0, 910.0f));
	this->heart_Asset.SetBoundingBox(Windows::Foundation::Rect (0.0f, 0.0f, 48.0f, 28.0f));

	this->painKiller_Asset.Initialize(L"Assets/PainKiller_Asset.png", this-> d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
	this->painKiller_Asset.Reset(Vector2(550.0f, 545.0f));
	this->painKiller_Asset.SetBoundingBox(Windows::Foundation::Rect (0.0f, 0.0f, 59.0f, 91.0f));

	//************************************************************************************** Jake Evans
}
void SpriteGame::DrawLevel6()
{
	//Level Sprites
	this->background.Draw();

	//*************************************************************************************
	this->hud.Draw();
	if (this->lives == 3 )
	{
		heart1.Draw();
		heart2.Draw();
		heart3.Draw();
	}
	else if (lives == 2)
	{
		heart1.Draw();
		heart2.Draw();
	}
	else if (lives == 1)
	{
		heart1.Draw();
	}

	if (this->painKillers == 3 )
	{
		painKiller1.Draw();
		painKiller2.Draw();
		painKiller3.Draw();
	}
	else if (painKillers == 2)
	{
		painKiller1.Draw();
		painKiller2.Draw();
	}
	else if (painKillers == 1)
	{
		painKiller1.Draw();
	}
	//************************************************************************************ Jake Evans

	//Interactive Sprites

	this->heart_Asset.Draw();
	this->painKiller_Asset.Draw();
	this->hero.Draw();
	this->enemy1.Draw();
	this->head1.Draw();
	this->enemy2.Draw();
	this->head2.Draw();
	this->door.Draw();
}

void SpriteGame::LoadLevel7()
{
	this->killedEnemy = false;
	this->highlightEnemy = false;
	this->currentLevel = 7;
	//Set Background
	this->background.Initialize(L"Assets/Level7Background.png", this->d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
	this->door.Initialize(L"Assets/ClosedDoor.png", this->d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);

	//Set Platforms
	this->platform1.SetBoundingBox(Windows::Foundation::Rect (0.0f, 567.0f, 626.0f, 43.0f));
	this->platform2.SetBoundingBox(Windows::Foundation::Rect (626.0f, 567.0f, 629.0f, 82.0f));
	this->platform3.SetBoundingBox(Windows::Foundation::Rect (1355.0f, 570.0f, windowBounds.Right, 82.0f));
	this->platform4.SetBoundingBox(Windows::Foundation::Rect (0.0f, 999.0f, windowBounds.Right, 81.0f));

	this->door.SetBoundingBox(Windows::Foundation::Rect (1860.0f, 870.0f, windowBounds.Right, 69.0f));
	this->door.Reset(Vector2(1901.0f, 920.0f));
	this->ladder.SetBoundingBox(Windows::Foundation::Rect (1257.0f, 557.0f, 108.0f, 408.0f));
	this->ladder.Reset(Vector2 (1257.0f, 557.0f));
	this->ladder.Initialize(L"Assets/ladder2.png", this->d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
	//Set Hero
	this->hero.Initialize(L"Assets/HeroJumpIdle.png", this->d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
	this->hero.Reset(Vector2( 0.0f, (568.0f - 177)), false, false);

	//Set Enemies 
	this->enemy1.Initialize(L"Assets/bugwalksprite.png", this->d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY); 
	this->enemy1.Reset(Vector2(1146.0f, 300.0f), false, false, 6, false,1); 
	this->enemy2.Initialize(L"Assets/bugwalksprite.png", this->d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY); 
	this->enemy2.Reset(Vector2(969.0f, 842.0f), false, false, 6, false,2); 
	this->enemy3.Initialize(L"Assets/bugwalksprite.png", this->d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY); 
	this->enemy3.Reset(Vector2(139.0f, 842.0f), false, false, 6, false,3); 
	this->enemy1.SetLevel(7); 
	this->enemy2.SetLevel(7); 
	this->enemy3.SetLevel(7); 
	this->head1.Initialize(L"Assets/BlankPlatform.png", this->d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY); 
	this->head2.Initialize(L"Assets/BlankPlatform.png", this->d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY); 
	this->head3.Initialize(L"Assets/BlankPlatform.png", this->d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY); 

	this->enemyQuantity = 3; 
	this->enemySelected = 0; 

	//****************************************************************************

	this->hud.Initialize(L"Assets/Hud_01.png", this-> d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);

	this->hud.Reset(Vector2(0.0f, windowBounds.Top));

	this->hearts.Initialize(L"Assets/Heart_Asset.png", this-> d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
	this->hearts.Reset(Vector2(350.0, windowBounds.Top + 15.0f));

	this->painKiller1.Initialize(L"Assets/PainKiller_Asset.png", this-> d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
	this->painKiller1.Reset(Vector2(920.0, windowBounds.Top + 15.0f));

	this->painKiller2.Initialize(L"Assets/PainKiller_Asset.png", this-> d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
	this->painKiller2.Reset(Vector2(982.0, windowBounds.Top + 15.0f));

	this->painKiller3.Initialize(L"Assets/PainKiller_Asset.png", this-> d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
	this->painKiller3.Reset(Vector2(1044.0, windowBounds.Top + 15.0f));

	this->spriteEnemiesLeft.Initialize(L"Assets/SpriteEnemiesLeft.png", this-> d3dDevice, this->d3dDeviceContext);
	this->spriteEnemiesLeft.Reset(Vector2(1510.0, windowBounds.Top + 20.0f));

	this->heart1.Initialize(L"Assets/Heart_Asset.png", this-> d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
	this->heart1.Reset(Vector2(420.0, windowBounds.Top + 20.0f));

	this->heart2.Initialize(L"Assets/Heart_Asset.png", this-> d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
	this->heart2.Reset(Vector2(486.0, windowBounds.Top + 20.0f));

	this->heart3.Initialize(L"Assets/Heart_Asset.png", this-> d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
	this->heart3.Reset(Vector2(552.0, windowBounds.Top + 20.0f));

	this->heart_Asset.Initialize(L"Assets/Heart_Asset.png", this-> d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
	this->heart_Asset.Reset(Vector2(0.0, 900.0f));
	this->heart_Asset.SetBoundingBox(Windows::Foundation::Rect (0.0f, 0.0f, 48.0f, 28.0f));

	this->painKiller_Asset.Initialize(L"Assets/PainKiller_Asset.png", this-> d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
	this->painKiller_Asset.Reset(Vector2(200.0f, 480.0f));
	this->painKiller_Asset.SetBoundingBox(Windows::Foundation::Rect (0.0f, 0.0f, 59.0f, 91.0f));

	//************************************************************************************** Jake Evans

}
void SpriteGame::DrawLevel7()
{
	//Level Sprites
	this->background.Draw();
	this->ladder.Draw();

	//*************************************************************************************
	this->hud.Draw();
	if (this->lives == 3 )
	{
		heart1.Draw();
		heart2.Draw();
		heart3.Draw();
	}
	else if (lives == 2)
	{
		heart1.Draw();
		heart2.Draw();
	}
	else if (lives == 1)
	{
		heart1.Draw();
	}

	if (this->painKillers == 3 )
	{
		painKiller1.Draw();
		painKiller2.Draw();
		painKiller3.Draw();
	}
	else if (painKillers == 2)
	{
		painKiller1.Draw();
		painKiller2.Draw();
	}
	else if (painKillers == 1)
	{
		painKiller1.Draw();
	}
	//************************************************************************************ Jake Evans

	//Interactive Sprites

	this->heart_Asset.Draw();
	this->painKiller_Asset.Draw();
	this->hero.Draw();
	this->enemy1.Draw();
	this->head1.Draw();
	this->enemy2.Draw();
	this->head2.Draw();
	this->enemy3.Draw();
	this->head3.Draw();
	this->door.Draw();
}

void SpriteGame::LoadLevel8()
{
	this->killedEnemy = false;
	this->highlightEnemy = false;
	this->currentLevel = 8;
	//Set Background
	this->background.Initialize(L"Assets/Level8Background.png", this->d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);

	//Set Platforms
	this->platform1.SetBoundingBox(Windows::Foundation::Rect (0.0f, 610.0f, 378.0f, 37.0f));
	this->platform2.SetBoundingBox(Windows::Foundation::Rect (550.0f, 610.0f, 374.0f, 37.0f));
	this->platform3.SetBoundingBox(Windows::Foundation::Rect (925.0f, 610.0f, windowBounds.Right, 72.0f));
	this->platform4.SetBoundingBox(Windows::Foundation::Rect (0.0f, 990.0f, 130.0f, 90.0f));
	this->platform5.SetBoundingBox(Windows::Foundation::Rect (318.0f, 990.0f, windowBounds.Right, 90.0f));

	this->door.SetBoundingBox(Windows::Foundation::Rect (1860.0f, 870.0f, windowBounds.Right, 69.0f));
	this->door.Initialize(L"Assets/ClosedDoor.png", this->d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
	this->door.Reset(Vector2(1901.0f, 820.0f));
	this->ladder.SetBoundingBox(Windows::Foundation::Rect (416.0f, 583.0f, 108.0f, 408.0f));
	this->ladder.Reset(Vector2 (416.0f, 583.0f));
	this->ladder.Initialize(L"Assets/ladder2.png", this->d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);

	//Set Hero
	this->hero.Initialize(L"Assets/HeroJumpIdle.png", this->d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
	this->hero.Reset(Vector2( 0.0f, (610.0f - 177)), false, false);

	//Set Enemies 
	this->enemy1.Initialize(L"Assets/bugwalksprite.png", this->d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY); 
	this->enemy1.Reset(Vector2(954.0f, 850.0f ), false, false, 6, false,1); 
	this->enemy2.Initialize(L"Assets/bugwalksprite.png", this->d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY); 
	this->enemy2.Reset(Vector2(1500.0f, 400.0f), false, false, 6, false,2); 
	this->enemy1.SetLevel(8); 
	this->enemy2.SetLevel(8); 
	this->enemy3.SetLevel(8); 
	this->head1.Initialize(L"Assets/BlankPlatform.png", this->d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY); 
	this->head2.Initialize(L"Assets/BlankPlatform.png", this->d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);

	this->enemyQuantity = 1; 
	this->enemySelected = 0; 

	//****************************************************************************
	this->hud.Initialize(L"Assets/Hud_01.png", this-> d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);

	this->hud.Reset(Vector2(0.0f, windowBounds.Top));

	this->hearts.Initialize(L"Assets/Heart_Asset.png", this-> d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
	this->hearts.Reset(Vector2(350.0, windowBounds.Top + 15.0f));

	this->painKiller1.Initialize(L"Assets/PainKiller_Asset.png", this-> d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
	this->painKiller1.Reset(Vector2(920.0, windowBounds.Top + 15.0f));

	this->painKiller2.Initialize(L"Assets/PainKiller_Asset.png", this-> d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
	this->painKiller2.Reset(Vector2(982.0, windowBounds.Top + 15.0f));

	this->painKiller3.Initialize(L"Assets/PainKiller_Asset.png", this-> d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
	this->painKiller3.Reset(Vector2(1044.0, windowBounds.Top + 15.0f));

	this->spriteEnemiesLeft.Initialize(L"Assets/SpriteEnemiesLeft.png", this-> d3dDevice, this->d3dDeviceContext);
	this->spriteEnemiesLeft.Reset(Vector2(1510.0, windowBounds.Top + 20.0f));

	this->heart1.Initialize(L"Assets/Heart_Asset.png", this-> d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
	this->heart1.Reset(Vector2(420.0, windowBounds.Top + 20.0f));

	this->heart2.Initialize(L"Assets/Heart_Asset.png", this-> d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
	this->heart2.Reset(Vector2(486.0, windowBounds.Top + 20.0f));

	this->heart3.Initialize(L"Assets/Heart_Asset.png", this-> d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
	this->heart3.Reset(Vector2(552.0, windowBounds.Top + 20.0f));

	this->heart_Asset.Initialize(L"Assets/Heart_Asset.png", this-> d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
	this->heart_Asset.Reset(Vector2(0.0, 900.0f));
	this->heart_Asset.SetBoundingBox(Windows::Foundation::Rect (0.0f, 0.0f, 48.0f, 28.0f));

	this->painKiller_Asset.Initialize(L"Assets/PainKiller_Asset.png", this-> d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
	this->painKiller_Asset.Reset(Vector2(1700.0f, 520.0f));
	this->painKiller_Asset.SetBoundingBox(Windows::Foundation::Rect (0.0f, 0.0f, 59.0f, 91.0f));

	//************************************************************************************** Jake Evans
}
void SpriteGame::DrawLevel8()
{
	//Level Sprites
	this->background.Draw();
	this->ladder.Draw();

	//*************************************************************************************
	this->hud.Draw();
	if (this->lives == 3 )
	{
		heart1.Draw();
		heart2.Draw();
		heart3.Draw();
	}
	else if (lives == 2)
	{
		heart1.Draw();
		heart2.Draw();
	}
	else if (lives == 1)
	{
		heart1.Draw();
	}

	if (this->painKillers == 3 )
	{
		painKiller1.Draw();
		painKiller2.Draw();
		painKiller3.Draw();
	}
	else if (painKillers == 2)
	{
		painKiller1.Draw();
		painKiller2.Draw();
	}
	else if (painKillers == 1)
	{
		painKiller1.Draw();
	}
	//************************************************************************************ Jake Evans

	//Interactive Sprites

	this->heart_Asset.Draw();
	this->painKiller_Asset.Draw();
	this->hero.Draw();
	this->enemy1.Draw();
	this->enemy2.Draw();
	this->head1.Draw();
	this->head2.Draw();
	this->door.Draw();
}

void SpriteGame::LoadLevel9()
{
	this->killedEnemy = false;
	this->highlightEnemy = false;
	this->currentLevel = 9;
	//Set Background
	this->background.Initialize(L"Assets/Level9Background.png", this->d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
	this->door.Initialize(L"Assets/ClosedDoor.png", this->d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);

	//Set Platforms
	this->platform1.SetBoundingBox(Windows::Foundation::Rect (0.0f, 810.0f, 772.0f, 32.0f));
	this->platform2.SetBoundingBox(Windows::Foundation::Rect (964.0f, 810.0f, windowBounds.Right, 32.0f));
	this->platform3.SetBoundingBox(Windows::Foundation::Rect (0.0f, 1050.0f, windowBounds.Right, 30.0f));
	this->platform4.SetBoundingBox(Windows::Foundation::Rect (1421.0f, 788.0f, 295.0f, 1080.0f));

	this->door.SetBoundingBox(Windows::Foundation::Rect (1860.0f, 670.0f, windowBounds.Right, 69.0f));
	this->door.Reset(Vector2(1901.0f, 620.0f));
	//Set Hero
	this->hero.Initialize(L"Assets/HeroJumpIdle.png", this->d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
	this->hero.Reset(Vector2( 0.0f, (810.0f - 177)), false, false);

	//Set Enemies 

	this->enemy1.Initialize(L"Assets/bugwalksprite.png", this->d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY); 
	this->enemy1.Reset(Vector2(804.0f, 900.0f), false, false, 6, false,1); 
	this->enemy1.SetLevel(9); 
	this->enemy2.Initialize(L"Assets/bugwalksprite.png", this->d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
	this->enemy2.Reset(Vector2(1357.0f, 498.0f), false, false, 6, false, 2);
	this->enemy2.SetLevel(9); 
	this->enemy3.SetLevel(9); 
	this->head1.Initialize(L"Assets/BlankPlatform.png", this->d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY); 
	this->head2.Initialize(L"Assets/BlankPlatform.png", this->d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
	this->enemyQuantity = 2; 
	this->enemySelected = 0; 

	//****************************************************************************

	this->hud.Initialize(L"Assets/Hud_01.png", this-> d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);

	this->hud.Reset(Vector2(0.0f, windowBounds.Top));

	this->hearts.Initialize(L"Assets/Heart_Asset.png", this-> d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
	this->hearts.Reset(Vector2(350.0, windowBounds.Top + 15.0f));

	this->painKiller1.Initialize(L"Assets/PainKiller_Asset.png", this-> d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
	this->painKiller1.Reset(Vector2(920.0, windowBounds.Top + 15.0f));

	this->painKiller2.Initialize(L"Assets/PainKiller_Asset.png", this-> d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
	this->painKiller2.Reset(Vector2(982.0, windowBounds.Top + 15.0f));

	this->painKiller3.Initialize(L"Assets/PainKiller_Asset.png", this-> d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
	this->painKiller3.Reset(Vector2(1044.0, windowBounds.Top + 15.0f));

	this->spriteEnemiesLeft.Initialize(L"Assets/SpriteEnemiesLeft.png", this-> d3dDevice, this->d3dDeviceContext);
	this->spriteEnemiesLeft.Reset(Vector2(1510.0, windowBounds.Top + 20.0f));

	this->heart1.Initialize(L"Assets/Heart_Asset.png", this-> d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
	this->heart1.Reset(Vector2(420.0, windowBounds.Top + 20.0f));

	this->heart2.Initialize(L"Assets/Heart_Asset.png", this-> d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
	this->heart2.Reset(Vector2(486.0, windowBounds.Top + 20.0f));

	this->heart3.Initialize(L"Assets/Heart_Asset.png", this-> d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
	this->heart3.Reset(Vector2(552.0, windowBounds.Top + 20.0f));

	this->heart_Asset.Initialize(L"Assets/Heart_Asset.png", this-> d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
	this->heart_Asset.Reset(Vector2(1300.0, 965.0f));
	this->heart_Asset.SetBoundingBox(Windows::Foundation::Rect (0.0f, 0.0f, 48.0f, 28.0f));

	this->painKiller_Asset.Initialize(L"Assets/PainKiller_Asset.png", this-> d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
	this->painKiller_Asset.Reset(Vector2(200.0f, 720.0f));
	this->painKiller_Asset.SetBoundingBox(Windows::Foundation::Rect (0.0f, 0.0f, 59.0f, 91.0f));

	//************************************************************************************** Jake Evans
}
void SpriteGame::DrawLevel9()
{
	//Level Sprites
	this->background.Draw();

	//*************************************************************************************
	this->hud.Draw();
	if (this->lives == 3 )
	{
		heart1.Draw();
		heart2.Draw();
		heart3.Draw();
	}
	else if (lives == 2)
	{
		heart1.Draw();
		heart2.Draw();
	}
	else if (lives == 1)
	{
		heart1.Draw();
	}

	if (this->painKillers == 3 )
	{
		painKiller1.Draw();
		painKiller2.Draw();
		painKiller3.Draw();
	}
	else if (painKillers == 2)
	{
		painKiller1.Draw();
		painKiller2.Draw();
	}
	else if (painKillers == 1)
	{
		painKiller1.Draw();
	}
	//************************************************************************************ Jake Evans

	//Interactive Sprites
	this->hero.Draw();
	this->enemy1.Draw();
	this->head1.Draw();
	this->heart_Asset.Draw();
	this->painKiller_Asset.Draw();
	this->door.Draw();
}

void SpriteGame::LoadLevel10()
{
	this->killedEnemy = false;
	this->highlightEnemy = false;
	this->currentLevel = 10;
	//Set Background
	this->background.Initialize(L"Assets/Level10Background.png", this->d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
	this->door.Initialize(L"Assets/ClosedDoor.png", this->d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);

	//Set Platforms
	this->platform1.SetBoundingBox(Windows::Foundation::Rect (0.0f, 772.0f, 909.0f, 308.0f));
	this->platform2.SetBoundingBox(Windows::Foundation::Rect (1105.0f, 772.0f, 832.0f, 320.0f));

	this->door.SetBoundingBox(Windows::Foundation::Rect (1860.0f, 650.0f, windowBounds.Right, 69.0f));
	this->door.Reset(Vector2(1901.0f, 600.0f));
	//Set Hero
	this->hero.Initialize(L"Assets/HeroJumpIdle.png", this->d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
	this->hero.Reset(Vector2( 0.0f, (772.0f - 177)), false, false);

	this->head1.Initialize(L"Assets/BlankPlatform.png", this->d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);

	this->enemyQuantity = 0;
	this->enemySelected = 0;

	//****************************************************************************

	this->hud.Initialize(L"Assets/Hud_01.png", this-> d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);

	this->hud.Reset(Vector2(0.0f, windowBounds.Top));

	this->hearts.Initialize(L"Assets/Heart_Asset.png", this-> d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
	this->hearts.Reset(Vector2(350.0, windowBounds.Top + 15.0f));

	this->painKiller1.Initialize(L"Assets/PainKiller_Asset.png", this-> d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
	this->painKiller1.Reset(Vector2(920.0, windowBounds.Top + 15.0f));

	this->painKiller2.Initialize(L"Assets/PainKiller_Asset.png", this-> d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
	this->painKiller2.Reset(Vector2(982.0, windowBounds.Top + 15.0f));

	this->painKiller3.Initialize(L"Assets/PainKiller_Asset.png", this-> d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
	this->painKiller3.Reset(Vector2(1044.0, windowBounds.Top + 15.0f));

	this->spriteEnemiesLeft.Initialize(L"Assets/SpriteEnemiesLeft.png", this-> d3dDevice, this->d3dDeviceContext);
	this->spriteEnemiesLeft.Reset(Vector2(1510.0, windowBounds.Top + 20.0f));

	this->heart1.Initialize(L"Assets/Heart_Asset.png", this-> d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
	this->heart1.Reset(Vector2(420.0, windowBounds.Top + 20.0f));

	this->heart2.Initialize(L"Assets/Heart_Asset.png", this-> d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
	this->heart2.Reset(Vector2(486.0, windowBounds.Top + 20.0f));

	this->heart3.Initialize(L"Assets/Heart_Asset.png", this-> d3dDevice, this->d3dDeviceContext, this->scaleX, this->scaleY);
	this->heart3.Reset(Vector2(552.0, windowBounds.Top + 20.0f));

	//************************************************************************************** Jake Evans
}
void SpriteGame::DrawLevel10()
{
	//Level Sprites
	this->background.Draw();


	//*************************************************************************************
	this->hud.Draw();
	if (this->lives == 3 )
	{
		heart1.Draw();
		heart2.Draw();
		heart3.Draw();
	}
	else if (lives == 2)
	{
		heart1.Draw();
		heart2.Draw();
	}
	else if (lives == 1)
	{
		heart1.Draw();
	}

	if (this->painKillers == 3 )
	{
		painKiller1.Draw();
		painKiller2.Draw();
		painKiller3.Draw();
	}
	else if (painKillers == 2)
	{
		painKiller1.Draw();
		painKiller2.Draw();
	}
	else if (painKillers == 1)
	{
		painKiller1.Draw();
	}
	//************************************************************************************ Jake Evans

	//Interactive Sprites
	this->hero.Draw();
	this->door.Draw();
}

void SpriteGame::DeathPosition(DirectX::SimpleMath::Vector2 deathPosition)
{
	this->enemyDeathPosition = deathPosition;
}

//Load Level / Draw Level Template

//void SpriteGame::LoadLevel()
//{
//	this->killedEnemy = false;
//	this->highlightEnemy = false;
//
//	//Set Background
//	this->background.Initialize(L"Assets/LevelBackground.png", this->d3dDevice, this->d3dDeviceContext);
//
//	//Set Platforms
//	this->platform1.SetBoundingBox(Windows::Foundation::Rect (0.0f, 0.0f, 0.0f, 0.0f));
//	this->platform2.SetBoundingBox(Windows::Foundation::Rect (0.0f, 0.0f, 0.0f, 0.0f));
//	this->platform3.SetBoundingBox(Windows::Foundation::Rect (0.0f, 0.0f, 0.0f, 0.0f));
//	this->platform4.SetBoundingBox(Windows::Foundation::Rect (0.0f, 0.0f, 0.0f, 0.0f));
//	this->platform5.SetBoundingBox(Windows::Foundation::Rect (0.0f, 0.0f, 0.0f, 0.0f));
//	this->platform6.SetBoundingBox(Windows::Foundation::Rect (0.0f, 0.0f, 0.0f, 0.0f));
//	this->door.SetBoundingBox(Windows::Foundation::Rect (0.0f, 0.0f, 0.0f, 0.0f));
//
//	//Set Hero
//	this->hero.Initialize(L"Assets/HeroJumpIdle.png", this->d3dDevice, this->d3dDeviceContext);
//	this->hero.Reset(Vector2( 0.0f, (0.0f - 177)), false, false);
//
//	//Set Enemies
//	this->enemy1.Initialize(L"Assets/bugwalksprite.png", this->d3dDevice, this->d3dDeviceContext);
//	this->enemy1.Reset(Vector2( (windowBounds.Right - 200), (0.0f - 200)), false, false, 6);
//}
//void SpriteGame::DrawLevel()
//{
//	//Level Sprites
//	this->background.Draw();
//
//	//Interactive Sprites
//	this->hero.Draw();
//	this->enemy1.Draw();
//}
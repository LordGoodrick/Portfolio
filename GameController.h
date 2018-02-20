#pragma once

#include <Xinput.h>
#include <SimpleMath.h>

class GameController
{
public:
  GameController(void);
  ~GameController(void);

  void Update();
  bool IsConnected();
  bool IsButtonDown(unsigned short int button);
  DirectX::SimpleMath::Vector2 GetThumbstick(unsigned short int thumbstick);

private:
  XINPUT_STATE gamepadState;
  bool connected;

};


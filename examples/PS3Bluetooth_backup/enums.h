enum LED
{
  LED1 = 0x01,
  LED2 = 0x02,
  LED3 = 0x04,
  LED4 = 0x08,

  LED5 = 0x09,
  LED6 = 0x0A,
  LED7 = 0x0C,
  LED8 = 0x0D,
  LED9 = 0x0E,
  LED10 = 0x0F,
};
enum Colors
{
  //Used to set the colors of the move controller            
  Red = 0xFF0000,//((255 << 16) | (0 << 8) | 0);
  Green = 0xFF00,//((0 << 16) | (255 << 8) | 0);
  Blue = 0xFF,//((0 << 16) | (0 << 8) | 255);

  Yellow = 0xFFEB04,//((255 << 16) | (235 << 8) | 4);
  Lightblue = 0xFFFF,//((0 << 16) | (255 << 8) | 255);
  Purble = 0xFF00FF,//((255 << 16) | (0 << 8) | 255);

  White = 0xFFFFFF,//((255 << 16) | (255 << 8) | 255);
  Off = 0x00,//((0 << 16) | (0 << 8) | 0);
};

enum Button
{
  // byte location | bit location

  //Sixaxis Dualshcock 3 & Navigation controller 
  SELECT = (11 << 8) | 0x01,
  L3 = (11 << 8) | 0x02,
  R3 = (11 << 8) | 0x04,
  START = (11 << 8) | 0x08,
  UP = (11 << 8) | 0x10,
  RIGHT = (11 << 8) | 0x20,
  DOWN = (11 << 8) | 0x40,
  LEFT = (11 << 8) | 0x80,

  L2 = (12 << 8) | 0x01,
  R2 = (12 << 8) | 0x02,
  L1 = (12 << 8) | 0x04,
  R1 = (12 << 8) | 0x08,
  TRIANGLE = (12 << 8) | 0x10,
  CIRCLE = (12 << 8) | 0x20,
  CROSS = (12 << 8) | 0x40,
  SQUARE = (12 << 8) | 0x80,

  PS = (13 << 8) | 0x01,

  //Playstation Move Controller
  SELECT_MOVE = (10 << 8) | 0x01,
  START_MOVE = (10 << 8) | 0x08,

  TRIANGLE_MOVE = (11 << 8) | 0x10,
  CIRCLE_MOVE = (11 << 8) | 0x20,
  CROSS_MOVE = (11 << 8) | 0x40,
  SQUARE_MOVE = (11 << 8) | 0x80,

  PS_MOVE = (12 << 8) | 0x01,
  MOVE_MOVE = (12 << 8) | 0x08,//covers 12 bits - we only need to read the top 8            
  T_MOVE = (12 << 8) | 0x10,//covers 12 bits - we only need to read the top 8            
};
enum AnalogButton
{
  //Sixaxis Dualshcock 3 & Navigation controller
  UP_ANALOG = 23,
  RIGHT_ANALOG = 24,
  DOWN_ANALOG = 25,
  LEFT_ANALOG = 26,

  L2_ANALOG = 27,
  R2_ANALOG = 28,
  L1_ANALOG = 29,
  R1_ANALOG = 30,
  TRIANGLE_ANALOG = 31,
  CIRCLE_ANALOG = 32,
  CROSS_ANALOG = 33,
  SQUARE_ANALOG = 34,

  //Playstation Move Controller
  T_MOVE_ANALOG = 15,//Both at byte 14 (last reading) and byte 15 (current reading)
};
enum AnalogHat
{
  LeftHatX = 15,
  LeftHatY = 16,
  RightHatX = 17,
  RightHatY = 18,
};
enum Sensor
{
  //Sensors inside the Sixaxis Dualshock 3 controller
  aX = 50,
  aY = 52,
  aZ = 54,
  gZ = 56,

  //Sensors inside the move motion controller - it only reads the 2nd frame
  aXmove = 28,
  aZmove = 30,
  aYmove = 32,

  gXmove = 40,
  gZmove = 42,
  gYmove = 44,

  tempMove = 46,

  mXmove = 47,
  mZmove = 49,
  mYmove = 50,            
};
enum Angle
{
  Pitch = 0x01,
  Roll = 0x02,
};
enum Status
{
  // byte location | bit location
  Plugged = (38 << 8) | 0x02,
  Unplugged = (38 << 8) | 0x03,

  Charging = (39 << 8) | 0xEE,
  NotCharging = (39 << 8) | 0xF1,
  Shutdown = (39 << 8) | 0x01,
  Dying = (39 << 8) | 0x02,
  Low = (39 << 8) | 0x03,
  High = (39 << 8) | 0x04,
  Full = (39 << 8) | 0x05,

  MoveCharging = (21 << 8) | 0xEE,
  MoveNotCharging = (21 << 8) | 0xF1,
  MoveShutdown = (21 << 8) | 0x01,
  MoveDying = (21 << 8) | 0x02,
  MoveLow = (21 << 8) | 0x03,
  MoveHigh = (21 << 8) | 0x04,
  MoveFull = (21 << 8) | 0x05,

  CableRumble = (40 << 8) | 0x10,//Opperating by USB and rumble is turned on
  Cable = (40 << 8) | 0x12,//Opperating by USB and rumble is turned off 
  BluetoothRumble = (40 << 8) | 0x14,//Opperating by bluetooth and rumble is turned on
  Bluetooth = (40 << 8) | 0x16,//Opperating by bluetooth and rumble is turned off                        
};
enum Rumble
{
  RumbleHigh = 0x10,
  RumbleLow = 0x20,            
};


boolean GetButton(Button b)
{
  if (l2capinbuf == NULL)
    return false;
  if (((unsigned char)l2capinbuf[(unsigned int)b >> 8] & ((byte)b & 0xff)) > 0)
    return true;
  else
    return false;
}
byte GetAnalogButton(AnalogButton a)
{
  if (l2capinbuf == NULL)
    return 0;
  return (byte)((unsigned char)l2capinbuf[(unsigned int)a]);
}
byte GetAnalogHat(AnalogHat a)
{
  if (l2capinbuf == NULL)
    return 0;                        
  return (byte)((unsigned char)l2capinbuf[(unsigned int)a]);            
}
long GetSensor(Sensor a)
{
  if (a == aX || a == aY || a == aZ || a == gZ)
  {
    if (l2capinbuf == NULL)
      return 0;
    return (((unsigned char)l2capinbuf[(unsigned int)a] << 8) | (unsigned char)l2capinbuf[(unsigned int)a + 1]);
  }
  else if (a == mXmove || a == mYmove || a == mZmove)
  {
    //Might not be correct, haven't tested it yet
    if (l2capinbuf == NULL)
      return 0;
    if (a == mXmove)
      return (((unsigned char)l2capinbuf[(unsigned int)a + 1] << 0x04) | ((unsigned char)l2capinbuf[(unsigned int)a] << 0x0C));
    //return (((unsigned char)l2capinbuf[(unsigned int)a + 1]) | (((unsigned char)l2capinbuf[(unsigned int)a] & 0x0F)) << 8);
    else if (a == mYmove)
      return (((unsigned char)l2capinbuf[(unsigned int)a + 1] & 0xF0) | ((unsigned char)l2capinbuf[(unsigned int)a] << 0x08));
    //return (((unsigned char)l2capinbuf[(unsigned int)a + 1]) | (((unsigned char)l2capinbuf[(unsigned int)a] & 0x0F)) << 8);
    else if (a == mZmove)
      return (((unsigned char)l2capinbuf[(unsigned int)a + 1] << 0x0F) | ((unsigned char)l2capinbuf[(unsigned int)a] << 0x0C));
    //return ((((unsigned char)l2capinbuf[(unsigned int)a + 1] & 0xF0) >> 4) | ((unsigned char)l2capinbuf[(unsigned int)a] << 4));
    else
      return 0;                
  }
  else if (a == tempMove)
  {
    if (l2capinbuf == NULL)
      return 0;
    return ((((unsigned char)l2capinbuf[(unsigned int)a + 1] & 0xF0) >> 4) | ((unsigned char)l2capinbuf[(unsigned int)a] << 4));    
  }
  else
  {
    if (l2capinbuf == NULL)
      return 0;
    return ((((unsigned char)l2capinbuf[(unsigned int)a + 1] << 8) | (unsigned char)l2capinbuf[(unsigned int)a]) - 0x8000);                
  }
}
short GetAngle(Angle a, boolean resolution)//Boolean indicate if 360-degrees resolution is used or not - set false if you want to use both axis
{
  double accXin;
  double accXval;
  double Pitch;

  double accYin;
  double accYval;
  double Roll;

  double accZin;
  double accZval;

  //Data for the Kionix KXPC4 used in DualShock 3
  double sensivity = 204.6;//0.66/3.3*1023 (660mV/g)
  double zeroG = 511.5;//1.65/3.3*1023 (1,65V)
  double R;//force vector

  accXin = GetSensor(aX);
  accXval = (zeroG - accXin) / sensivity;//Convert to g's
  accXval *= 2;

  accYin = GetSensor(aY);
  accYval = (zeroG - accYin) / sensivity;//Convert to g's
  accYval *= 2;

  accZin = GetSensor(aZ);
  accZval = (zeroG - accZin) / sensivity;//Convert to g's
  accZval *= 2;

  //Debug.Print("accXin: " + accXin + " accYin: " + accYin + " accZin: " + accZin);
  //Debug.Print("aX: " + accXval + " aY: " + accYval + " aZ: " + accZval);

  R = sqrt(pow(accXval, 2) + pow(accYval, 2) + pow(accZval, 2));

  if (a == Pitch)
  {
    //the result will come out as radians, so it is multiplied by 180/pi, to convert to degrees
    //In the end it is minus by 90, so its 0 degrees when in horizontal postion
    Pitch = acos(accXval / R) * 180 / PI - 90;
    //Debug.Print("accXangle: " + accXangle);

    if(resolution)
    {
      if (accZval < 0)//Convert to 360 degrees resolution - uncomment if you need both pitch and roll
      {
        if (Pitch < 0)                    
          Pitch = -180 - Pitch;                    
        else                    
          Pitch = 180 - Pitch;                    
      }
    }
    return (short)Pitch;

  }
  else
  {
    //the result will come out as radians, so it is multiplied by 180/pi, to convert to degrees
    //In the end it is minus by 90, so its 0 degrees when in horizontal postion
    Roll = acos(accYval / R) * 180 / PI - 90;
    //Debug.Print("accYangle: " + accYangle);

    if(resolution)
    {
      if (accZval < 0)//Convert to 360 degrees resolution - uncomment if you need both pitch and roll
      {
        if (Roll < 0)                   
          Roll = -180 - Roll;
        else
          Roll = 180 - Roll;
      }
    }
    return (short)Roll;
  }
}
boolean GetStatus(Status c)
{
  if (l2capinbuf == NULL)
    return false;
  if ((unsigned char)l2capinbuf[(unsigned int)c >> 8] == ((char)c & 0xff))
    return true;
  return false;
}
String GetStatusString()
{
  if (!PS3MoveBTConnected)
  {
    String ConnectionStatus = "";
    if (GetStatus(Plugged)) ConnectionStatus = "Plugged";
    else if (GetStatus(Unplugged)) ConnectionStatus = "Unplugged";
    else ConnectionStatus = "Error";

    String PowerRating = "";
    if (GetStatus(Charging)) PowerRating = "Charging";
    else if (GetStatus(NotCharging)) PowerRating = "Not Charging";
    else if (GetStatus(Shutdown)) PowerRating = "Shutdown";
    else if (GetStatus(Dying)) PowerRating = "Dying";
    else if (GetStatus(Low)) PowerRating = "Low";
    else if (GetStatus(High)) PowerRating = "High";
    else if (GetStatus(Full)) PowerRating = "Full";
    else PowerRating = "Error: " + (unsigned char)l2capinbuf[39];

    String WirelessStatus = "";
    if (GetStatus(CableRumble)) WirelessStatus = "Cable - Rumble is on";
    else if (GetStatus(Cable)) WirelessStatus = "Cable - Rumble is off";
    else if (GetStatus(BluetoothRumble)) WirelessStatus = "Bluetooth - Rumble is on";
    else if (GetStatus(Bluetooth)) WirelessStatus = "Bluetooth - Rumble is off";
    else WirelessStatus = "Error";

    return ("ConnectionStatus: " + ConnectionStatus + " - PowerRating: " + PowerRating + " - WirelessStatus: " + WirelessStatus);
  }
  else
  {
    String PowerRating = "";
    if (GetStatus(MoveCharging)) PowerRating = "Charging";
    else if (GetStatus(MoveNotCharging)) PowerRating = "Not Charging";
    else if (GetStatus(MoveShutdown)) PowerRating = "Shutdown";
    else if (GetStatus(MoveDying)) PowerRating = "Dying";
    else if (GetStatus(MoveLow)) PowerRating = "Low";
    else if (GetStatus(MoveHigh)) PowerRating = "High";
    else if (GetStatus(MoveFull)) PowerRating = "Full";
    else PowerRating = "Error: " + (unsigned char)l2capinbuf[21];

    return ("PowerRating: " + PowerRating);
  }            
}




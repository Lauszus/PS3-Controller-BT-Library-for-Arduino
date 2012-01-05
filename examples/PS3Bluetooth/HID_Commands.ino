/*                          HCI ACL Data Packet
 *
 *   buf[0]          buf[1]          buf[2]          buf[3]
 *   0       4       8    11 12      16              24            31 MSB
 *  .-+-+-+-+-+-+-+-|-+-+-+-|-+-|-+-|-+-+-+-+-+-+-+-|-+-+-+-+-+-+-+-.
 *  |      HCI Handle       |PB |BC |       Data Total Length       |   HCI ACL Data Packet
 *  .-+-+-+-+-+-+-+-|-+-+-+-|-+-|-+-|-+-+-+-+-+-+-+-|-+-+-+-+-+-+-+-.
 *
 *   buf[4]          buf[5]          buf[6]          buf[7]
 *   0               8               16                            31 MSB
 *  .-+-+-+-+-+-+-+-|-+-+-+-+-+-+-+-|-+-+-+-+-+-+-+-|-+-+-+-+-+-+-+-.
 *  |            Length             |          Channel ID           |   Basic L2CAP header
 *  .-+-+-+-+-+-+-+-|-+-+-+-+-+-+-+-|-+-+-+-+-+-+-+-|-+-+-+-+-+-+-+-.
 *
 *   buf[8]          buf[9]          buf[10]         buf[11]
 *   0               8               16                            31 MSB
 *  .-+-+-+-+-+-+-+-|-+-+-+-+-+-+-+-|-+-+-+-+-+-+-+-|-+-+-+-+-+-+-+-.
 *  |     Code      |  Identifier   |            Length             |   Control frame (C-frame)
 *  .-+-+-+-+-+-+-+-|-+-+-+-+-+-+-+-|-+-+-+-+-+-+-+-|-+-+-+-+-+-+-+-.   (signaling packet format)
 */

/************************************************************/
/*                    HID Commands                          */
/************************************************************/

//Playstation Sixaxis Dualshoch Controller commands
void HID_Command(char* data, unsigned int length)
{
  char buf[64];
  buf[0] = (char)(hci_handle & 0xff);    // HCI handle with PB,BC flag
  buf[1] = (char)(((hci_handle >> 8) & 0x0f) | 0x20);
  buf[2] = (char)((4 + length) & 0xff); // HCI ACL total data length
  buf[3] = (char)((4 + length) >> 8);
  buf[4] = (char)(length & 0xff); // L2CAP header: Length
  buf[5] = (char)(length >> 8);
  buf[6] = control_scid[0];//Both the Navigation and Dualshock controller sends data via the controller channel
  buf[7] = control_scid[1];

  for (unsigned int i = 0; i < length; i++)//L2CAP C-frame            
    buf[8 + i] = data[i];

  dtimeHID = millis() - timerHID;

  if (dtimeHID <= 250)// Check if is has been more than 250ms since last command                
    delay((int)(250 - dtimeHID));//There have to be a delay between commands

  Usb.outTransfer(BT_ADDR, ep_record[ DATAOUT_PIPE ].epAddr, (8 + length), buf);

  timerHID = millis();
}
void hid_setAllOff()
{
  for (int i = 0; i < OUTPUT_REPORT_BUFFER_SIZE; i++)
    HIDBuffer[i + 2] = pgm_read_byte(&OUTPUT_REPORT_BUFFER[i]);//First two bytes reserved for report type and ID

  HID_Command(HIDBuffer, OUTPUT_REPORT_BUFFER_SIZE + 2);
}
void hid_setRumbleOff()
{
  HIDBuffer[3] = 0x00;
  HIDBuffer[4] = 0x00;//low mode off
  HIDBuffer[5] = 0x00;
  HIDBuffer[6] = 0x00;//high mode off

  HID_Command(HIDBuffer, OUTPUT_REPORT_BUFFER_SIZE + 2);
}
void hid_setRumbleOn(Rumble mode)
{
  /* Still not totally sure how it works, maybe something like this instead?
   * 3 - duration_right
   * 4 - power_right
   * 5 - duration_left
   * 6 - power_left
   */
  if (((unsigned int)mode & 0x30) > 0)
  {
    HIDBuffer[3] = 0xfe;
    HIDBuffer[5] = 0xfe;

    if (mode == RumbleHigh)
    {
      HIDBuffer[4] = 0;//low mode off
      HIDBuffer[6] = 0xff;//high mode on
    }
    else
    {
      HIDBuffer[4] = 0xff;//low mode on
      HIDBuffer[6] = 0;//high mode off
    }

    HID_Command(HIDBuffer, OUTPUT_REPORT_BUFFER_SIZE + 2);
  }
}
void hid_setLedOff(LED a)
{
  //check if LED is already off
  if ((char)((char)(((unsigned int)a << 1) & HIDBuffer[11])) != 0)
  {
    //set the LED into the write buffer
    HIDBuffer[11] = (byte)((byte)(((unsigned int)a & 0x0f) << 1) ^ HIDBuffer[11]);

    HID_Command(HIDBuffer, OUTPUT_REPORT_BUFFER_SIZE + 2);
  }            
}
void hid_setLedOn(LED a)
{
  HIDBuffer[11] = (char)((char)(((unsigned int)a & 0x0f) << 1) | HIDBuffer[11]);

  HID_Command(HIDBuffer, OUTPUT_REPORT_BUFFER_SIZE + 2);            
}
void hid_enable_sixaxis()//Command used to enable the Dualshock 3 and Navigation controller
{
  char cmd_buf[12];
  cmd_buf[0] = 0x53;// HID BT Set_report (0x50) | Report Type (Feature 0x03)
  cmd_buf[1] = 0xF4;// Report ID
  cmd_buf[2] = 0x42;// Special PS3 Controller enable commands
  cmd_buf[3] = 0x03;
  cmd_buf[4] = 0x00;
  cmd_buf[5] = 0x00;

  HID_Command(cmd_buf, 6);
}

//Playstation Move Controller commands
void HIDMove_Command(char* data, unsigned int length)
{
  char buf[64];
  buf[0] = (char)(hci_handle & 0xff);    // HCI handle with PB,BC flag
  buf[1] = (char)(((hci_handle >> 8) & 0x0f) | 0x20);
  buf[2] = (char)((4 + length) & 0xff); // HCI ACL total data length
  buf[3] = (char)((4 + length) >> 8);
  buf[4] = (char)(length & 0xff); // L2CAP header: Length
  buf[5] = (char)(length >> 8);
  buf[6] = interrupt_scid[0];//The Move controller sends it's data via the intterrupt channel
  buf[7] = interrupt_scid[1];

  for (unsigned int i = 0; i < length; i++)//L2CAP C-frame            
    buf[8 + i] = data[i];

  dtimeHID = millis() - timerHID;
  
  if (dtimeHID <= 250)// Check if is has been less than 200ms since last command                            
    delay((int)(250 - dtimeHID));//There have to be a delay between commands
  
  Usb.outTransfer(BT_ADDR, ep_record[ DATAOUT_PIPE ].epAddr, (8 + length), buf);
  
  timerHID = millis();
}
void hid_MoveSetBulb(char r, char g, char b)//Use this to set the Color using RGB values
{            
  //set the Bulb's values into the write buffer            
  HIDMoveBuffer[3] = r;
  HIDMoveBuffer[4] = g;
  HIDMoveBuffer[5] = b;

  HIDMove_Command(HIDMoveBuffer, HIDMoveBufferSize);   
}
void hid_MoveSetBulb(Colors color)//Use this to set the Color using the predefined colors in "enums.h"
{
  //set the Bulb's values into the write buffer            
  HIDMoveBuffer[3] = (char)(color >> 16);
  HIDMoveBuffer[4] = (char)(color >> 8);
  HIDMoveBuffer[5] = (char)(color);  

  HIDMove_Command(HIDMoveBuffer, HIDMoveBufferSize);
}
void hid_MoveSetRumble(char rumble)
{
  //set the rumble value into the write buffer
  HIDMoveBuffer[7] = rumble;

  HIDMove_Command(HIDMoveBuffer, HIDMoveBufferSize);            
}



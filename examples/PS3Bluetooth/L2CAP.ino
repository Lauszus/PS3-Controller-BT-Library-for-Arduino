void ACL_event_task()
{  
  Usb.inTransfer(BT_ADDR, ep_record[ DATAIN_PIPE ].epAddr, MAX_BUFFER_SIZE, l2capinbuf, USB_NAK_NOWAIT); // input on endpoint 2
  if (((l2capinbuf[0] | (l2capinbuf[1] << 8)) == (hci_handle | 0x2000)))//acl_handle_ok  
  {
    if ((l2capinbuf[6] | (l2capinbuf[7] << 8)) == 0x0001)//l2cap_control - Channel ID for ACL-U                                
    {
      if (l2capinbuf[8] != 0x00)
      {
        //Serial.print("L2CAP Signaling Command - 0x");Serial.println(l2capinbuf[8],HEX);
      }
      if (l2capinbuf[8] == L2CAP_CMD_COMMAND_REJECT)       
      {           
        printProgStr(Cmd_Reject_str);
        Serial.print(String(String((unsigned char)l2capinbuf[13],HEX) + " " + String((unsigned char)l2capinbuf[12],HEX) + " Data: " + String((unsigned char)l2capinbuf[17],HEX) + " " + String((unsigned char)l2capinbuf[16],HEX) + " " + String((unsigned char)l2capinbuf[15],HEX) + " " + String((unsigned char)l2capinbuf[14],HEX)));
      }
      else if (l2capinbuf[8] == L2CAP_CMD_CONNECTION_REQUEST)
      {
        //Serial.print(String("\r\nPSM: 0x" + String((unsigned char)l2capinbuf[13],HEX) + " " + String((unsigned char)l2capinbuf[12],HEX) + " SCID: 0x" + String((unsigned char)l2capinbuf[15],HEX) + " " + String((unsigned char)l2capinbuf[14],HEX)));     
        if ((l2capinbuf[13] | l2capinbuf[12]) == L2CAP_PSM_HID_CTRL)
        {
          identifier = l2capinbuf[9];
          control_scid[0] = l2capinbuf[14];
          control_scid[1] = l2capinbuf[15];
          l2cap_event_flag |= L2CAP_EV_CONTROL_CONNECTION_REQUEST;
        }
        else if ((l2capinbuf[13] | l2capinbuf[12]) == L2CAP_PSM_HID_INTR)
        {
          identifier = l2capinbuf[9];
          interrupt_scid[0] = l2capinbuf[14];
          interrupt_scid[1] = l2capinbuf[15];
          l2cap_event_flag |= L2CAP_EV_INTERRUPT_CONNECTION_REQUEST;                                        
        }
      }
      else if (l2capinbuf[8] == L2CAP_CMD_CONFIG_RESPONSE)
      {
        if (l2capinbuf[12] == control_dcid[0] && l2capinbuf[13] == control_dcid[1])
        {
          if ((l2capinbuf[16] | (l2capinbuf[17] << 8)) == 0x0000)//Success
          {
            //Serial.println("HID Control Configuration Complete");
            l2cap_event_flag |= L2CAP_EV_CONTROL_CONFIG_SUCCESS;
          }
        }
        else if (l2capinbuf[12] == interrupt_dcid[0] && l2capinbuf[13] == interrupt_dcid[1])
        {
          if ((l2capinbuf[16] | (l2capinbuf[17] << 8)) == 0x0000)//Success
          {
            //Serial.println("HID Interrupt Configuration Complete");
            l2cap_event_flag |= L2CAP_EV_INTERRUPT_CONFIG_SUCCESS;
          }
        }
      }
      else if (l2capinbuf[8] == L2CAP_CMD_CONFIG_REQUEST)
      {
        if (l2capinbuf[12] == control_dcid[0] && l2capinbuf[13] == control_dcid[1])
        {
          //Serial.println("HID Control Configuration Request");
          identifier = l2capinbuf[9]; 
          l2cap_event_flag |= L2CAP_EV_CONTROL_CONFIG_REQUEST;          
        }
        else if (l2capinbuf[12] == interrupt_dcid[0] && l2capinbuf[13] == interrupt_dcid[1])
        {
          //Serial.println("HID Interrupt Configuration Request");
          identifier = l2capinbuf[9];
          l2cap_event_flag |= L2CAP_EV_INTERRUPT_CONFIG_REQUEST;          
        }
      }                                    
      else if (l2capinbuf[8] == L2CAP_CMD_DISCONNECT_REQUEST)
      {
        if (l2capinbuf[12] == control_dcid[0] && l2capinbuf[13] == control_dcid[1])
        {
          printProgStr(Disconnet_Req_Control_str);          
        }
        else if (l2capinbuf[12] == interrupt_dcid[0] && l2capinbuf[13] == interrupt_dcid[1])
        {
          printProgStr(Disconnet_Req_Interrupt_str);
        }          
      }
      else if (l2capinbuf[8] == L2CAP_CMD_DISCONNECT_RESPONSE)
      {
        if (l2capinbuf[12] == control_scid[0] && l2capinbuf[13] == control_scid[1])
        {                                        
          //Serial.println("Disconnected Response: Disconnected Control");
          identifier = l2capinbuf[9];
          l2cap_event_flag |= L2CAP_EV_CONTROL_DISCONNECT_RESPONSE;
        }
        else if (l2capinbuf[12] == interrupt_scid[0] && l2capinbuf[13] == interrupt_scid[1])
        {                                        
          //Serial.println("Disconnected Response: Disconnected Interrupt");
          identifier = l2capinbuf[9];
          l2cap_event_flag |= L2CAP_EV_INTERRUPT_DISCONNECT_RESPONSE;                                        
        }
      }                                     
    }                                
    else if (l2capinbuf[6] == interrupt_dcid[0] && l2capinbuf[7] == interrupt_dcid[1])//l2cap_interrupt
    {                                
      //Serial.println("L2CAP Interrupt");  
      if(PS3BTConnected || PS3MoveBTConnected || PS3NavigationBTConnected)
      {
        readReport();
        //printReport();//Uncomment for debugging      
      }
    }
    L2CAP_task();
  }
}
void L2CAP_task()
        {
            switch (l2cap_state)
            {
                case L2CAP_EV_WAIT:
                    break;
                case L2CAP_EV_CONTROL_SETUP:
                    if (l2cap_control_connection_reguest)
                    {
                        printProgStr(HID_Control_Connect_Req_str);                     
                        l2cap_connection_response(identifier, control_dcid, control_scid, PENDING);                        
                        l2cap_connection_response(identifier, control_dcid, control_scid, SUCCESSFUL);                        
                        identifier++;
                        l2cap_config_request(identifier, control_scid);                        

                        l2cap_state = L2CAP_EV_CONTROL_REQUEST;
                    }
                    break;
                case L2CAP_EV_CONTROL_REQUEST:
                    if (l2cap_control_config_reguest)
                    {
                        printProgStr(HID_Control_Config_Req_str);
                        l2cap_config_response(identifier, control_scid);                        
                        l2cap_state = L2CAP_EV_CONTROL_SUCCESS;
                    }
                    break;

                case L2CAP_EV_CONTROL_SUCCESS:
                    if (l2cap_control_config_success)
                    {
                        printProgStr(HID_Control_Success_str);
                        l2cap_state = L2CAP_EV_INTERRUPT_SETUP;
                    }
                    break;
                case L2CAP_EV_INTERRUPT_SETUP:
                    if (l2cap_interrupt_connection_reguest)
                    {
                        printProgStr(HID_Interrupt_Connect_Req_str);
                        l2cap_connection_response(identifier, interrupt_dcid, interrupt_scid, PENDING);                        
                        l2cap_connection_response(identifier, interrupt_dcid, interrupt_scid, SUCCESSFUL);                        
                        identifier++;
                        l2cap_config_request(identifier, interrupt_scid);                        

                        l2cap_state = L2CAP_EV_INTERRUPT_REQUEST;
                    }
                    break;
                case L2CAP_EV_INTERRUPT_REQUEST:
                    if (l2cap_interrupt_config_reguest)
                    {
                        printProgStr(HID_Interrupt_Config_Req_str);
                        l2cap_config_response(identifier, interrupt_scid);                        
                        l2cap_state = L2CAP_EV_INTERRUPT_SUCCESS;
                    }
                    break;
                case L2CAP_EV_INTERRUPT_SUCCESS:
                    if (l2cap_interrupt_config_success)
                    {
                        printProgStr(HID_Interrupt_Success_str);
                        l2cap_state = L2CAP_EV_HID_ENABLE_SIXAXIS;
                    }
                    break;
                case L2CAP_EV_HID_ENABLE_SIXAXIS:
                    delay(1000);

                    if (remote_name[0][0] == 'P')//First letter in PLAYSTATION(R)3 Controller ('P') - 0x50
                    {
                        hid_enable_sixaxis();
                        hid_setLedOn(LED1);
                        
                        for (byte i = 15; i < 19; i++)
                            l2capinbuf[i] = 0x7F;//Set the analog joystick values to center position 
                        l2capinbuf[12] = 0x00;//reset the 12 byte, as the program sometimes read it as a button has been pressed
                        
                        delay(1000);//There has to be a delay before data can be read 
                        printProgStr(Dualshock_Enabled_str);     
                        PS3BTConnected = true;
                    }
                    else if (remote_name[0][0] == 'N')//First letter in Navigation Controller ('N') - 0x4E
                    {
                        hid_enable_sixaxis();
                        hid_setLedOn(LED1);//This just turns LED constantly on, on the Navigation controller
                        
                        for (byte i = 15; i < 19; i++)
                            l2capinbuf[i] = 0x7F;//Set the analog joystick values to center
                        l2capinbuf[12] = 0x00;//reset the 12 byte, as the program sometimes read it as a button has been pressed
                        
                        delay(1000);//There has to be a delay before data can be read
                        printProgStr(Navigation_Enabled_str);                        
                        PS3NavigationBTConnected = true;                                                
                    }
                    else if (remote_name[0][0] == 'M')//First letter in Motion Controller ('M') - 0x4D
                    {                                                                        
                        hid_MoveSetBulb(Red);
                        delay(100);
                        hid_MoveSetBulb(Green);
                        delay(100);
                        hid_MoveSetBulb(Blue);
                        delay(100);

                        hid_MoveSetBulb(Yellow);
                        delay(100);
                        hid_MoveSetBulb(Lightblue);
                        delay(100);
                        hid_MoveSetBulb(Purble);
                        delay(100);

                        hid_MoveSetBulb(White);
                        delay(100);
                        hid_MoveSetBulb(Off);                        
                        
                        l2capinbuf[12] = 0x00;//reset the 12 byte, as the program sometimes read it as the PS_Move button has been pressed
                        
                        delay(1000);//There has to be a delay before data can be read
                        printProgStr(Motion_Enabled_str);
                        PS3MoveBTConnected = true;
                        
                        timerBulbRumble = millis();
                    }                    
                    l2cap_state = L2CAP_EV_L2CAP_DONE;                    
                    break;

                case L2CAP_EV_L2CAP_DONE:
                    if (PS3MoveBTConnected)//The Bulb and rumble values, has to be send at aproximatly every 5th second for it to stay on
                    {
                        dtimeBulbRumble = millis() - timerBulbRumble;
                        if (dtimeBulbRumble > 4000)//Send at least every 4th second
                        {
                            HIDMove_Command(HIDMoveBuffer, HIDMoveBufferSize);//The Bulb and rumble values, has to be written again and again, for it to stay turned on
                            timerBulbRumble = millis();
                        }
                    }
                    break;

                case L2CAP_EV_INTERRUPT_DISCONNECT:
                    if (l2cap_interrupt_disconnect_response)
                    {
                        printProgStr(Interrupt_Disconnected_str);
                        identifier++;
                        l2cap_disconnection_request(identifier, control_dcid, control_scid);                                                
                        l2cap_state = L2CAP_EV_CONTROL_DISCONNECT;
                    }
                    break;

                case L2CAP_EV_CONTROL_DISCONNECT:
                    if (l2cap_control_disconnect_response)
                    {
                        printProgStr(Control_Disconnected_str);
                        hci_disconnect();
                        l2cap_state = L2CAP_EV_L2CAP_DONE;
                        hci_state = HCI_DISCONNECT_STATE;
                    }
                    break;
            }
        }
        
/************************************************************/
/*             HID Report (HCI ACL Packet)                  */
/************************************************************/
void readReport()
{                    
  if((unsigned char)l2capinbuf[8] == 0xA1)//HID_THDR_DATA_INPUT  
  {
    if(PS3BTConnected || PS3NavigationBTConnected)
      ButtonState = (unsigned long)((unsigned char)l2capinbuf[11] | ((unsigned int)((unsigned char)l2capinbuf[12]) << 8) | ((unsigned long)((unsigned char)l2capinbuf[13]) << 16));
    else if(PS3MoveBTConnected)
      ButtonState = (unsigned long)((unsigned char)l2capinbuf[10] | ((unsigned int)((unsigned char)l2capinbuf[11]) << 8) | ((unsigned long)((unsigned char)l2capinbuf[12]) << 16));
      
    //Serial.println(ButtonState,HEX);

    if(ButtonState != OldButtonState)
      ButtonChanged = true;    
    else
      ButtonChanged = false;

    OldButtonState = ButtonState; 
  }
}  

void printReport()//Uncomment for debugging
{                    
  if((unsigned char)l2capinbuf[8] == 0xA1)//HID_THDR_DATA_INPUT  
  {
    for(int i = 10; i < 58;i++)
    {
      if((unsigned char)l2capinbuf[i] < 16) 
        Serial.print("0");   
      Serial.print((unsigned char)l2capinbuf[i],HEX);
      Serial.print(" ");
    }             
    Serial.println("");
  }
}        

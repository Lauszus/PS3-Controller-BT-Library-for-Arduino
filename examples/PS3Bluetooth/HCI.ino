void HCI_event_task()
{
  byte rcode = 0;  //return code
  /*
  char char_left;
  char result_pointer;
  char buf_offset;
  */
  /* check the event pipe*/
  rcode = Usb.inTransfer(BT_ADDR, ep_record[ EVENT_PIPE ].epAddr, MAX_BUFFER_SIZE, hcibuf, USB_NAK_NOWAIT); // input on endpoint 1
  if ( !rcode)
  {
    switch (hcibuf[0]){//switch on event type                
    case EV_COMMAND_COMPLETE:
      hci_command_packets = hcibuf[2]; // update flow control
      hci_event_flag |= HCI_FLAG_CMD_COMPLETE; // set command complete flag

      // process parameters if any

      if((hcibuf[3] == 0x09) && (hcibuf[4] == 0x10)){  // parameters from read local bluetooth address
        for (char i = 0; i < 6; i++) 
          my_bdaddr[i] = (unsigned char) hcibuf[6 + i];
      }

      if((hcibuf[3] == 0x05) && (hcibuf[4] == 0x10)){  //parameters from read buffer size
        acl_data_packet_length = (unsigned char) hcibuf[6] | (unsigned char) hcibuf[7] << 8; 
        sync_data_packet_length =(unsigned char) hcibuf[8];
        acl_data_packets= (unsigned char) hcibuf[9]  | (unsigned char) hcibuf[10] << 8; 
        sync_data_packets =(unsigned char) hcibuf[11] | (unsigned char) hcibuf[12] << 8;  
      }
      if((hcibuf[3] == 0x01) && (hcibuf[4] == 0x10)){  // parameters from read local version
        hci_version = (unsigned char) hcibuf[6]; 
        hci_revision =(unsigned char) hcibuf[7] | (unsigned char) hcibuf[8] << 8; 
        lmp_version= (unsigned char) hcibuf[9]; 
        manufacturer_id = (unsigned char) hcibuf[10] | (unsigned char) hcibuf[11] << 8;  
        lmp_subversion =(unsigned char) hcibuf[12] | (unsigned char) hcibuf[13] << 8;  
      }

      if((hcibuf[3] == 0x14) && (hcibuf[4] == 0x0c)){ // parameters from read local name
        for (char i = 0; i <20; i++){ // save first 20 bytes
          local_name[i] = (unsigned char) hcibuf[6 + i];         
        }

        for(char i = 0; i < 3; i++){ // ignore rest
          Usb.inTransfer(BT_ADDR, ep_record[ EVENT_PIPE ].epAddr, MAX_BUFFER_SIZE, hcibuf, USB_NAK_NOWAIT);
        }

      }
      break;

    case EV_COMMAND_STATUS:

      hci_command_packets = hcibuf[3]; // update flow control
      hci_event_flag |= HCI_FLAG_CMD_STATUS; //set status flag
      if(hcibuf[2]){    // show status on serial if not OK  
        printProgStr(HCI_Command_Failed_str);
        Serial.print(hcibuf[2],HEX);
        Serial.print(' ');
        Serial.print(hcibuf[4],HEX);
        Serial.print(' ');
        Serial.print(hcibuf[5],HEX);

      }
      break;

    case EV_CONNECT_COMPLETE:

      hci_event_flag |= HCI_FLAG_CONN_COMPLETE; // set connection complete flag
      if (!hcibuf[2]){ // check if connected OK
        hci_handle = (unsigned char) hcibuf[3] | (unsigned char) hcibuf[4] << 8; //store the handle for the ACL connection
        hci_event_flag |= HCI_FLAG_CONNECT_OK; //set connection OK flag
      }
      break;


    case EV_DISCONNECT_COMPLETE:

      hci_event_flag |= HCI_FLAG_DISCONN_COMPLETE; //set disconnect commend complete flag
      if (!hcibuf[2]){ // check if disconnected OK
        hci_event_flag &= ~(HCI_FLAG_CONNECT_OK); //clear connection OK flag
      }
      break;

    case EV_NUM_COMPLETE_PKT:
      acl_outstanding_pkt -= (unsigned char) hcibuf[6] | (unsigned char) hcibuf[7] << 8;
      break;  

    case EV_REMOTE_NAME_COMPLETE:

      for (char i = 0; i < 30; i++){
        remote_name[remote_name_entry][i] = (unsigned char) hcibuf[9 + i];  //store first 30 bytes  
      }
      for(char i = 0; i < 4; i++){ // discard additional bytes
        Usb.inTransfer(BT_ADDR, ep_record[ EVENT_PIPE ].epAddr, MAX_BUFFER_SIZE, hcibuf, USB_NAK_NOWAIT);
      }
      hci_event_flag |=HCI_FLAG_REMOTE_NAME_COMPLETE;
      break;

    case EV_INCOMING_CONNECT:
      disc_bdaddr[0][0] = (unsigned char) hcibuf[2];
      disc_bdaddr[0][1] = (unsigned char) hcibuf[3];
      disc_bdaddr[0][2] = (unsigned char) hcibuf[4];
      disc_bdaddr[0][3] = (unsigned char) hcibuf[5];
      disc_bdaddr[0][4] = (unsigned char) hcibuf[6];
      disc_bdaddr[0][5] = (unsigned char) hcibuf[7];
      disc_class[0][0] = (unsigned char) hcibuf[8];
      disc_class[0][1] = (unsigned char) hcibuf[9];
      disc_class[0][2] = (unsigned char) hcibuf[10];
      dev_link_type = (unsigned char) hcibuf[11];
      hci_event_flag |=HCI_FLAG_INCOMING_REQUEST;
      break;

    case EV_ROLE_CHANGED:
      dev_role = (unsigned char)hcibuf[9];
      //Serial.println("Role Changed");
      break;
    default:
      if(hcibuf[0] != 0x00)
      {
        printProgStr(Unmanaged_Event_str);
        Serial.println(hcibuf[0],HEX);
      }
      break;

    }  // switch    
  }
  HCI_task();  
}

/* Poll Bluetooth and print result */
void HCI_task()
{
  switch (hci_state){
  case HCI_INIT_STATE:
    hci_counter++;
    if (hci_counter > 10){  // wait until we have looped 10 times to clear any old events
      hci_reset();
      hci_state = HCI_RESET_STATE;
      hci_counter = 0;
    }
    break;

  case HCI_RESET_STATE:
    hci_counter++;
    if (hci_cmd_complete){
      hci_state = HCI_BUFFER_SIZE_STATE;
      printProgStr(HCI_Reset_str);
      hci_read_buffer_size();
    }
    if (hci_counter > 1000) {
      printProgStr(Reset_Error_str);
      hci_state = HCI_INIT_STATE;
      hci_counter = 0;
    }
    break;

  case HCI_BUFFER_SIZE_STATE:
    if (hci_cmd_complete){
      printProgStr(ACL_Length_str);
      Serial.print(acl_data_packet_length, DEC);
      printProgStr(SCO_Length_str);
      Serial.print(sync_data_packet_length, DEC);
      printProgStr(ACL_Number_str);
      Serial.print(acl_data_packets, DEC);
      printProgStr(SCO_Number_str);
      Serial.print(sync_data_packets, DEC);
      hci_state = HCI_LOCAL_VERSION_STATE; 
      hci_read_local_version();
    }
    break;


  case HCI_LOCAL_VERSION_STATE:
    if (hci_cmd_complete){
      printProgStr(HCI_Version_str);
      Serial.print(hci_version, DEC);
      printProgStr(HCI_Revision_str);
      Serial.print(hci_revision, DEC);
      printProgStr(LMP_Version_str);
      Serial.print(lmp_version, DEC);
      printProgStr(Manuf_Id_str);
      Serial.print(manufacturer_id, DEC);
      printProgStr(LMP_Subvers_str);
      Serial.print(lmp_subversion, DEC);
      hci_state = HCI_LOCAL_NAME_STATE; 
      hci_read_local_name();
    }
    break;

  case HCI_LOCAL_NAME_STATE:
    if (hci_cmd_complete){
      printProgStr(Local_Name_str);
      for (char i = 0; i < 20; i++){
        if(local_name[i] == NULL) 
          break;
        Serial.write(local_name[i]);
      }
      hci_state = HCI_BDADDR_STATE;
      hci_read_bdaddr();         
    }
    break; 

  case HCI_BDADDR_STATE:
    if (hci_cmd_complete){
      printProgStr(Local_BDADDR_str);

      for(int i = 5; i>0;i--)
      {
        if(my_bdaddr[i] < 16) 
          Serial.print("0");       
        Serial.print(my_bdaddr[i], HEX); 
        Serial.print(":");
      }      
      if (my_bdaddr[0] < 16) 
        Serial.print("0");
      Serial.print(my_bdaddr[0], HEX);

      hci_state = HCI_SCANNING_STATE;

    }
    break;
  case HCI_SCANNING_STATE:
    printProgStr(Connect_In_str);
    hci_write_scan_enable();
    hci_state = HCI_CONNECT_IN_STATE;
    break;

  case HCI_CONNECT_IN_STATE:
    if(hci_incoming_connect_request)
    {
      printProgStr(In_Request_str);
      remote_name_entry = 0;
      hci_remote_name(remote_name_entry);
      hci_state = HCI_REMOTE_NAME_STATE;        
    }
    break;     

  case HCI_REMOTE_NAME_STATE:
    if(hci_remote_name_complete)
    {
      printProgStr(Remote_Name_str);
      for (char i = 0; i < 30; i++)        
      {
        if(remote_name[remote_name_entry][i] == NULL)
          break;
        Serial.write(remote_name[remote_name_entry][i]);   
      }             

      hci_accept_connection(0);
      hci_state = HCI_CONNECTED_STATE;                                
    }      
    break;

  case HCI_CONNECTED_STATE:
    if (hci_connect_complete)
    {     
      printProgStr(Device_Connected_str); 
      for(int i = 5; i>0;i--)
      {
        if(disc_bdaddr[0][i] < 16) 
          Serial.print("0");       
        Serial.print(disc_bdaddr[0][i], HEX); 
        Serial.print(":");
      }      
      if (disc_bdaddr[0][0] < 16) 
        Serial.print("0");
      Serial.print(disc_bdaddr[0][0], HEX);
      
      hci_write_scan_disable();//Only allow one controller
      hci_state = HCI_DISABLE_SCAN;
    }
    break;

  case HCI_DISABLE_SCAN:
    if (hci_cmd_complete)
    {                            
      printProgStr(Scan_Disabled_str);
      l2cap_state = L2CAP_EV_CONTROL_SETUP;
      hci_state = HCI_DONE_STATE;
    }
    break;

  case HCI_DONE_STATE:
    if (hci_disconnect_complete)
      hci_state = HCI_DISCONNECT_STATE;
    break;

  case HCI_DISCONNECT_STATE:
    if (hci_disconnect_complete)
    {
      printProgStr(Device_Disconnected_str);
      
      for(int i = 5; i>0;i--)
      {
        if(disc_bdaddr[0][i] < 16) 
          Serial.print("0");       
        Serial.print(disc_bdaddr[0][i], HEX); 
        Serial.print(":");
      }      
      if (disc_bdaddr[0][0] < 16) 
        Serial.print("0");
      Serial.print(disc_bdaddr[0][0], HEX);
         
      l2cap_event_flag = 0;//Clear all flags
      hci_event_flag = 0;//Clear all flags 

      //Reset all buffers                        
      for (byte i = 0; i < MAX_BUFFER_SIZE; i++)
        hcibuf[i] = 0;        
      for (byte i = 0; i < MAX_BUFFER_SIZE; i++)
        l2capinbuf[i] = 0;
      for (byte i = 0; i < MAX_BUFFER_SIZE; i++)
        l2capoutbuf[i] = 0;   
        
      for (int i = 0; i < OUTPUT_REPORT_BUFFER_SIZE; i++)
        HIDBuffer[i + 2] = pgm_read_byte(&OUTPUT_REPORT_BUFFER[i]);//First two bytes reserved for report type and ID    
      for (byte i = 2; i < HIDMoveBufferSize; i++)
        HIDMoveBuffer[i] = 0;           

      l2cap_state = L2CAP_EV_WAIT;                        
      hci_state = HCI_SCANNING_STATE;
    }
    break;
  default:
    break;
  }
}





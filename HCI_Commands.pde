/************************************************************/
/*                    HCI Commands                        */
/************************************************************/

//perform HCI Command
void HCI_Command( unsigned int nbytes, char* dataptr ) {
  hci_command_packets--; 
  hci_event_flag &= ~HCI_FLAG_CMD_COMPLETE;
  Usb.ctrlReq( BT_ADDR, ep_record[ CONTROL_PIPE ].epAddr, bmREQ_HCI_OUT, HCI_COMMAND_REQ, 0x00, 0x00 ,0, nbytes, dataptr );
}

void hci_reset(void)
{
  hci_event_flag = 0; // clear all the flags
  hcibuf[0] = 0x03;
  hcibuf[1] = 0x0C;
  hcibuf[2] = 0x00;
  HCI_Command(3 , hcibuf);
}

void hci_connect(char disc_device)
{
  hci_event_flag &= ~(HCI_FLAG_CONN_COMPLETE | HCI_FLAG_CONNECT_OK);
  hcibuf[0] = 0x05; // HCI OCF = 5
  hcibuf[1]= 0x04; // HCI OGF = 1
  hcibuf[2] = 0x0d; // parameter length =13
  hcibuf[3] = disc_bdaddr[disc_device][0]; // 6 octet bluetooth address
  hcibuf[4] = disc_bdaddr[disc_device][1];
  hcibuf[5] = disc_bdaddr[disc_device][2];
  hcibuf[6] = disc_bdaddr[disc_device][3];
  hcibuf[7] = disc_bdaddr[disc_device][4];
  hcibuf[8] = disc_bdaddr[disc_device][5];
  hcibuf[9] = 0x18; // DM1 or DH1 may be used
  hcibuf[10] = 0xcc; // DM3, DH3, DM5, DH5 may be used
  hcibuf[11] = 0x01; // page repetition mode R1
  hcibuf[12] = 0x00; // always 0
  hcibuf[13] = 0x00; // clock offset 
  hcibuf[14] = 0x00;  // 
  hcibuf[15] = 0x00; //  do not allow role switch
  HCI_Command(16 , hcibuf);
}
void hci_read_buffer_size(void)
{  
  hcibuf[0] = 0x05; // HCI OCF = 5
  hcibuf[1] = 0x10; // HCI OGF = 4
  hcibuf[2] = 0x00;
  HCI_Command(3 , hcibuf);
}

void hci_read_local_version(void)
{
  hcibuf[0] = 0x01; // HCI OCF = 1
  hcibuf[1] = 0x10; // HCI OGF = 4
  hcibuf[2] = 0x00;
  HCI_Command(3 , hcibuf);
}


void hci_read_local_name(void)
{  
  hcibuf[0] = 0x14; // HCI OCF = 14
  hcibuf[1] = 0x0c; // HCI OGF = 3
  hcibuf[2] = 0x00;
  HCI_Command(3 , hcibuf);
}

void hci_write_scan_enable(void)
{
  hcibuf[0] = 0x1A; // HCI OCF = 1A
  hcibuf[1] = 0x03 << 2; // HCI OGF = 3
  hcibuf[2] = 0x01;
  hcibuf[3] = 0x03;
  HCI_Command(4 , hcibuf);
}
void hci_write_scan_disable()
{
  hcibuf[0] = 0x1A; // HCI OCF = 1A
  hcibuf[1] = 0x03 << 2; // HCI OGF = 3
  hcibuf[2] = 0x01;// parameter length = 1
  hcibuf[3] = 0x00;// Inquiry Scan disabled. Page Scan disabled.
  HCI_Command(4 , hcibuf);
}
void hci_read_bdaddr(void)
{   
  hcibuf[0] = 0x09; // HCI OCF = 9
  hcibuf[1] = 0x10; // HCI OGF = 4
  hcibuf[2] = 0x00;
  HCI_Command(3 , hcibuf);
}
void hci_accept_connection(char disc_device)
{
  hci_event_flag |= HCI_FLAG_CONNECT_OK;
  hci_event_flag &= ~(HCI_FLAG_INCOMING_REQUEST);

  hcibuf[0] = 0x09; // HCI OCF = 9
  hcibuf[1]= 0x04; // HCI OGF = 1
  hcibuf[2] = 0x07; // parameter length 7
  hcibuf[3] =disc_bdaddr[disc_device][0]; // 6 octet bdaddr
  hcibuf[4] = disc_bdaddr[disc_device][1];
  hcibuf[5] = disc_bdaddr[disc_device][2];
  hcibuf[6] = disc_bdaddr[disc_device][3];
  hcibuf[7] = disc_bdaddr[disc_device][4];
  hcibuf[8] = disc_bdaddr[disc_device][5];
  hcibuf[9] = 0; //switch role to master

  HCI_Command(10 , hcibuf);
}


void hci_remote_name(char disc_device)
{
  hci_event_flag &= ~(HCI_FLAG_REMOTE_NAME_COMPLETE);
  hcibuf[0] = 0x19; // HCI OCF = 19
  hcibuf[1]= 0x04; // HCI OGF = 1
  hcibuf[2] = 0x0A; // parameter length = 10
  hcibuf[3] = disc_bdaddr[disc_device][0]; // 6 octet bdaddr
  hcibuf[4] = disc_bdaddr[disc_device][1];
  hcibuf[5] = disc_bdaddr[disc_device][2];
  hcibuf[6] = disc_bdaddr[disc_device][3];
  hcibuf[7] = disc_bdaddr[disc_device][4];
  hcibuf[8] = disc_bdaddr[disc_device][5];
  hcibuf[9] = disc_mode[disc_device];
  hcibuf[10] = 0x00; // always 0
  hcibuf[11] = 0x00; // offset
  hcibuf[12] = 0x00; 

  HCI_Command(13 , hcibuf);
}                
void hci_disconnect()
{
  hci_event_flag &= ~HCI_FLAG_DISCONN_COMPLETE;
  hcibuf[0] = 0x06; // HCI OCF = 6
  hcibuf[1]= 0x01 << 2; // HCI OGF = 1
  hcibuf[2] = 0x03; // parameter length =3
  hcibuf[3] = (byte)(hci_handle & 0xFF);//connection handle - low byte
  hcibuf[4] = (byte)((hci_handle >> 8) & 0x0F);//connection handle - high byte
  hcibuf[5] = 0x13; // reason

  HCI_Command(6 , hcibuf);
}


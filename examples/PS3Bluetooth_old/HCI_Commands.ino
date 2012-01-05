// used in control endpoint header for HCI Commands
#define bmREQ_HCI_OUT  USB_SETUP_HOST_TO_DEVICE|USB_SETUP_TYPE_CLASS|USB_SETUP_RECIPIENT_DEVICE
#define HCI_COMMAND_REQ    0

/************************************************************/
/*                    HCI Commands                        */
/************************************************************/

//perform HCI Command
void HCI_Command(char* dataptr, unsigned int nbytes) 
{
  hci_command_packets--; 
  hci_event_flag &= ~HCI_FLAG_CMD_COMPLETE;
  Usb.ctrlReq( BT_ADDR, ep_record[ CONTROL_PIPE ].epAddr, bmREQ_HCI_OUT, HCI_COMMAND_REQ, 0x00, 0x00 ,0, nbytes, dataptr );
}

void hci_reset()
{
  hci_event_flag = 0; // clear all the flags
  hcibuf[0] = 0x03; // HCI OCF = 3
  hcibuf[1] = 0x03 << 2; // HCI OGF = 3
  hcibuf[2] = 0x00;
  HCI_Command(hcibuf, 3);
}
void hci_read_buffer_size()
{  
  hcibuf[0] = 0x05; // HCI OCF = 5
  hcibuf[1] = 0x04 << 2; // HCI OGF = 4
  hcibuf[2] = 0x00;
  HCI_Command(hcibuf, 3);
}

void hci_read_local_version()
{
  hcibuf[0] = 0x01; // HCI OCF = 1
  hcibuf[1] = 0x04 << 2; // HCI OGF = 4
  hcibuf[2] = 0x00;
  HCI_Command(hcibuf, 3);
}


void hci_read_local_name()
{  
  hcibuf[0] = 0x14; // HCI OCF = 14
  hcibuf[1] = 0x03 << 2; // HCI OGF = 3
  hcibuf[2] = 0x00;
  HCI_Command(hcibuf, 3);
}

void hci_write_scan_enable()
{
  hcibuf[0] = 0x1A; // HCI OCF = 1A
  hcibuf[1] = 0x03 << 2; // HCI OGF = 3
  hcibuf[2] = 0x01;// parameter length = 1
  hcibuf[3] = 0x02;// Inquiry Scan disabled. Page Scan enabled.
  HCI_Command(hcibuf, 4);
}
void hci_write_scan_disable()
{
  hcibuf[0] = 0x1A; // HCI OCF = 1A
  hcibuf[1] = 0x03 << 2; // HCI OGF = 3
  hcibuf[2] = 0x01;// parameter length = 1
  hcibuf[3] = 0x00;// Inquiry Scan disabled. Page Scan disabled.
  HCI_Command(hcibuf, 4);
}
void hci_read_bdaddr()
{   
  hcibuf[0] = 0x09; // HCI OCF = 9
  hcibuf[1] = 0x04 << 2; // HCI OGF = 4
  hcibuf[2] = 0x00;
  HCI_Command(hcibuf, 3);
}
void hci_accept_connection(char disc_device)
{
  hci_event_flag |= HCI_FLAG_CONNECT_OK;
  hci_event_flag &= ~(HCI_FLAG_INCOMING_REQUEST);

  hcibuf[0] = 0x09; // HCI OCF = 9
  hcibuf[1] = 0x01 << 2; // HCI OGF = 1
  hcibuf[2] = 0x07; // parameter length 7
  hcibuf[3] = disc_bdaddr[disc_device][0]; // 6 octet bdaddr
  hcibuf[4] = disc_bdaddr[disc_device][1];
  hcibuf[5] = disc_bdaddr[disc_device][2];
  hcibuf[6] = disc_bdaddr[disc_device][3];
  hcibuf[7] = disc_bdaddr[disc_device][4];
  hcibuf[8] = disc_bdaddr[disc_device][5];
  hcibuf[9] = 0; //switch role to master

  HCI_Command(hcibuf, 10);
}


void hci_remote_name(char disc_device)
{
  hci_event_flag &= ~(HCI_FLAG_REMOTE_NAME_COMPLETE);
  hcibuf[0] = 0x19; // HCI OCF = 19
  hcibuf[1] = 0x01 << 2; // HCI OGF = 1
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

  HCI_Command(hcibuf, 13);
}                
void hci_disconnect()
{
  hci_event_flag &= ~HCI_FLAG_DISCONN_COMPLETE;
  hcibuf[0] = 0x06; // HCI OCF = 6
  hcibuf[1]= 0x01 << 2; // HCI OGF = 1
  hcibuf[2] = 0x03; // parameter length =3
  hcibuf[3] = (char)(hci_handle & 0xFF);//connection handle - low byte
  hcibuf[4] = (char)((hci_handle >> 8) & 0x0F);//connection handle - high byte
  hcibuf[5] = 0x13; // reason

  HCI_Command(hcibuf, 6);
}


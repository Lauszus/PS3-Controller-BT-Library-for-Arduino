/************************************************************/
/*                    L2CAP Commands                        */
/************************************************************/
void L2CAP_Command(char* data, int length)
{
  char buf[64];
  buf[0] = (char)(hci_handle & 0xff);    // HCI handle with PB,BC flag
  buf[1] = (char)(((hci_handle >> 8) & 0x0f) | 0x20);
  buf[2] = (char)((4 + length) & 0xff);   // HCI ACL total data length
  buf[3] = (char)((4 + length) >> 8);
  buf[4] = (char)(length & 0xff);         // L2CAP header: Length
  buf[5] = (char)(length >> 8);
  buf[6] = 0x01;  // L2CAP header: Channel ID
  buf[7] = 0x00;  // L2CAP Signalling channel over ACL-U logical link

  for (unsigned int i = 0; i < length; i++)//L2CAP C-frame
    buf[8 + i] = data[i];

  Usb.outTransfer(BT_ADDR, ep_record[ DATAOUT_PIPE ].epAddr, (8 + length), buf);
}
void l2cap_connection_response(char rxid, char dcid[], char scid[], byte result)
{            
  l2capoutbuf[0] = L2CAP_CMD_CONNECTION_RESPONSE;// Code
  l2capoutbuf[1] = rxid;// Identifier
  l2capoutbuf[2] = 0x08;// Length
  l2capoutbuf[3] = 0x00;
  l2capoutbuf[4] = dcid[0];// Destination CID
  l2capoutbuf[5] = dcid[1];
  l2capoutbuf[6] = scid[0];// Source CID
  l2capoutbuf[7] = scid[1];
  l2capoutbuf[8] = result;// Result: Pending or Success
  l2capoutbuf[9] = 0x00;
  l2capoutbuf[10] = 0x00;//No further information
  l2capoutbuf[11] = 0x00;

  L2CAP_Command(l2capoutbuf, 12);            
}        
void l2cap_config_request(char rxid, char dcid[])
{            
  l2capoutbuf[0] = L2CAP_CMD_CONFIG_REQUEST;// Code
  l2capoutbuf[1] = rxid;// Identifier
  l2capoutbuf[2] = 0x08;// Length
  l2capoutbuf[3] = 0x00;
  l2capoutbuf[4] = dcid[0];// Destination CID
  l2capoutbuf[5] = dcid[1];
  l2capoutbuf[6] = 0x00;// Flags
  l2capoutbuf[7] = 0x00;
  l2capoutbuf[8] = 0x01;// Config Opt: type = MTU (Maximum Transmission Unit) - Hint
  l2capoutbuf[9] = 0x02;// Config Opt: length            
  l2capoutbuf[10] = 0xFF;// MTU
  l2capoutbuf[11] = 0xFF;

  L2CAP_Command(l2capoutbuf, 12);
}
void l2cap_config_response(char rxid, char scid[])
{            
  l2capoutbuf[0] = L2CAP_CMD_CONFIG_RESPONSE;// Code
  l2capoutbuf[1] = rxid;// Identifier
  l2capoutbuf[2] = 0x0A;// Length
  l2capoutbuf[3] = 0x00;
  l2capoutbuf[4] = scid[0];// Source CID
  l2capoutbuf[5] = scid[1];
  l2capoutbuf[6] = 0x00;// Flag
  l2capoutbuf[7] = 0x00;
  l2capoutbuf[8] = 0x00;// Result
  l2capoutbuf[9] = 0x00;
  l2capoutbuf[10] = 0x01;// Config
  l2capoutbuf[11] = 0x02;
  l2capoutbuf[12] = 0xA0;
  l2capoutbuf[13] = 0x02;

  L2CAP_Command(l2capoutbuf, 14);
}
void l2cap_disconnection_request(char rxid, char dcid[], char scid[])
{
  l2capoutbuf[0] = L2CAP_CMD_DISCONNECT_REQUEST;// Code
  l2capoutbuf[1] = rxid;// Identifier
  l2capoutbuf[2] = 0x04;// Length
  l2capoutbuf[3] = 0x00;
  l2capoutbuf[4] = scid[0];// Really Destination CID
  l2capoutbuf[5] = scid[1];
  l2capoutbuf[6] = dcid[0];// Really Source CID
  l2capoutbuf[7] = dcid[1];
  L2CAP_Command(l2capoutbuf, 8);
}
void l2cap_disconnection_response(char rxid, char dcid[], char scid[])
{
  l2capoutbuf[0] = L2CAP_CMD_DISCONNECT_RESPONSE;// Code
  l2capoutbuf[1] = rxid;// Identifier
  l2capoutbuf[2] = 0x04;// Length
  l2capoutbuf[3] = 0x00;
  l2capoutbuf[4] = scid[0];// Really Destination CID
  l2capoutbuf[5] = scid[1];
  l2capoutbuf[6] = dcid[0];// Really Source CID
  l2capoutbuf[7] = dcid[1];
  L2CAP_Command(l2capoutbuf, 8);
}

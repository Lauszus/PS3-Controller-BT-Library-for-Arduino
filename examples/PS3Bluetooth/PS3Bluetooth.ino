#include <Usb.h>
#include <MemoryFree.h>
#include <avr/pgmspace.h>

#include "enums.h"//Enums to set the LED, Color of the Motions controller etc.
#include "progmemConstants.h"

/*The application will work in reduced host mode, so we can save program and data
 memory space. After verifying the PID and VID we will use known values for the 
 configuration values for device, interface, endpoints and HID */

/* CSR Bluetooth data taken from descriptors */
#define BT_ADDR        1
#define CSR_VID_LO      0x12  // CSR VID
#define CSR_VID_HI      0x0a
#define CSR_PID_LO      0x01  // Bluetooth HCI Device
#define CSR_PID_HI      0x00
#define BT_CONFIGURATION 1
#define BT_INTERFACE    0 // Only use interface 0
#define BT_NUM_EP      4 // 4 endpoints
#define INT_MAXPKTSIZE   16
#define BULK_MAXPKTSIZE  64
#define EP_INTERRUPT    0x03 // endpoint types
#define EP_BULK         0x02
#define EP_POLL         0x01 // interrupt poll interval

#define CONTROL_PIPE      0 // names we give to the 4 pipes
#define EVENT_PIPE        1
#define DATAIN_PIPE       2
#define DATAOUT_PIPE      3

#define MAX_BUFFER_SIZE   64 // size of general purpose data buffer

/* Bluetooth HCI states for hci_task() */
#define HCI_INIT_STATE 0
#define HCI_RESET_STATE 1
#define HCI_BUFFER_SIZE_STATE 2
#define HCI_LOCAL_VERSION_STATE 3
#define HCI_LOCAL_NAME_STATE 4
#define HCI_BDADDR_STATE 5
#define HCI_SCANNING_STATE 6
#define HCI_CONNECT_IN_STATE 7
#define HCI_REMOTE_NAME_STATE 8
#define HCI_CONNECTED_STATE 9
#define HCI_DISABLE_SCAN 10
#define HCI_DONE_STATE 11
#define HCI_DISCONNECT_STATE 12

/* variables used by high level HCI task */
unsigned char hci_state;  //current state of bluetooth hci connection
unsigned int  hci_counter; // counter used for bluetooth hci loops
unsigned char remote_name_entry;

/* HCI event flags*/
#define HCI_FLAG_CMD_COMPLETE 0x01
#define HCI_FLAG_CMD_STATUS 0x02
#define HCI_FLAG_CONN_COMPLETE 0x04
#define HCI_FLAG_DISCONN_COMPLETE 0x08
#define HCI_FLAG_CONNECT_OK 0x10
#define HCI_FLAG_INQUIRY_COMPLETE 0x20
#define HCI_FLAG_REMOTE_NAME_COMPLETE 0x40
#define HCI_FLAG_INCOMING_REQUEST 0x80

/*Macros for HCI event flag tests */
#define hci_cmd_complete (hci_event_flag & HCI_FLAG_CMD_COMPLETE)
#define hci_cmd_status (hci_event_flag & HCI_FLAG_CMD_STATUS)
#define hci_connect_complete (hci_event_flag & HCI_FLAG_CONN_COMPLETE)
#define hci_disconnect_complete (hci_event_flag & HCI_FLAG_DISCONN_COMPLETE)
#define hci_connect_ok (hci_event_flag & HCI_FLAG_CONNECT_OK)
#define hci_inquiry_complete (hci_event_flag & HCI_FLAG_INQUIRY_COMPLETE)
#define hci_remote_name_complete (hci_event_flag & HCI_FLAG_REMOTE_NAME_COMPLETE)
#define hci_incoming_connect_request (hci_event_flag & HCI_FLAG_INCOMING_REQUEST)
unsigned int  hci_event_flag;// hci flags of received bluetooth events

/* HCI Events managed */
#define EV_COMMAND_COMPLETE  0x0e
#define EV_COMMAND_STATUS    0x0f
#define EV_CONNECT_COMPLETE  0x03
#define EV_DISCONNECT_COMPLETE 0x05
#define EV_NUM_COMPLETE_PKT  0x13
#define EV_INQUIRY_COMPLETE  0x01
#define EV_INQUIRY_RESULT    0x02
#define EV_REMOTE_NAME_COMPLETE  0x07
#define EV_INCOMING_CONNECT  0x04
#define EV_ROLE_CHANGED  0x12

/*variables filled from HCI event management */
char hci_command_packets; //how many packets can host send to controller
int  hci_handle;
unsigned int acl_outstanding_pkt;
unsigned int acl_data_packet_length;
unsigned char sync_data_packet_length;
unsigned int acl_data_packets;
unsigned int sync_data_packets;
unsigned char hci_version;
unsigned int hci_revision;
unsigned char lmp_version;
unsigned int manufacturer_id;
unsigned int lmp_subversion;
unsigned char my_bdaddr[6]; // bluetooth address stored least significant byte first
unsigned char local_name[20];  // first 20 chars of name
unsigned char disc_bdaddr[3][6]; // maximum of three discovered devices
unsigned char disc_class[3][3];
unsigned char disc_mode[3];
unsigned char remote_name[3][30]; // first 20 chars of name
unsigned char dev_link_type;
unsigned char dev_role;
  

/* Bluetooth L2CAP states for L2CAP_task() */
#define L2CAP_EV_WAIT 0
#define L2CAP_EV_CONTROL_SETUP 1
#define L2CAP_EV_CONTROL_REQUEST 2
#define L2CAP_EV_CONTROL_SUCCESS 3
#define L2CAP_EV_INTERRUPT_SETUP 4
#define L2CAP_EV_INTERRUPT_REQUEST 5
#define L2CAP_EV_INTERRUPT_SUCCESS 6
#define L2CAP_EV_HID_ENABLE_SIXAXIS 7
#define L2CAP_EV_L2CAP_DONE 8
#define L2CAP_EV_INTERRUPT_DISCONNECT 9
#define L2CAP_EV_CONTROL_DISCONNECT 10

unsigned char l2cap_state;

// Used For Connection Response - Remember to Include High Byte
const byte PENDING = 0x01;
const byte SUCCESSFUL = 0x00;

/* L2CAP event flags */
#define L2CAP_EV_CONTROL_CONNECTION_REQUEST 0x01
#define L2CAP_EV_CONTROL_CONFIG_REQUEST 0x02
#define L2CAP_EV_CONTROL_CONFIG_SUCCESS 0x04

#define L2CAP_EV_INTERRUPT_CONNECTION_REQUEST 0x08
#define L2CAP_EV_INTERRUPT_CONFIG_REQUEST 0x10
#define L2CAP_EV_INTERRUPT_CONFIG_SUCCESS 0x20

#define L2CAP_EV_CONTROL_DISCONNECT_RESPONSE 0x40
#define L2CAP_EV_INTERRUPT_DISCONNECT_RESPONSE 0x80

/*Macros for L2CAP event flag tests */
#define l2cap_control_connection_reguest (l2cap_event_flag & L2CAP_EV_CONTROL_CONNECTION_REQUEST)
#define l2cap_control_config_reguest (l2cap_event_flag & L2CAP_EV_CONTROL_CONFIG_REQUEST)
#define l2cap_control_config_success (l2cap_event_flag & L2CAP_EV_CONTROL_CONFIG_SUCCESS)

#define l2cap_interrupt_connection_reguest (l2cap_event_flag & L2CAP_EV_INTERRUPT_CONNECTION_REQUEST)
#define l2cap_interrupt_config_reguest (l2cap_event_flag & L2CAP_EV_INTERRUPT_CONFIG_REQUEST)
#define l2cap_interrupt_config_success (l2cap_event_flag & L2CAP_EV_INTERRUPT_CONFIG_SUCCESS)

#define l2cap_control_disconnect_response (l2cap_event_flag & L2CAP_EV_CONTROL_DISCONNECT_RESPONSE)
#define l2cap_interrupt_disconnect_response (l2cap_event_flag & L2CAP_EV_INTERRUPT_DISCONNECT_RESPONSE)
unsigned int l2cap_event_flag;// l2cap flags of received bluetooth events

/* L2CAP signaling commands */
#define L2CAP_CMD_COMMAND_REJECT 0x01
#define L2CAP_CMD_CONNECTION_REQUEST 0x02
#define L2CAP_CMD_CONNECTION_RESPONSE 0x03
#define L2CAP_CMD_CONFIG_REQUEST 0x04
#define L2CAP_CMD_CONFIG_RESPONSE 0x05
#define L2CAP_CMD_DISCONNECT_REQUEST 0x06
#define L2CAP_CMD_DISCONNECT_RESPONSE 0x07

/* L2CAP Channels */
char control_scid[2];// L2CAP source CID for HID_Control                
char control_dcid[2] = { 
  0x40, 0x00 };//0x0040        
char interrupt_scid[2];// L2CAP source CID for HID_Interrupt        
char interrupt_dcid[2] = { 
  0x41, 0x00 };//0x0041
char identifier;//Identifier for connection  

/* Bluetooth L2CAP PSM */
const char L2CAP_PSM_HID_CTRL = 0x11;// HID_Control        
const char L2CAP_PSM_HID_INTR = 0x13;// HID_Interrupt

boolean PS3BTConnected;// Variable used to indicate if the normal playstation controller is successfully connected
boolean PS3MoveBTConnected;// Variable used to indicate if the move controller is successfully connected
boolean PS3NavigationBTConnected;// Variable used to indicate if the navigation controller is successfully connected

boolean ButtonChanged;//Indicate if a button has been pressed
unsigned long ButtonState;
unsigned long OldButtonState;

String outputBT;
String lastOutputBT;
boolean printTemperature;

unsigned long timerHID;// timer used see if there has to be a delay before a new HID command
unsigned long dtimeHID;// delta time since last HID command

unsigned long timerBulbRumble;// used to continuously set PS3 Move controller Bulb and rumble values
unsigned long dtimeBulbRumble;// used to know how longs since last since the Bulb and rumble values was written

EP_RECORD ep_record[ BT_NUM_EP ];  //endpoint record structure for the Bluetooth controller

char hcibuf[ MAX_BUFFER_SIZE ];//General purpose buffer for hci data
char l2capinbuf[ MAX_BUFFER_SIZE ];//General purpose buffer for l2cap in data
char l2capoutbuf[ MAX_BUFFER_SIZE ];//General purpose buffer for l2cap out data

char HIDBuffer[ MAX_BUFFER_SIZE ];// Used to store HID commands
#define HIDMoveBufferSize 50
char HIDMoveBuffer[HIDMoveBufferSize];// Used to store HID commands for the Move controller
/*
void setup();//Needed for it to work in earlier version of the Arduino IDE
void loop();
*/
MAX3421E Max;
USB Usb;

void setup() {
  //Needed for PS3 Dualshock Controller to work
  for (int i = 0; i < OUTPUT_REPORT_BUFFER_SIZE; i++)
    HIDBuffer[i + 2] = pgm_read_byte(&OUTPUT_REPORT_BUFFER[i]);//First two bytes reserved for report type and ID

  HIDBuffer[0] = 0x52;// HID BT Set_report (0x50) | Report Type (Output 0x02)
  HIDBuffer[1] = 0x01;// Report ID

  //Needed for PS3 Move Controller commands to work
  HIDMoveBuffer[0] = 0xA2;// HID BT DATA_request (0xA0) | Report Type (Output 0x02)            
  HIDMoveBuffer[1] = 0x02;// Report ID      
    
  Serial.begin(115200);
  Serial.println("");
  printProgStr(Free_Memory_str);
  Serial.print(freeMemory());
  Max.powerOn();
  delay(200);
}

void loop() {
  Max.Task();
  Usb.Task();
  delay(1);
  if(Usb.getUsbTaskState() == USB_STATE_CONFIGURING)//wait for addressing state 
  {   
    CSR_init();
    Usb.setUsbTaskState( USB_STATE_RUNNING );        
  }
  if(Usb.getUsbTaskState() == USB_STATE_RUNNING)
  {
    HCI_event_task(); //poll the HCI event pipe
    ACL_event_task(); // start polling the ACL input pipe too, though discard data until connected
    
    if(PS3BTConnected || PS3NavigationBTConnected)
    {
      outputBT = "";//Reset string

      if(GetAnalogHat(LeftHatX) > 137 || GetAnalogHat(LeftHatX) < 117)
        outputBT += String(" - LeftHatX: " + String(GetAnalogHat(LeftHatX), DEC));
      if(GetAnalogHat(LeftHatY) > 137 || GetAnalogHat(LeftHatY) < 117)
        outputBT += String(" - LeftHatY: " + String(GetAnalogHat(LeftHatY), DEC));
      if(GetAnalogHat(RightHatX) > 137 || GetAnalogHat(RightHatX) < 117)
        outputBT += String(" - RightHatX: " + String(GetAnalogHat(RightHatX), DEC));
      if(GetAnalogHat(RightHatY) > 137 || GetAnalogHat(RightHatY) < 117)
        outputBT += String(" - RightHatY: " + String(GetAnalogHat(RightHatY), DEC));

      //Analog button values can be read from almost all buttons
      if(GetAnalogButton(L2_ANALOG) > 0)
        outputBT += String(" - L2: " + String(GetAnalogButton(L2_ANALOG), DEC)); 
      if(GetAnalogButton(R2_ANALOG) > 0)
        outputBT += String(" - R2: " + String(GetAnalogButton(R2_ANALOG), DEC)); 

      if(ButtonChanged)     
      {
        //Serial.println("ButtonChanged");
        if(GetButton(PS))
        {
          outputBT += " - PS";
          disconnectController();
        }
        else
        {
          if(GetButton(TRIANGLE))
            outputBT += " - Traingle";
          if(GetButton(CIRCLE))
            outputBT += " - Circle";
          if(GetButton(CROSS))
            outputBT += " - Cross";
          if(GetButton(SQUARE))
            outputBT += " - Square";

          if(GetButton(UP))
            outputBT += " - Up";
          if(GetButton(RIGHT))
            outputBT += " - Right";
          if(GetButton(DOWN))
            outputBT += " - Down";
          if(GetButton(LEFT))
            outputBT += " - Left";

          if(GetButton(L1))
            outputBT += " - L1";  
          //if(GetButton(L2))
          //  outputBT += " - L2";            
          if(GetButton(L3))
            outputBT += " - L3"; 
          if(GetButton(R1))
            outputBT += " - R1";  
          //if(GetButton(R2))
          //  outputBT += " - R2";              
          if(GetButton(R3))
            outputBT += " - R3";

          if(GetButton(SELECT))
            outputBT += " - Select - " + GetStatusString();
          if(GetButton(START))
            outputBT += " - Start";                                
        }                  
      }
      if (outputBT != "" && outputBT != lastOutputBT)//Check if output is not empty and not equal to the last one
        Serial.print("\r\nPS3 Controller" + outputBT);
      lastOutputBT = outputBT;
    }
    else if(PS3MoveBTConnected)
    {
      outputBT = "";//Reset string      

      if(GetAnalogButton(T_MOVE_ANALOG) > 0)
        outputBT += String(" - T: " + String(GetAnalogButton(T_MOVE_ANALOG), DEC)); 

      if(ButtonChanged)     
      {
        //Serial.println("ButtonChanged");
        if(GetButton(PS_MOVE))
        {
          outputBT += " - PS";
          disconnectController();
        }
        else
        {
          if (GetButton(SELECT_MOVE))
          {
            outputBT += " - Select";
            printTemperature = false;
          }
          if (GetButton(START_MOVE))
          {
            outputBT += " - Start";
            printTemperature = true;
          }

          if (GetButton(TRIANGLE_MOVE))
          {            
            outputBT += " - Triangle";
            hid_MoveSetBulb(Red);
          }
          if (GetButton(CIRCLE_MOVE))
          {
            outputBT += " - Circle";
            hid_MoveSetBulb(Green);
          }
          if (GetButton(SQUARE_MOVE))
          {
            outputBT += " - Square";
            hid_MoveSetBulb(Blue);
          }
          if (GetButton(CROSS_MOVE))
          {
            outputBT += " - Cross";
            hid_MoveSetBulb(Yellow);
          }

          if (GetButton(MOVE_MOVE))
          {                        
            hid_MoveSetBulb(Off);                        
            outputBT += " - Move";
            outputBT += " - " + GetStatusString();//Print status string
          }
          //if (GetButton(T_MOVE))
            //outputBT += " - T";
        }
      }
      if(printTemperature)
      {
        String templow;
        String temphigh;
        String input = String(GetSensor(tempMove), DEC);
        
        if (input.length() > 3)
        {
          temphigh = input.substring(0, 2);
          templow = input.substring(2);
        }
        else
        {
          temphigh = input.substring(0, 1);
          templow = input.substring(1);
        }
        outputBT += " - Temperature: " + temphigh + "." + templow;
      }
      if (outputBT != "" && outputBT != lastOutputBT)//Check if output is not empty and not equal to the last one
        Serial.print("\r\nPS3 Move Controller" + outputBT);
      lastOutputBT = outputBT;
    }
  }
}
/* Initialize CSR Bluetooth Controller */
void CSR_init( void )
{
  byte rcode = 0;  //return code
  /* Initialize data structures for endpoints of device 1*/
  ep_record[ CONTROL_PIPE ] = *( Usb.getDevTableEntry( 0,0 ));  //copy endpoint 0 parameters
  ep_record[ EVENT_PIPE ].epAddr = 0x01;    // Bluetooth event endpoint
  ep_record[ EVENT_PIPE ].Attr  = EP_INTERRUPT;
  ep_record[ EVENT_PIPE ].MaxPktSize = INT_MAXPKTSIZE;
  ep_record[ EVENT_PIPE ].Interval  = EP_POLL;
  ep_record[ EVENT_PIPE ].sndToggle = bmSNDTOG0;
  ep_record[ EVENT_PIPE ].rcvToggle = bmRCVTOG0;
  ep_record[ DATAIN_PIPE ].epAddr = 0x02;    // Bluetoth data endpoint
  ep_record[ DATAIN_PIPE ].Attr  = EP_BULK;
  ep_record[ DATAIN_PIPE ].MaxPktSize = BULK_MAXPKTSIZE;
  ep_record[ DATAIN_PIPE ].Interval  = 0;
  ep_record[ DATAIN_PIPE ].sndToggle = bmSNDTOG0;
  ep_record[ DATAIN_PIPE ].rcvToggle = bmRCVTOG0;
  ep_record[ DATAOUT_PIPE ].epAddr = 0x02;    // Bluetooth data endpoint
  ep_record[ DATAOUT_PIPE ].Attr  = EP_BULK;
  ep_record[ DATAOUT_PIPE ].MaxPktSize = BULK_MAXPKTSIZE;
  ep_record[ DATAOUT_PIPE ].Interval  = 0;
  ep_record[ DATAOUT_PIPE ].sndToggle = bmSNDTOG0;
  ep_record[ DATAOUT_PIPE ].rcvToggle = bmRCVTOG0;
  Usb.setDevTableEntry( BT_ADDR, ep_record );              //plug kbd.endpoint parameters to devtable

  /* read the device descriptor and check VID and PID*/
  rcode = Usb.getDevDescr( BT_ADDR, ep_record[ CONTROL_PIPE ].epAddr, DEV_DESCR_LEN , hcibuf );
  if( rcode ) {
    printProgStr(Dev_Error_str);
    Serial.print( rcode, HEX );
    while(1);  //stop
  }
  if((hcibuf[ 8 ] != CSR_VID_LO) || (hcibuf[ 9 ] != CSR_VID_HI) || (hcibuf[ 10 ] != CSR_PID_LO) || (hcibuf[ 11 ] != CSR_PID_HI) ) {
    printProgStr(Wrong_Device_str);
    while(1);  //stop   
  }

  /* Configure device */
  rcode = Usb.setConf( BT_ADDR, ep_record[ CONTROL_PIPE ].epAddr, BT_CONFIGURATION );                    
  if( rcode ) {
    printProgStr(Config_Error_str);
    Serial.print( rcode, HEX );
    while(1);  //stop
  }


  hci_state = HCI_INIT_STATE;
  hci_counter = 0;

  l2cap_state = L2CAP_EV_WAIT;

  printProgStr(CSR_Init_str);
  delay(200);

}

// Print a string from Program Memory directly to save RAM 
void printProgStr(const prog_char str[])
{
  char c;
  if(!str) 
    return;
  while((c = pgm_read_byte(str++)))
    Serial.print(c);
}
/*
void test()
{  
  for(int i = 0; i < OUTPUT_REPORT_BUFFER_SIZE;i++)
  {
    Serial.print(" 0x");
    Serial.print(pgm_read_byte(&OUTPUT_REPORT_BUFFER[i]), HEX);
  }  
}
*/
void disconnectController()//Use this void to disconnect any of the controllers
{
  if (PS3BTConnected)
    PS3BTConnected = false;
  else if (PS3MoveBTConnected)
    PS3MoveBTConnected = false;
  else if (PS3NavigationBTConnected)
    PS3NavigationBTConnected = false;

  //First the HID interrupt channel has to be disconencted, then the HID control channel and finally the HCI connection
  l2cap_disconnection_request(0x0A, interrupt_dcid, interrupt_scid);            
  l2cap_state = L2CAP_EV_INTERRUPT_DISCONNECT;
}












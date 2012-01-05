#define OUTPUT_REPORT_BUFFER_SIZE 48

const byte OUTPUT_REPORT_BUFFER[OUTPUT_REPORT_BUFFER_SIZE] PROGMEM = 
{
  0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 
  0xff, 0x27, 0x10, 0x00, 0x32, 
  0xff, 0x27, 0x10, 0x00, 0x32, 
  0xff, 0x27, 0x10, 0x00, 0x32, 
  0xff, 0x27, 0x10, 0x00, 0x32, 
  0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 
};
/* Print strings in Program Memory */
/* HCI Strings */
const char Free_Memory_str[] PROGMEM = "\r\nfreeMemory() reports: "; 
const char Dev_Error_str[] PROGMEM ="\r\nDevice Descriptor Error: ";
const char Wrong_Device_str[] PROGMEM ="\r\nWrong USB Device ID";
const char Config_Error_str[] PROGMEM ="\r\nError Setting Configuration: ";
const char Int_Error_str[] PROGMEM ="\r\nError Setting Interface: ";
const char CSR_Init_str[] PROGMEM ="\r\nCSR Initialized";
const char HCI_Reset_str[] PROGMEM ="\r\nHCI Reset complete";
const char Reset_Error_str[] PROGMEM ="\r\nNo response to HCI Reset ";
const char ACL_Length_str[] PROGMEM ="\r\nACL Data Packet Length: ";
const char SCO_Length_str[] PROGMEM ="\r\nSCO Data Packet Length: ";
const char ACL_Number_str[] PROGMEM ="\r\nTotal ACL Data Packets: ";
const char SCO_Number_str[] PROGMEM ="\r\nTotal SCO Data Packets: ";
const char HCI_Version_str[] PROGMEM ="\r\nHCI Version: ";
const char HCI_Revision_str[] PROGMEM ="\r\nHCI Revision: ";
const char LMP_Version_str[] PROGMEM ="\r\nLMP Version: ";
const char Manuf_Id_str[] PROGMEM ="\r\nManufacturer Id: ";
const char LMP_Subvers_str[] PROGMEM ="\r\nLMP Subversion: ";
const char Local_Name_str[] PROGMEM ="\r\nLocal Name: ";
const char Local_BDADDR_str[] PROGMEM ="\r\nLocal Bluetooth Address: ";
const char Class_str[] PROGMEM ="  Class: ";
const char Mode_str[] PROGMEM ="  Mode: ";
const char Connect_In_str[] PROGMEM = "\r\nWait For Incoming Connection Request";
const char In_Request_str[] PROGMEM = "\r\nIncoming Request";
const char Remote_Name_str[] PROGMEM ="\r\nRemote Name: ";
const char Device_Connected_str[] PROGMEM = "\r\nConnected to Device: ";
const char Device_Disconnected_str[] PROGMEM = "\r\nDisconnected from Device: ";
const char Scan_Disabled_str[] PROGMEM = "\r\nScan Disabled";
const char HCI_Command_Failed_str[] PROGMEM ="\r\nHCI Command Failed: ";
const char Unmanaged_Event_str[] PROGMEM ="\r\nUnmanaged Event: ";

/* L2CAP Strings */
const char Cmd_Reject_str[] PROGMEM ="\r\nL2CAP Command Rejected - Reason: ";
const char Disconnet_Req_Control_str[] PROGMEM ="\r\nDisconnected Request: Disconnected Control";
const char Disconnet_Req_Interrupt_str[] PROGMEM ="\r\nDisconnected Request: Disconnected Interrupt";
const char HID_Control_Connect_Req_str[] PROGMEM ="\r\nHID Control Incoming Connection Request";
const char HID_Control_Config_Req_str[] PROGMEM ="\r\nHID Control Configuration Request";
const char HID_Control_Success_str[] PROGMEM ="\r\nHID Control Successfully Configured";
const char HID_Interrupt_Connect_Req_str[] PROGMEM ="\r\nHID Interrupt Incoming Connection Request";
const char HID_Interrupt_Config_Req_str[] PROGMEM ="\r\nHID Interrupt Configuration Request";
const char HID_Interrupt_Success_str[] PROGMEM ="\r\nHID Interrupt Successfully Configured";
const char Dualshock_Enabled_str[] PROGMEM ="\r\nDualshock 3 Controller Enabled";
const char Navigation_Enabled_str[] PROGMEM ="\r\nNavigation Controller Enabled";
const char Motion_Enabled_str[] PROGMEM ="\r\nMotion Controller Enabled";
const char Interrupt_Disconnected_str[] PROGMEM ="\r\nDisconnected Interrupt Channel";
const char Control_Disconnected_str[] PROGMEM ="\r\nDisconnected Control Channel";

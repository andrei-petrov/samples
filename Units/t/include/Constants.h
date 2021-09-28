//--------------------------------------------------------------------------------
//  Copyright: (C) 2008-2009 by Andrey Petrov. All rights reserved
//--------------------------------------------------------------------------------
//  Unit:        T
//  Cooperation: absent
//  Date:        2008-11-05
//--------------------------------------------------------------------------------
//  Description: 
//--------------------------------------------------------------------------------


#pragma once

#include "CMN/include/Constants.h"
// Here is all ID_ for in T module must be declared

/////////////////////////////////////////////////////
//
//  DIALOGS
//

#define IDD_T_MESSAGE_BOX   1100


//////////////////////////////////////////////////////////
//
//  ERRORS
//

#define IDE_T_MC_EXTENDED_ERROR         1100 // "hr = 0x%X\r\nInterface: %s\r\n"
#define IDE_T_MC_ERROR                  1101 // " "

#define IDE_T_COM_EXTENDED_ERROR        1102 // "hr = 0x%X\r\n"
#define IDE_T_COM_ERROR                 1103 // " "

#define IDE_T_CONNECTION_ADVISE_FAILED  1105 // "Cannot connect events notification sink"
#define IDE_T_LOGFILE_NOT_OPENED        1106 //

#define IDE_T_ERROR_LOADING_MODULE      1110 // 
#define IDE_T_ENTRYPOINT_NOT_FOUND      1111 // 
#define IDE_T_DELAYLOAD_FAILED          1112            
#define IDE_T_UNSUPPORTED_SCHEMA        1118
#define IDE_T_LDAP_EXT_ERROR            1122 // "%s returned error %x"
#define	IDE_T_SQLSCRIPTFILE_OPEN_ERROR  1123 


/////////////////////////////////////////////////////
//
//  STRINGS
//

// TMessageBox string constants
#define IDS_T_OK                18100 // "&OK"
#define IDS_T_CANCEL            18101 // "&Cancel"
#define IDS_T_YES               18102 // "&Yes"
#define IDS_T_NO                18103 // "&No"
#define IDS_T_RETRY             18104 // "&Retry"
#define IDS_T_ABORT             18105 // "&Abort"
#define IDS_T_IGNORE            18106 // "&Ignore"
#define IDS_T_COPYTOCLIPBOARD   18107 // "&Copy"
#define IDS_T_CLOSE             18108 // "&Close"
#define IDS_T_HELP              18109 // "&Help"
#define IDS_T_ERROR             18110 // "Error"
#define IDS_T_QUESTION          18111 // "Question"
#define IDS_T_WARNING           18112 // "Warning"
#define IDS_T_INFORMATION       18113 // "Information"
#define IDS_T_DO_NOT_SHOW       18114 // "Do not show this message in the future"
#define IDS_T_SUPPRESS          18115 // "Suppress this message durig current operation"
#define IDS_T_RUNREPORTER       18116 // "Run the Error Reporter"

#define IDS_T_BUTTON_MORE       18120 // "More >>"
#define IDS_T_BUTTON_LESS       18121 // "<< Less"

// Exception aux string constants
#define IDS_T_EXCEPTION_MSGPREFIX       18122 // "Message %i: "
#define IDS_T_MC_EXTENDED_UNKNOWN_ERROR 18123 // "Unknown error"
#define IDS_T_MC_UNKNOWN_ERROR          18124 // "Unknown error 0x%X"




/////////////////////////////////////////////////////
//
//  COMMANDS
//

#define ID_T_RUNREPORTER		33100

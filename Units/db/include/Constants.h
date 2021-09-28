//--------------------------------------------------------------------------------
//  Copyright: (C) 2008-2009 by Andrey Petrov. All rights reserved
//--------------------------------------------------------------------------------
//  Unit:        DB
//  Cooperation: absent
//  Date:        2008-11-13
//--------------------------------------------------------------------------------
//  Description: 
//--------------------------------------------------------------------------------
 

#pragma once

#include "CMN/include/Constants.h"

//#define CONNECTION_STRING _T("Driver={SQL Native Client}; Server=%s; Initial Catalog=%s; Trusted_Connection=Yes") 
//#define CONNECTION_STRING _T("Provider=SQLOLEDB;Integrated Security=SSPI;Data Source=%s;Initial Catalog=%s")

// Here is all ID_ for in DB module must be declared

/////////////////////////////////////////////////////
//
//  DOC/VIEW TEMPLATES, SERVICE
//



#define IDR_DB_CREATE_TABLES			       42
#define IDR_DB_INSERT_NEW_SOFT		           43
#define IDR_DB_CREATE_COMPUTER_GROUP           44
#define IDR_DB_CREATE_PUBLISHER                45
#define IDR_DB_SEARCH_OBJECTS                  46
#define IDR_DB_GET_PROPERTIES                  47
#define IDR_DB_CREATE_DB                       48  
#define IDR_DB_DELETE_OBJECT                   49
#define IDR_DB_SEARCH_CHILDREN                 50 
#define IDR_DB_INSERT_NEW_USBDEVICE		       51
#define IDR_DB_ADD_INITIAL_DATA			       52
#define IDR_DB_INSERT_FAILED_INSPECTION	       53
#define IDR_DB_MOVE_COMPUTER                   54
#define IDR_DB_ADD_INITIAL_OBJECTS             55
#define IDR_DB_ADD_SCHEMA_OBJECTS              57
#define IDR_DB_CREATE_MANAGED_OU               58
#define IDR_DB_GET_LANGUAGE                    60
#define IDR_DB_GET_VERSION                     61
#define IDR_DB_SET_VERSION                     62
#define IDR_DB_GET_OBJECTCOUNT                 64
#define IDR_DB_CREATE_SOFTTYPE                 65
#define IDR_DB_GET_REPORT_PARAMS               66  
#define IDR_DB_CREATE_TASK                     67
#define IDR_DB_CREATE_LICENSE_IMAGE            68

//#define IDR_DB_CREATE_ACTION                   68
#define IDR_DB_GET_ROOTDN                      69
#define IDR_DB_SET_ROOTDN                      70
#define IDR_DB_MOVE_OBJECT                     71
#define IDR_DB_SET_ROOTNAME                    72

//#define IDR_DB_GET_SUPERIORS                   72
//#define IDR_DB_GET_INFERIORS                   73
//#define IDR_DB_GET_ATTRIBUTES                  74
#define IDR_DB_GET_FULL_VERSION                75
#define IDR_DB_CREATE_COMP_TASK_LINK           76
#define IDR_DB_DELETE_OLD_ACTIONS              78 
#define IDR_DB_SET_USER_PICTURE                79 
#define IDR_DB_SET_DEFAULT_PICTURE             80
#define IDR_DB_DEFAULT_PICTURE                 81
#define IDR_DB_SP_GET_LANGUAGE                 82
#define IDR_DB_DEMO_PICTURE                    83
#define IDR_DB_IS_COMPUTER_EXIST               84  
#define IDR_DB_IS_OBJECT_LOCKED                85
#define IDR_DB_GET_REP_PARAM_VALUE             86
#define IDR_DB_CREATE_PROHIBITION_RULE         87
#define IDR_DB_CREATE_PASSPORT                 88
#define IDR_DB_CREATE_PASSPORT_LINK            89
#define IDR_DB_CREATE_LICENSERULE_LINK         90

#define IDR_DB_GET_SOFTFORCOMPUTER             91
#define IDR_DB_GET_OSFORCOMPUTER               92
#define IDR_DB_GET_HARDFORCOMPUTER             93
#define IDR_DB_GET_INSPECTEDCOMPUTERS          94
#define IDR_DB_GET_OSLISTFORCOMPUTER           95
#define IDR_DB_GET_CORRECTEDUTCTIMETO          96
#define IDR_DB_GET_HARDWARECHANGES             97
#define IDR_DB_GET_ALLHARDCHANGES              98
#define IDR_DB_GET_ALLSOFTCHANGES              99
#define IDR_DB_GET_PRODUCTNAMEWITHOUTLANGUAGE 102
#define IDR_DB_GET_PRODUCTNAMEGROUP           103 
#define IDR_DB_UPDATE_DBONNEWSYNONYMS         104 
#define IDR_DB_GET_PRODUCTNAMEFROMTEMPLATE    105
#define IDR_DB_INSERT_NEW_REPORTS             106 
#define IDR_DB_GET_REPCOMPWITHSOFTPARAMS      107 
#define IDR_DB_GET_REPCOMPWITHHARDWAREPARAMS  108
#define IDR_DB_CREATELICENSERULE              109
#define IDR_DB_SAVEPASSWORD                   110
#define IDR_DB_RETRIEVEPASSWORD               111
#define IDR_DB_GET_DEVICELIST                 112
#define IDR_DB_GET_REPORTS                    113
#define IDR_DB_CREATE_HARDWARELICENSERULE_LINK     114
#define IDR_DB_GET_PASSPORTNONQUALIFYCOMP     117
#define IDR_DB_GET_PASSPORTLIST               118
#define IDR_DB_GET_WIDESPREADHARDWARE         120
#define IDR_DB_GET_WIDESPREADSOFT             121
#define IDR_DB_GET_CHARGUID                   122
#define IDR_DB_CREATEHARDWARELICENSERULE      123 
#define IDR_DB_GET_SOFTPASSPORTCHECK          125
#define IDR_DB_GET_HARDWAREPASSPORTCHECK      126
#define IDR_DB_GET_NAMEBYGUID                 127
#define IDR_DB_GET_EXTENDDEVICEDESC           128
#define IDR_DB_GET_FREQUENCYFROMPROCESSORNAME 129
#define IDR_DB_CREATE_CATEGORY                130 
#define IDR_DB_CREATE_SYNONYM                 131
#define IDR_DB_CREATE_SUBDIVISION             132
#define IDR_DB_GET_SOFTTYPEGUID               133
#define IDR_DB_GET_SOFTLIST                   134

#define IDR_DB_CREATE_OBJECTRELATIONVIEWS               135
#define IDR_DB_CREATE_VIEWCOMPUTER                      136
#define IDR_DB_CREATE_VIEWCOMPUTERPASSPORTS             137
#define IDR_DB_CREATE_VIEWHARDINSTALLATION              138
#define IDR_DB_CREATE_VIEWHARDWARELICENSERULE           139
#define IDR_DB_CREATE_VIEWINSTALLATION                  140
#define IDR_DB_CREATE_VIEWLICENSERULE                   141
#define IDR_DB_CREATE_VIEWPASSPORTHARDWARELICENSERULES  142
#define IDR_DB_CREATE_VIEWPASSPORTLICENSERULES          143
#define IDR_DB_CREATE_VIEWPROHIBITIONRULE               144
#define IDR_DB_CREATE_VIEWS                             145
#define IDR_DB_CREATE_VIEWSOFT                          146
#define IDR_DB_CREATE_VIEWSOFTTYPE                      147
#define IDR_DB_CREATE_VIEWSYNONYM                       148
#define IDR_DB_CREATE_VIEWTASKCOMPUTERS                 149
#define IDR_DB_CREATE_LIBPRODUCTFILE                    152
#define IDR_DB_CREATE_VIEWLIBPRODUCTFILE                153
#define IDR_DB_CREATE_VIEWFOUNDPRODUCTFILE              154
#define IDR_DB_GET_LIBRARYXML                           156
#define IDR_DB_ADDLIBRARY                               157
#define IDR_DB_CREATE_VIEWFILEINSTALLATION              158
#define IDR_DB_GET_DEFINITEINSPECTIONFILECHECK          159
#define IDR_DB_GET_FILECHECK                            160
#define IDR_DB_GET_MAJORVERSIONFROMFULLVERSION          161

#define IDR_DB_SET_CATEGORYPROPERTIES                   162
#define IDR_DB_SET_COMPUTERPROPERTIES                   163
#define IDR_DB_SET_HARDWARELICENSERULEPROPERTIES        164
#define IDR_DB_SET_LICENSERULEPROPERTIES                165
#define IDR_DB_SET_HARDWAREPROPERTIES                   166
#define IDR_DB_SET_OBJECTROOTPROPERTIES                 167
#define IDR_DB_SET_PASSPORTPROPERTIES                   168
#define IDR_DB_SET_PROHIBITIONRULEPROPERTIES            169
#define IDR_DB_SET_PUBLISHERPROPERTIES                  170
#define IDR_DB_SET_SOFTPROPERTIES                       171
#define IDR_DB_SET_SOFTTYPEPROPERTIES                   172
#define IDR_DB_SET_SUBDIVISIONPROPERTIES                173
#define IDR_DB_SET_SYNONYMPROPERTIES                    174
#define IDR_DB_SET_TASKPROPERTIES                       175

#define IDR_DB_INSERT_NEW_INSPECTIONDATA                176
#define IDR_DB_INSERT_NEW_HARDWARE                      177
#define IDR_DB_INSERT_NEW_FOUNDPRODUCTFILES             178
#define IDR_DB_INSERT_NEW_FILESFOUND                    179 

#define IDR_DB_SET_LIBPRODUCTFILEPROPERTIES             182
#define IDR_DB_CREATE_VIEWMAXCOMPUTERACTION             183
#define IDR_DB_GET_COMPWITHSECURITY                     184
#define IDR_DB_GET_COMPWITHDEFINITESECURITY             185
#define IDR_DB_GET_SECURITYSOFTLIST                     186
#define IDR_DB_GET_DATAFROMOLDDB                        187
#define IDR_DB_GET_LIBRARYFROMOLDDB                     188
#define IDR_DB_GET_COMPUTERSFROMOLDDB                   189
#define IDR_DB_GET_TASKSFROMOLDDB                       190
#define IDR_DB_GET_RULESFROMOLDDB                       191
#define IDR_DB_GET_PASPORTSFROMOLDDB                    192
#define IDR_DB_GET_INSPECTIONSFROMOLDDB                 193
#define IDR_DB_CLEARDB                                  194
#define IDR_DB_INSERT_CHIPSETLIBRARY                    195         
#define IDR_DB_ADD_CHIPSETLIBRARY                       196
#define IDR_DB_CREATE_VIEWLASTCOMPUTERINSP              197 
#define IDR_DB_CREATE_VIEWLASTSUCCESSCOMPUTERINSP       198
#define IDR_DB_CREATE_VIEWMAXFILESSEARCHCOMPUTERACTION  199
#define IDR_DB_CREATE_VIEWMAXPROGRAMSSEARCHCOMPUTERACTION 200



/////////////////////////////////////////////////////
//
//  DIALOGS
//



//////////////////////////////////////////////////////////
//
//  ERRORS
//

#define IDE_DB_CANNOT_CONNECT									1600
#define IDE_DB_STORED_PROCEDURE_FAILED							1601
#define IDE_DB_XML_IMPORT_FAILED								1602
#define IDE_DB_FAILINSPECTINFO_FAILED	 						1603
#define IDE_DB_COMMAND_FAILED		     						1604
#define IDE_DB_SEARCHCHILDREN_FAILED     						1605
#define IDE_DB_SETDBVERSION_FAILED	     						1606 
#define IDE_DB_GETOBJECTTYPE_FAILED	     						1607 						
#define IDE_DB_GETCOMPUTERS4TASK_FAILED  						1608 
#define IDE_DB_INSERTPICTURE_FAILED      						1609 
#define IDE_DB_GETCOMPUTERSCOUNT_FAILED  						1610 
#define IDE_DB_GETCREATIONDATE_FAILED    						1611 
#define IDE_DB_ADDLOGIN_FAILED			 						1612 
#define IDE_DB_GETREPPARAMVALUE_FAILED   						1613
#define IDE_DB_GETCONNECTION_TIMEOUT     						1614

#define IDE_DB_GETCOMPUTERSOFT_FAILED    						1615
#define IDE_DB_GETCOMPUTEROS_FAILED      						1616
#define IDE_DB_GETCOMPUTERHARD_FAILED    						1617
#define IDE_DB_GETINSPECTEDCOMPS_FAILED  						1618

#define IDE_DB_EXPORTDATABASE_FAILED     						1619
#define IDE_DB_SETROOTNAME_FAILED		 						1620 
#define IDE_DB_GETLICENSERULEFORPASSPORT_FAILED   				1621
#define IDE_DB_GETHARDWARELICENSERULEFORPASSPORT_FAILED			1622
#define IDE_DB_GETPASSPORTS4COMPUTER_FAILED						1623
#define IDE_DB_CREATELICENSEIMAGE_FAILED						1624 
#define IDE_DB_APPLYSYNONYMS_FAILED								1625 
#define IDE_DB_GRANT_GUEST_PERMISSIONS_FAILED					1626
#define	IDE_DB_SQLBROWSER_FAILED        						1627
#define	IDE_DB_GETIPBYHOSTNAME_FAILED   						1628
#define	IDE_DB_GETHOSTNAME_FAILED       						1629
#define	IDE_DB_GETLOCALSQL_FAILED       						1630
#define	IDE_DB_SERVERENUM_FAILED        						1631
#define	IDE_DB_WSAINIT_FAILED           						1632
#define	IDE_DB_SOCK_CONNECT_ERROR          						1633
#define	IDE_DB_SOCK_DISCONNECT_ERROR           					1634
#define	IDE_DB_SOCK_CREATE_ERROR           						1635
#define	IDE_DB_SOCK_CLOSE_ERROR           						1636
#define	IDE_DB_ADD_USER_FAILED									1637

					//Миграция БД
#define		IDE_DB_GETLIBRARYFROMOLDDB_FAILED     1633
#define		IDE_DB_GETCOMPUTERSFROMOLDDB_FAILED   1634
#define		IDE_DB_GETTASKSFROMOLDDB_FAILED       1635
#define		IDE_DB_GETRULESFROMOLDDB_FAILED       1636
#define		IDE_DB_GETPASSPORTSFROMOLDDB_FAILED   1637
#define		IDE_DB_GETINSPECTIONSFROMOLDDB_FAILED 1638

/////////////////////////////////////////////////////
//
//  STRINGS
//



/////////////////////////////////////////////////////
//
//  COMMANDS
//





////////////////////////////////////////////////////////////////////////////////////
//
//  windows messages 
//

CREATE PROCEDURE [dbo].[AddInitialObjects] 

AS
BEGIN
	
	SET NOCOUNT ON;
	
BEGIN TRY

declare @Message  nvarchar(1000);
declare @AddingDate datetime;
declare @SoftTypeGUID  uniqueidentifier;
declare @CategoryGUID  uniqueidentifier;
declare @PublisherGUID  uniqueidentifier;
declare @ReportsGUID  uniqueidentifier;
declare @ObjectRootGUID  uniqueidentifier;
declare @SchemaRootGUID  uniqueidentifier;
declare @SchemaGUID  uniqueidentifier;
declare @LicenseRuleContainerGUID uniqueidentifier;
declare @HardwareLicenseRuleContainerGUID uniqueidentifier;
declare @ProhibitionRuleContainerGUID uniqueidentifier;
declare @SoftTypeContainerGUID uniqueidentifier;
declare @PublisherContainerGUID uniqueidentifier;
declare @HardwareRuleContainerGUID uniqueidentifier;
declare @CategoryContainerGUID uniqueidentifier;
declare @SoftContainerGUID uniqueidentifier;
declare @SubdivisionContainerGUID uniqueidentifier;
declare @SynonymContainerGUID uniqueidentifier;
declare @Check smallint;
 
BEGIN TRANSACTION    
  
   set @Check = 0;   
   set @AddingDate = getutcdate();

        --------------------------------------------    Adding of objects-containers  --------------------------------------------
        set @ObjectRootGUID = NEWID();
        insert into dbo.ObjectRelations values(@ObjectRootGUID, 'ObjectRoot', 'Сервер', '00000000-0000-0000-0000-000000000000', @AddingDate);  
        if (@@Error <> 0)
        set @Check = -1;
        
        set @SchemaRootGUID = NEWID();
        insert into dbo.ObjectRelations values(@SchemaRootGUID, 'SchemaRoot', 'SchemaRoot', '00000000-0000-0000-0000-000000000000', @AddingDate);  
        if (@@Error <> 0)
        set @Check = -1;

        insert into dbo.ObjectRelations values(NEWID(), 'TaskContainer', 'Задания', @ObjectRootGUID, @AddingDate); 
        if (@@Error <> 0)
        set @Check = -1;
        
        set @SubdivisionContainerGUID = NEWID();
        insert into dbo.ObjectRelations values(@SubdivisionContainerGUID, 'SubdivisionContainer', 'Подразделения', @ObjectRootGUID, @AddingDate); 
        if (@@Error <> 0)
        set @Check = -1;
        
        set @ProhibitionRuleContainerGUID = NEWID();
        insert into dbo.ObjectRelations values(@ProhibitionRuleContainerGUID, 'ProhibitionRuleContainer', 'Правила черного списка ПО', @ObjectRootGUID, @AddingDate); 
        if (@@Error <> 0)
        set @Check = -1;
        
        set @SoftTypeContainerGUID = NEWID();
        insert into dbo.ObjectRelations values(@SoftTypeContainerGUID, 'SoftTypeContainer', 'Группы ПО', @ObjectRootGUID, @AddingDate); 
        if (@@Error <> 0)
        set @Check = -1;
        
        set @HardwareRuleContainerGUID = NEWID();
        insert into dbo.ObjectRelations values(@HardwareRuleContainerGUID, 'HardwareRuleContainer', 'Правила черного списка оборудования', @ObjectRootGUID, @AddingDate); 
        if (@@Error <> 0)
        set @Check = -1;
        
        set @HardwareLicenseRuleContainerGUID = NEWID();
        insert into dbo.ObjectRelations values(@HardwareLicenseRuleContainerGUID, 'HardwareLicenseRuleContainer', 'Правила белого списка оборудования', @ObjectRootGUID, @AddingDate); 
        if (@@Error <> 0)
        set @Check = -1;
        
        set @PublisherContainerGUID = NEWID();
        insert into dbo.ObjectRelations values(@PublisherContainerGUID, 'PublisherContainer', 'Библиотека ПО', @ObjectRootGUID, @AddingDate); 
        if (@@Error <> 0)
        set @Check = -1;
        
        set @LicenseRuleContainerGUID = NEWID();
        insert into dbo.ObjectRelations values(@LicenseRuleContainerGUID, 'LicenseRuleContainer', 'Правила белого списка ПО', @ObjectRootGUID, @AddingDate); 
        if (@@Error <> 0)
        set @Check = -1;

        set @ReportsGUID = NEWID();
        insert into dbo.ObjectRelations values(@ReportsGUID, 'ReportContainer', 'Отчеты', @ObjectRootGUID, @AddingDate);  
        if (@@Error <> 0)
        set @Check = -1;
        
        set @SoftContainerGUID = NEWID();
        insert into dbo.ObjectRelations values(@SoftContainerGUID, 'SoftContainer', 'Общий список ПО', @ObjectRootGUID, @AddingDate);  
        if (@@Error <> 0)
        set @Check = -1;
        
        insert into dbo.ObjectRelations values(NEWID(), 'HardwareContainer', 'Общий список оборудования', @ObjectRootGUID, @AddingDate);  
        if (@@Error <> 0)
        set @Check = -1;
        
        insert into dbo.ObjectRelations values(NEWID(), 'FoundProductFileContainer', 'Общий список компонентов программ', @ObjectRootGUID, @AddingDate);  
        if (@@Error <> 0)
        set @Check = -1;
        
        insert into dbo.ObjectRelations values(NEWID(), 'ComputerContainer', 'Общий список компьютеров', @ObjectRootGUID, @AddingDate);  
        if (@@Error <> 0)
        set @Check = -1;
        
        insert into dbo.ObjectRelations values(NEWID(), 'PassportContainer', 'Паспорта', @ObjectRootGUID, @AddingDate);  
        if (@@Error <> 0)
        set @Check = -1;
        
        set @SchemaGUID = NEWID();
        insert into dbo.ObjectRelations values(@SchemaGUID, 'SchemaContainer', 'Schema', @SchemaRootGUID, @AddingDate);  
        if (@@Error <> 0)
        set @Check = -1;
        
        set @CategoryContainerGUID = NEWID();
        insert into dbo.ObjectRelations values(@CategoryContainerGUID, 'CategoryContainer', 'Категории', @ObjectRootGUID, @AddingDate);  
        if (@@Error <> 0)
        set @Check = -1;
        
        set @SynonymContainerGUID = NEWID();
        insert into dbo.ObjectRelations values(@SynonymContainerGUID, 'SynonymContainer', 'Синонимы', @ObjectRootGUID, @AddingDate);  
        if (@@Error <> 0)
        set @Check = -1;
          
        -----------------------------------------------------------------------------------------------------------------------

        insert into dbo.ObjectRoot values(@ObjectRootGUID, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 'Демонстрационная версия', NULL, 'Издатель неизвестен', 'Название неизвестно', 'Неизвестна', 'Многоязычная', 'Устройство неизвестно', 'Производитель неизвестен', 'Класс неизвестен', 1, NULL, 25, NULL);  
        if (@@Error <> 0)
        set @Check = -1;
        
        -------------------------------- Schema - Containers adding -----------------------------------------------------------        

        insert into dbo.ObjectSchema values(NEWID(), @SchemaGUID, 'TaskContainer', 'ObjectGUID, ObjectType, Name, ParentGUID, CreationDate', 'ObjectRoot', 'Task', 0);  
        if (@@Error <> 0)
        set @Check = -1;

        insert into dbo.ObjectSchema values(NEWID(), @SchemaGUID, 'FoundProductFileContainer',  'ObjectGUID, ObjectType, Name, ParentGUID, CreationDate', 'ObjectRoot', 'FoundProductFile', 0);  
        if (@@Error <> 0)
        set @Check = -1;

        insert into dbo.ObjectSchema values(NEWID(), @SchemaGUID, 'PublisherContainer', 'ObjectGUID, ObjectType, Name, ParentGUID, CreationDate', 'ObjectRoot', 'Publisher', 0);  
        if (@@Error <> 0)
        set @Check = -1;

        insert into dbo.ObjectSchema values(NEWID(), @SchemaGUID, 'SubdivisionContainer', 'ObjectGUID, ObjectType, Name, ParentGUID, CreationDate', 'ObjectRoot', 'Subdivision', 0);  
        if (@@Error <> 0)
        set @Check = -1;

        insert into dbo.ObjectSchema values(NEWID(), @SchemaGUID, 'SoftTypeContainer', 'ObjectGUID, ObjectType, Name, ParentGUID, CreationDate', 'ObjectRoot', 'SoftType', 0);  
        if (@@Error <> 0)
        set @Check = -1;

        insert into dbo.ObjectSchema values(NEWID(), @SchemaGUID, 'PassportContainer', 'ObjectGUID, ObjectType, Name, ParentGUID, CreationDate', 'ObjectRoot', 'Passport', 0);  
        if (@@Error <> 0)
        set @Check = -1;

        insert into dbo.ObjectSchema values(NEWID(), @SchemaGUID, 'LicenseRuleContainer', 'ObjectGUID, ObjectType, Name, ParentGUID, CreationDate', 'ObjectRoot', 'LicenseRule', 0);  
        if (@@Error <> 0)
        set @Check = -1;

        insert into dbo.ObjectSchema values(NEWID(), @SchemaGUID, 'ProhibitionRuleContainer', 'ObjectGUID, ObjectType, Name, ParentGUID, CreationDate', 'ObjectRoot', 'ProhibitionRule', 0);  
        if (@@Error <> 0)
        set @Check = -1;

        insert into dbo.ObjectSchema values(NEWID(), @SchemaGUID, 'HardwareRuleContainer', 'ObjectGUID, ObjectType, Name, ParentGUID, CreationDate', 'ObjectRoot', 'HardwareProhibitionRule', 0);  
        if (@@Error <> 0)
        set @Check = -1;

        insert into dbo.ObjectSchema values(NEWID(), @SchemaGUID, 'HardwareLicenseRuleContainer', 'ObjectGUID, ObjectType, Name, ParentGUID, CreationDate', 'ObjectRoot', 'HardwareLicenseRule', 0);  
        if (@@Error <> 0)
        set @Check = -1;

        insert into dbo.ObjectSchema values(NEWID(), @SchemaGUID, 'ReportContainer', 'ObjectGUID, ObjectType, Name, ParentGUID, CreationDate', 'ReportContainer, ObjectRoot', 'Report, ReportContainer', 0);
        if (@@Error <> 0)
        set @Check = -1;

        insert into dbo.ObjectSchema values(NEWID(), @SchemaGUID, 'SoftContainer', 'ObjectGUID, ObjectType, Name, ParentGUID, CreationDate', 'ObjectRoot', 'Soft', 0);
        if (@@Error <> 0)
        set @Check = -1;

        insert into dbo.ObjectSchema values(NEWID(), @SchemaGUID, 'HardwareContainer', 'ObjectGUID, ObjectType, Name, ParentGUID, CreationDate', 'ObjectRoot', 'Hardware', 0);
        if (@@Error <> 0)
        set @Check = -1;

        insert into dbo.ObjectSchema values(NEWID(), @SchemaGUID, 'SchemaContainer', 'ObjectGUID, ObjectType, Name, ParentGUID, CreationDate', 'SchemaRoot', 'SchemaObject', 0);
        if (@@Error <> 0)
        set @Check = -1;

        insert into dbo.ObjectSchema values(NEWID(), @SchemaGUID, 'ComputerContainer', 'ObjectGUID, ObjectType, Name, ParentGUID, CreationDate', 'ObjectRoot', 'Computer', 0);
        if (@@Error <> 0)
        set @Check = -1;

        insert into dbo.ObjectSchema values(NEWID(), @SchemaGUID, 'PassportSoftContainer', 'ObjectGUID, ObjectType, Name, ParentGUID, CreationDate', 'ObjectRoot', 'PassportSoft', 0);  
        if (@@Error <> 0)
        set @Check = -1;

        insert into dbo.ObjectSchema values(NEWID(), @SchemaGUID, 'PassportHardwareContainer', 'ObjectGUID, ObjectType, Name, ParentGUID, CreationDate', 'ObjectRoot', 'PassportHardware', 0);  
        if (@@Error <> 0)
        set @Check = -1;

        insert into dbo.ObjectSchema values(NEWID(), @SchemaGUID, 'CategoryContainer', 'ObjectGUID, ObjectType, Name, ParentGUID, CreationDate', 'ObjectRoot', 'Category', 0);  
        if (@@Error <> 0)
        set @Check = -1;

        insert into dbo.ObjectSchema values(NEWID(), @SchemaGUID, 'SynonymContainer', 'ObjectGUID, ObjectType, Name, ParentGUID, CreationDate', 'ObjectRoot', 'Synonym', 0);  
        if (@@Error <> 0)
        set @Check = -1;
        
        -------------------------------- TypeObjects adding -----------------------------------------------------------     


        insert into dbo.ObjectSchema values(NEWID(), @SchemaGUID, 'LibProductFile', 'ObjectGUID, ObjectType, ParentGUID,  InitLibFlag,  FileVersion,  OriginalFileName, Company, ProductName, ProductVersion, Language, Size', 'SoftType', '', 1);
        if (@@Error <> 0)
        set @Check = -1;

        insert into dbo.ObjectSchema values(NEWID(), @SchemaGUID, 'USBDevice', 'ObjectGUID, ParentGUID,  ObjectType, DeviceType, Name, Description, SerialNumber,  LastUsing, Status', 'Action', '', 1);
        if (@@Error <> 0)
        set @Check = -1;

        -- virtual:   FileVersion, FileName, OriginalFileName, Company, ProductName, ProductVersion, Language
        insert into dbo.ObjectSchema values(NEWID(), @SchemaGUID, 'FileInstallation', 'ObjectGUID, ObjectType, ParentGUID, FoundProductFileGUID,  FileVersion, FileName, OriginalFileName, Company, ProductName, ProductVersion, Language, FileCheck, Size', 'Action', '', 1);
        if (@@Error <> 0)
        set @Check = -1;

        insert into dbo.ObjectSchema values(NEWID(), @SchemaGUID, 'FoundProductFile', 'ObjectGUID, ObjectType, ParentGUID,  FileVersion, FileName, OriginalFileName, Company, ProductName, ProductVersion, Language, Size, FileCheck', 'FoundProductFileContainer', '', 1);
        if (@@Error <> 0)
        set @Check = -1;

        insert into dbo.ObjectSchema values(NEWID(), @SchemaGUID, 'ObjectRoot', 'ObjectGUID, ObjectType, Name, ParentGUID, CreationDate, Version, DN, DefaultPolicy, TurnOffBackgroundRefresh, RefreshInterval, DoNotAllowExceptions , FileAndPrinterSharing, UserCompanyName, IsUserPicture, DefaultPublisher, DefaultProductName, DefaultVersion, DefaultLanguage, IsReportTitle, SMTPHost, DefaultUnitName, DefaultManufacturer, DefaultClass, Port, CopyCompDescToAD', '', 'ReportContainer, SoftContainer, TaskContainer, SubdivisionContainer, CategoryContainer', 1);
        if (@@Error <> 0)
        set @Check = -1;

        insert into dbo.ObjectSchema values(NEWID(), @SchemaGUID, 'SchemaRoot', 'ObjectGUID, ObjectType, Name, ParentGUID, CreationDate', '', 'SchemaContainer', 0);
        if (@@Error <> 0)
        set @Check = -1;

        insert into dbo.ObjectSchema values(NEWID(), @SchemaGUID, 'Report', 'ObjectGUID, ObjectType, Name, ParentGUID, CreationDate, ReportURL, ParameterList, ReportType', 'ReportContainer', '', 0);
        if (@@Error <> 0)
        set @Check = -1;

        insert into dbo.ObjectSchema values(NEWID(), @SchemaGUID, 'Publisher', 'ObjectGUID, ObjectType, Name, ParentGUID, CreationDate, Website, Phone, Address,  InitLibFlag', 'PublisherContainer', '', 1);
        if (@@Error <> 0)
        set @Check = -1;

        -- CategoryName, Publisher - virtual    
        insert into dbo.ObjectSchema values(NEWID(), @SchemaGUID, 'SoftType', 'ObjectGUID, ObjectType, Name, ParentGUID, CreationDate, ProductName, PublisherGUID, CategoryGUID, CategoryName, Publisher, VersionMajor, InitLibFlag', 'SoftTypeContainer', '', 1);
        if (@@Error <> 0)
        set @Check = -1;

        insert into dbo.ObjectSchema values(NEWID(), @SchemaGUID, 'ComputerLink', 'ObjectGUID, ObjectType, Name, ParentGUID, CreationDate, ComputerGUID', 'Task', '', 0);
        if (@@Error <> 0)
        set @Check = -1;

        insert into dbo.ObjectSchema values(NEWID(), @SchemaGUID, 'PassportLink', 'ObjectGUID, ObjectType, Name, ParentGUID, CreationDate, PassportGUID', 'Computer', '', 0);
        if (@@Error <> 0)
        set @Check = -1;

        insert into dbo.ObjectSchema values(NEWID(), @SchemaGUID, 'LicenseRuleLink', 'ObjectGUID, ObjectType, Name, ParentGUID, CreationDate, LicenseRuleGUID', 'Passport', '', 0);
        if (@@Error <> 0)
        set @Check = -1;

        insert into dbo.ObjectSchema values(NEWID(), @SchemaGUID, 'HardwareLicenseRuleLink', 'ObjectGUID, ObjectType, Name, ParentGUID, CreationDate, HardwareLicenseRuleGUID', 'Passport', '', 0);
        if (@@Error <> 0)
        set @Check = -1;

        --  CategoryName, ProductName, Publisher, PublisherGUID is Virtual
        insert into dbo.ObjectSchema values(NEWID(), @SchemaGUID, 'Soft', 'ObjectGUID, ObjectType, Name, ParentGUID, CreationDate, PublisherGUID,  Publisher, PublisherGUID, VersionMinor, VersionMajor, Localization, ProductName,  IsOperatingSystem, ProductKey, InspectionDate, LanguageID, IsVisible, CategoryGUID, CategoryName, SoftTypeGUID, VersionBuildMinor, VersionBuildMajor', 'SoftContainer', '', 1);
        if (@@Error <> 0)
        set @Check = -1;

        insert into dbo.ObjectSchema values(NEWID(), @SchemaGUID, 'Category', 'ObjectGUID, ObjectType, Name, ParentGUID,  Description, InitLibFlag', 'CategoryContainer', '', 1);
        if (@@Error <> 0)
        set @Check = -1;

        insert into dbo.ObjectSchema values(NEWID(), @SchemaGUID, 'Hardware', 'ObjectGUID, ObjectType, Name, ParentGUID, CreationDate, Class, DeviceDesc, IsVisible, Memory,  Frequency, Quantity, Speed, Version, SKU, WakeupType, BusWidth, BusLength, Socket, Service, Chipset', 'HardwareContainer', '', 1);
        if (@@Error <> 0)
        set @Check = -1;

        insert into dbo.ObjectSchema values(NEWID(), @SchemaGUID, 'ProhibitionRule', 'ObjectGUID, ObjectType, Name, ParentGUID, CreationDate, ProductName, Publisher, ProductVersion, Localization, ProductKey, Description, SoftType', 'ProhibitionRuleContainer', '', 1);
        if (@@Error <> 0)
        set @Check = -1;

		-- [m.fomin] добавил атрибут LicenseExpire
        insert into dbo.ObjectSchema values(NEWID(), @SchemaGUID, 'LicenseRule', 'ObjectGUID, ObjectType, Name, ParentGUID, CreationDate, ProductName, Publisher, ProductVersion, Localization, ProductKey, Description, MaxInstallAmount, SoftType, CreatedForPassportGUID, PassportName, LinkGUID, LicenseExpire', 'LicenseRuleContainer', 'LicenseImage', 1);
        if (@@Error <> 0)
        set @Check = -1;

        insert into dbo.ObjectSchema values(NEWID(), @SchemaGUID, 'HardwareLicenseRule', 'ObjectGUID, ObjectType, Name, ParentGUID, CreationDate, DeviceDesc, Class, Description,  CreatedForPassportGUID, PassportName, LinkGUID, MinMemory, MinFreeMemory, MinFrequency, MinQuantity, MaxMemory, MaxFreeMemory, MaxFrequency, MaxQuantity, MaxDeviceAmount, ReleaseDateFrom, ReleaseDateTo, Version, Service, BusWidth, BusLength, Chipset, Socket', 'HardwareLicenseRuleContainer', '', 1);
        if (@@Error <> 0)
        set @Check = -1;

        insert into dbo.ObjectSchema values(NEWID(), @SchemaGUID, 'Computer', 'ObjectGUID, ObjectType, Name, ParentGUID, CreationDate, Status, Description, LastSuccessInspDate, LastInspectionDate, PassportList, LinkGUID, SubdivisionGUID', 'ComputerContainer', 'Action, PassportLink', 1); 
        if (@@Error <> 0)
        set @Check = -1;

        insert into dbo.ObjectSchema values(NEWID(), @SchemaGUID, 'Passport', 'ObjectGUID, ObjectType, Name, ParentGUID, CreationDate, Description, LinkGUID, PassportContainerGUID', 'PassportContainer', 'HardwareLicenseRuleLink, LicenseRuleLink', 1); 
        if (@@Error <> 0)
        set @Check = -1;

        insert into dbo.ObjectSchema values(NEWID(), @SchemaGUID, 'Task', 'ObjectGUID, ObjectType, Name, ParentGUID, CreationDate, Login, Password, 
                                                          Domain, SearchFileMask, SearchPath, SyncDN, SyncSchedule, InspectionType, 
                                                          InspectionOptions, UseAlternativeCredentials, SearchFilesLimit, Timeout, 
                                                          NextInspectTime, LastInspectTime, InspectionStoringPeriod, MailTo, SendMail,
                                                          SharedFolder, ReportList, ScanWithAgent,  Schedule, ScanUninspectedOnly, 
                                                          SearchFiles, LimitFiles, UseDNSubChildren, ProgramComponentsSearch,
                                                          ProgramComponentsSearchMasks, ProgramComponentsSearchDiskList, 
                                                          ProgramComponentsSearchExcludeSystemDir, ProgramComponentsSearchExcludeHiddenOrSystem,
                                                          ProgramComponentsSearchLastModifiedDate, ScanWithAgentTimeout, AlternativeScan, CheckHeader,
														  IncrementalSearch, SNChanged, SearchFilesModifiedDate, SearchFilesMinSize, SearchFilesMaxSize,
														  IsSearchFilesByModifiedDate, IsSearchFilesByRangeSizes, ProgramComponentsLimitFiles, IsProgramComponentsLimitFiles', 
                                                          'TaskContainer', 'ComputerLink', 1); 
        if (@@Error <> 0)
        set @Check = -1;

        insert into dbo.ObjectSchema values(NEWID(), @SchemaGUID, 'Subdivision', 'ObjectGUID, ObjectType, Name, ParentGUID, CreationDate,  Description', 'SubdivisionContainer', '', 1); 
        if (@@Error <> 0)
        set @Check = -1;

        insert into dbo.ObjectSchema values(NEWID(), @SchemaGUID, 'Action', 'ObjectGUID, ObjectType, ParentGUID, CreationDate, ActionDate,  ActionType, ResultCode, ErrorText, InspectionType, IP, Architecture, Domain, Users, Virtuality, BootIni, IncrementalSearchData, IncrementalSearch, FileSearch, ProgramSearch', 'Computer', 'Installation, HardInstallation, FilesFound, USBDevice', 0);
        if (@@Error <> 0)
        set @Check = -1;

        --  Following attributes can be requested, but realy Installation hasn't them:  Publisher, PublisherGUID,  VersionMinor, VersionMajor, Localization, ProductName,  IsOperatingSystem, ProductKey, CategoryGUID, CategoryName 
        insert into dbo.ObjectSchema values(NEWID(), @SchemaGUID, 'Installation', 'ObjectGUID, ObjectType, SoftGUID, ParentGUID, CreationDate, Publisher, PublisherGUID,  VersionMinor, VersionMajor, Localization, ProductName,  IsOperatingSystem, ProductKey,  SoftInstallDate, PassportCheck, CategoryGUID, CategoryName, SoftTypeGUID, VersionBuildMinor, VersionBuildMajor', 'Action', '', 0);
        if (@@Error <> 0)
        set @Check = -1;

        insert into dbo.ObjectSchema values(NEWID(), @SchemaGUID, 'LicenseImage', 'ObjectGUID, ObjectType, Name, ParentGUID, CreationDate, LicenseType, Description, BinaryData', 'LicenseRule', '', 0);
        if (@@Error <> 0)
        set @Check = -1;

        --  Following attributes can be requested, but realy HardInstallation hasn't them:  Class, DeviceDesc 
        insert into dbo.ObjectSchema values(NEWID(), @SchemaGUID, 'HardInstallation', 'ObjectGUID, ObjectType, HardwareGUID, ParentGUID, CreationDate, Class, DeviceDesc, Mfg, PassportCheck, Memory, FreeMemory, Frequency, Quantity, Speed, SerialNumber, Version, SKU, WakeupType, BusWidth, BusLength, UniversalUniqueID, CurrentUsage, ReleaseDate, MAC, Socket, Service, Chipset', 'Action', '', 0);
        if (@@Error <> 0)
        set @Check = -1;

        insert into dbo.ObjectSchema values(NEWID(), @SchemaGUID, 'FilesFound', 'ObjectGUID, ObjectType, ParentGUID, CreationDate, FileMask, FilePath, Size', 'Action', '', 0);
        if (@@Error <> 0)
        set @Check = -1;

        insert into dbo.ObjectSchema values(NEWID(), @SchemaGUID, 'ObjectSchema', 'ObjectGUID, ObjectType, Name, ParentGUID, CreationDate, Attributes, PossibleSuperiors, PossibleInferiors, IsEdited', 'SchemaContainer', '', 0);
        if (@@Error <> 0)
        set @Check = -1;

        insert into dbo.ObjectSchema values(NEWID(), @SchemaGUID, 'Synonym', 'ObjectGUID, ObjectType, ParentGUID, CreationDate, AttributeName, Template, DisplayName', 'SynonymContainer', '', 1);
        if (@@Error <> 0)
        set @Check = -1;
        

        ------------------------------------------------------------------------------------------------
--		[m.fomin] добавил явное указание полей LicenseRule в интсрукциях insert для того, 
--		чтобы добавление новых полей не вызывало необходимости дописывать новые значения в оператор values

--		white rules    
   
--		Abbyy Lingvo 10.0       
        insert into dbo.LicenseRule ([ObjectGUID],[ParentGUID],[Name],[ProductName],[Publisher],[Localization],[ProductVersion],[ProductKey],[Description],[MaxInstallAmount],[SoftType])
		values(NEWID(), @LicenseRuleContainerGUID, '1C:Предприятие 8.x',  '1C:Предприятие%', '%%', '%%', '8.%', '%%', 'Правило для примера', 0, 0);  
		if (@@Error <> 0)
        set @Check = -1;                 

--		Abbyy Lingvo 10.0       
        insert into dbo.LicenseRule ([ObjectGUID],[ParentGUID],[Name],[ProductName],[Publisher],[Localization],[ProductVersion],[ProductKey],[Description],[MaxInstallAmount],[SoftType])
		values(NEWID(), @LicenseRuleContainerGUID, 'Abbyy Lingvo 10.0', '%Abbyy%Lingvo%', '%Abbyy%software%', '%%', '10.0', '%%', 'Правило для примера', 0, 0);   
		if (@@Error <> 0)
        set @Check = -1;         

--		Adobe Photoshop любых версий        
        insert into dbo.LicenseRule ([ObjectGUID],[ParentGUID],[Name],[ProductName],[Publisher],[Localization],[ProductVersion],[ProductKey],[Description],[MaxInstallAmount],[SoftType])
		values(NEWID(), @LicenseRuleContainerGUID, 'Adobe Photoshop любых версий', '%Adobe%Photoshop%', '%Adobe%Systems%Incorporated%', '%%', '%%', '%%', 'Правило для примера', 0, 0);  
		if (@@Error <> 0)
        set @Check = -1;         

--		eToken PKI client любой версии          
        insert into dbo.LicenseRule ([ObjectGUID],[ParentGUID],[Name],[ProductName],[Publisher],[Localization],[ProductVersion],[ProductKey],[Description],[MaxInstallAmount],[SoftType])
		values(NEWID(), @LicenseRuleContainerGUID, 'eToken PKI client любой версии',  '%eToken%PKI%client%', 'Aladdin%', '%%', '%%', '%%', 'Правило для примера', 0, 0);  
		if (@@Error <> 0)
        set @Check = -1; 

--		MS Office 2003 русский любой редакции          
        insert into dbo.LicenseRule ([ObjectGUID],[ParentGUID],[Name],[ProductName],[Publisher],[Localization],[ProductVersion],[ProductKey],[Description],[MaxInstallAmount],[SoftType])
		values(NEWID(), @LicenseRuleContainerGUID, 'MS Office 2003 русский любой редакции', 'Microsoft%Office%2003%', '%Microsoft%', '%русский%', '11.0', '%%', 'Правило для примера', 0, 0);   
		if (@@Error <> 0)
        set @Check = -1; 

--		MS SQL Server 2005 любой редакции           
        insert into dbo.LicenseRule ([ObjectGUID],[ParentGUID],[Name],[ProductName],[Publisher],[Localization],[ProductVersion],[ProductKey],[Description],[MaxInstallAmount],[SoftType])
		values(NEWID(), @LicenseRuleContainerGUID, 'MS SQL Server 2005 любой редакции', 'Microsoft%SQL%Server%2005', '%Microsoft%', '%%', '%%', '%%', 'Правило для примера', 0, 0);  
		if (@@Error <> 0)
        set @Check = -1;         

--		Windows 2003 Enterprise sp2 язык любой 
        insert into dbo.LicenseRule ([ObjectGUID],[ParentGUID],[Name],[ProductName],[Publisher],[Localization],[ProductVersion],[ProductKey],[Description],[MaxInstallAmount],[SoftType])
		values(NEWID(), @LicenseRuleContainerGUID, 'Windows 2003 Enterprise sp2 язык любой', '%Windows%2003%Enterprise%Service%Pack%2%', '%Microsoft%', '%%', '5.2', '%%', 'Правило для примера', 0, 1);   
		if (@@Error <> 0)
        set @Check = -1; 

--		Windows 2008 sp2 английский   
        insert into dbo.LicenseRule ([ObjectGUID],[ParentGUID],[Name],[ProductName],[Publisher],[Localization],[ProductVersion],[ProductKey],[Description],[MaxInstallAmount],[SoftType])
		values(NEWID(), @LicenseRuleContainerGUID, 'Windows 2008 sp2 английский', '%Windows%2008%Service%Pack%2%', '%Microsoft%', '%английский%', '6.0', '%%', 'Правило для примера', 0, 1);  
		if (@@Error <> 0)
        set @Check = -1; 

--		Windows 7 Ultimate            
        insert into dbo.LicenseRule ([ObjectGUID],[ParentGUID],[Name],[ProductName],[Publisher],[Localization],[ProductVersion],[ProductKey],[Description],[MaxInstallAmount],[SoftType])
		values(NEWID(), @LicenseRuleContainerGUID, 'Windows 7 Ultimate',  '%Windows%7%Ultimate%', '%Microsoft%', '%%', '6.1', '%%', 'Правило для примера', 0, 1);    
		if (@@Error <> 0)
        set @Check = -1; 

--		Windows Vista и старше с ключом           
        insert into dbo.LicenseRule ([ObjectGUID],[ParentGUID],[Name],[ProductName],[Publisher],[Localization],[ProductVersion],[ProductKey],[Description],[MaxInstallAmount],[SoftType])
		values(NEWID(), @LicenseRuleContainerGUID, 'Windows Vista и старше с ключом',  '%Windows%', '%Microsoft%', '%%', '6.%', '11111-22222-33333-44444-55555', 'Правило для примера', 0, 1);  
		if (@@Error <> 0)
        set @Check = -1; 

--		Windows XP любой редакции и языка         
        insert into dbo.LicenseRule ([ObjectGUID],[ParentGUID],[Name],[ProductName],[Publisher],[Localization],[ProductVersion],[ProductKey],[Description],[MaxInstallAmount],[SoftType])
		values(NEWID(), @LicenseRuleContainerGUID, 'Windows XP любой редакции и языка',  '%Windows%XP%', '%Microsoft%', '%%', '5.1', '%%', 'Правило для примера', 0, 1);          
		if (@@Error <> 0)
        set @Check = -1; 
        
         ------------------------------------------------------------------------------------------------

--		hardware white rules    
   
--		Батарея ИБП HID      
        insert into dbo.HardwareLicenseRule values(NEWID(), @HardwareLicenseRuleContainerGUID, 'Наличие UPS',   'BATTERY', '%%', 'Правило для примера', 0, 0, 0, 0, 0, 0, 0, 0, NULL, 0, '%%', '%%', '%%', '%%', '%%', '%%', NULL, NULL);  
        if (@@Error <> 0)
        set @Check = -1;    
        
--		Диск     
        insert into dbo.HardwareLicenseRule values(NEWID(), @HardwareLicenseRuleContainerGUID, 'Диски от 200 до 300 Гб', 'DISKDRIVE', '%%', 'Правило для примера', 204800, 307200, 0, 0, 0, 0, 0, 0, NULL, 0, '%%', '%%', '%%', '%%', '%%', '%%', NULL, NULL);  
        if (@@Error <> 0)
        set @Check = -1;   

 --		Processor Duo     
        insert into dbo.HardwareLicenseRule values(NEWID(), @HardwareLicenseRuleContainerGUID, 'Процессоры Core 2 Duo',  'PROCESSOR', '%Duo%', 'Правило для примера', 0, 0, 0, 0, 0, 0, 0, 0, NULL, 0, '%%', '%%', '%%', '%%', '%%', '%%', NULL, NULL);  
        if (@@Error <> 0)
        set @Check = -1; 
        
 --		Processor Frequency       
        insert into dbo.HardwareLicenseRule values(NEWID(), @HardwareLicenseRuleContainerGUID, 'Процессоры от 2400 до 2800 МГц', 'PROCESSOR', '%%', 'Правило для примера', 0, 0, 0, 0, 2400, 2800, 0, 0, NULL, 0, '%%', '%%', '%%', '%%', '%%', '%%', NULL, NULL);  
        if (@@Error <> 0)
        set @Check = -1; 
        
--		MEMORY      
        insert into dbo.HardwareLicenseRule values(NEWID(), @HardwareLicenseRuleContainerGUID, 'Память от 1 до 4 Гб',  'MEMORY', '%%', 'Правило для примера', 1024, 4096, 0, 0, 0, 0, 0, 0, NULL, 0, '%%', '%%', '%%', '%%', '%%', '%%', NULL, NULL);  
        if (@@Error <> 0)
        set @Check = -1; 
        
--		black rules        

--		Counter Strike   
        insert into dbo.ProhibitionRule values(NEWID(), @ProhibitionRuleContainerGUID, 'Counter Strike',  '%Counter%Strike%', '%%', '%%', '%%', '%%', 'Правило для примера', 0);  
        if (@@Error <> 0)
        set @Check = -1; 
--		ICQ                         
        insert into dbo.ProhibitionRule values(NEWID(), @ProhibitionRuleContainerGUID, 'ICQ всех видов', '%ICQ%', '%%', '%%', '%%', '%%', 'Правило для примера', 0);  
        if (@@Error <> 0)
        set @Check = -1; 
--		Подозрительные Windows Vista и старше           
        insert into dbo.ProhibitionRule values(NEWID(), @ProhibitionRuleContainerGUID, 'Подозрительные Windows Vista и старше', '%Windows%', '%Microsoft%', '%%', '6.%', 'BBBBB-BBBBB-BBBBB-BBBBB-BBBBB', 'Правило для примера', 1);  
        if (@@Error <> 0)
        set @Check = -1; 
        
       
 -----------------------------------------------------------------------------------------------                     
       
--		Subdivision 
        insert into dbo.Subdivision values(NEWID(), @SubdivisionContainerGUID, 'Подразделение по умолчанию', '');  
        if (@@Error <> 0)
        set @Check = -1; 
        
        ----------------------------------------------------------------------------------------------
        
-- Undefined category  
		/* set @CategoryGUID = NEWID();   
        insert into dbo.Category values(@CategoryGUID, @CategoryContainerGUID, 'ПО без категории', '', 1);  
        if (@@Error <> 0)
        set @Check = -1; */
        
-- Undefined publisher     
        insert into dbo.Publisher values(NEWID(), @PublisherContainerGUID, 'Издатель неизвестен', '', '', '', 1);  
        if (@@Error <> 0)
        set @Check = -1;
               
          
    insert into dbo.Synonym values(NEWID(), @SynonymContainerGUID,  'Publisher',  '%Adobe System%', 'Adobe Systems Incorporated');
    if (@@Error <> 0)
    set @Check = -1;
     
   insert into dbo.Synonym values(NEWID(), @SynonymContainerGUID, 'Publisher',  '%ABBYY%', 'ABBYY Software');
   if (@@Error <> 0)
   set @Check = -1;
    
   insert into dbo.Synonym values(NEWID(), @SynonymContainerGUID, 'Publisher',  '%ACD System%', 'ACD Systems Ltd.');
   if (@@Error <> 0)
   set @Check = -1;
    
   insert into dbo.Synonym values(NEWID(), @SynonymContainerGUID, 'Publisher',  '%Acresso%', 'Acresso Software Incorporated');
   if (@@Error <> 0)
   set @Check = -1;
     
   insert into dbo.Synonym values(NEWID(), @SynonymContainerGUID, 'Publisher',  '%American Power Conversion%', 'American Power Conversion Corporation');
   if (@@Error <> 0)
   set @Check = -1;
      
   insert into dbo.Synonym values(NEWID(), @SynonymContainerGUID, 'Publisher',  '%Atheros%', 'Atheros Communications Incorporated');
   if (@@Error <> 0)
   set @Check = -1;
 
   insert into dbo.Synonym values(NEWID(), @SynonymContainerGUID, 'Publisher',  '%AVG%', 'AVG Technologies');
   if (@@Error <> 0)
   set @Check = -1;
     
   insert into dbo.Synonym values(NEWID(), @SynonymContainerGUID, 'Publisher',  '%Google%', 'Google Incorporated');
   if (@@Error <> 0)
   set @Check = -1;
      
   insert into dbo.Synonym values(NEWID(), @SynonymContainerGUID, 'Publisher',  '%Intel%', 'Intel Corporation');
   if (@@Error <> 0)
   set @Check = -1;
     
   insert into dbo.Synonym values(NEWID(), @SynonymContainerGUID, 'Publisher',  '%Macrovision%', 'Macrovision Corporation');
   if (@@Error <> 0)
   set @Check = -1;
      
   insert into dbo.Synonym values(NEWID(), @SynonymContainerGUID, 'Publisher',  '%Microsoft Cor%', 'Microsoft Corporation');
   if (@@Error <> 0)
   set @Check = -1;
    
   insert into dbo.Synonym values(NEWID(), @SynonymContainerGUID, 'Publisher',  '%Nokia%', 'Nokia Corporation');
   if (@@Error <> 0)
   set @Check = -1;
      
   insert into dbo.Synonym values(NEWID(), @SynonymContainerGUID, 'Publisher',  '%Oracle%', 'Oracle Corporation');
   if (@@Error <> 0)
   set @Check = -1;
      
   insert into dbo.Synonym values(NEWID(), @SynonymContainerGUID, 'Publisher',  '%Quest Software%', 'Quest Software Incorporated');
   if (@@Error <> 0)
   set @Check = -1;
      
   insert into dbo.Synonym values(NEWID(), @SynonymContainerGUID, 'Publisher',  '%Realtek%', 'Realtek Semiconductor Corporation');
   if (@@Error <> 0)
   set @Check = -1;
     
   insert into dbo.Synonym values(NEWID(), @SynonymContainerGUID, 'Publisher',  '%Sun Microsystem%', 'Sun Microsystems Incorporated');
   if (@@Error <> 0)
   set @Check = -1;
      
   insert into dbo.Synonym values(NEWID(), @SynonymContainerGUID, 'Publisher',  '%Symantec%', 'Symantec Corporation');
   if (@@Error <> 0)
   set @Check = -1;
    
   insert into dbo.Synonym values(NEWID(), @SynonymContainerGUID, 'Publisher',  '%VMware%', 'VMware Incorporated');
   if (@@Error <> 0)
   set @Check = -1;
   
   insert into dbo.Synonym values(NEWID(), @SynonymContainerGUID, 'Publisher',  '%Информзащита%', 'ЗАО НИП «Информзащита»');
   if (@@Error <> 0)
   set @Check = -1;
  
   insert into dbo.Synonym values(NEWID(), @SynonymContainerGUID,  'Publisher', '%TechSmith%', 'TechSmith Corporation');
   if (@@Error <> 0)
   set @Check = -1;
  
   insert into dbo.Synonym values(NEWID(), @SynonymContainerGUID, 'Publisher', 'ICQ %', 'ICQ Incorporated');
   if (@@Error <> 0)
   set @Check = -1;
    
   insert into dbo.Synonym values(NEWID(), @SynonymContainerGUID, 'Publisher', '%NVIDIA%', 'NVIDIA Corporation');
   if (@@Error <> 0)
   set @Check = -1;
    
   insert into dbo.Synonym values(NEWID(), @SynonymContainerGUID, 'Publisher', 'Primavera%', 'Primavera Systems Incorporated');
   if (@@Error <> 0)
   set @Check = -1;
     
   insert into dbo.Synonym values(NEWID(), @SynonymContainerGUID, 'Publisher', 'Seagate%', 'Seagate Technology');
   if (@@Error <> 0)
   set @Check = -1;
      
   insert into dbo.Synonym values(NEWID(), @SynonymContainerGUID, 'Publisher',  '%Apple%', 'Apple Computer Incorporated');
   if (@@Error <> 0)
   set @Check = -1;
     
   insert into dbo.Synonym values(NEWID(), @SynonymContainerGUID, 'Publisher',  '%HTC%', 'HTC Corporation');
   if (@@Error <> 0)
   set @Check = -1;
    
   insert into dbo.Synonym values(NEWID(), @SynonymContainerGUID, 'Publisher',  '%Opera Software%', 'Opera Software');
   if (@@Error <> 0)
   set @Check = -1;
   
   insert into dbo.Synonym values(NEWID(), @SynonymContainerGUID, 'Publisher',  '%Samsung Electronics%', 'Samsung Electronics');
   if (@@Error <> 0)
   set @Check = -1;

   insert into dbo.Synonym values(NEWID(), @SynonymContainerGUID, 'Publisher',  '%Skype Technologies%', 'Skype Technologies');
   if (@@Error <> 0)
   set @Check = -1;
    
   insert into dbo.Synonym values(NEWID(), @SynonymContainerGUID, 'Publisher',  '%SmartSoft %', 'SmartSoft Ltd.');
   if (@@Error <> 0)
   set @Check = -1;
     
   insert into dbo.Synonym values(NEWID(), @SynonymContainerGUID, 'Publisher',  '%Araxis%', 'Araxis Ltd.');
   if (@@Error <> 0)
   set @Check = -1;
     
   insert into dbo.Synonym values(NEWID(), @SynonymContainerGUID, 'Publisher',  'Western Digital%', 'Western Digital Technologies');
   if (@@Error <> 0)
   set @Check = -1;
      
   insert into dbo.Synonym values(NEWID(), @SynonymContainerGUID, 'Publisher',  '%Altova%', 'Altova Incorporated');
   if (@@Error <> 0)
   set @Check = -1;
     
   insert into dbo.Synonym values(NEWID(), @SynonymContainerGUID, 'Publisher',  '%AVG Tech%', 'AVG Technologies');
   if (@@Error <> 0)
   set @Check = -1;
     
   insert into dbo.Synonym values(NEWID(), @SynonymContainerGUID, 'Publisher',  '%Borland%', 'Borland Software Corporation');
   if (@@Error <> 0)
   set @Check = -1;
     
   insert into dbo.Synonym values(NEWID(), @SynonymContainerGUID, 'Publisher',  '%Compuware Corp%', 'Compuware Corporation');
   if (@@Error <> 0)
   set @Check = -1;
     
   insert into dbo.Synonym values(NEWID(), @SynonymContainerGUID, 'Publisher',  '%DivX%', 'DivX Incorporated');
   if (@@Error <> 0)
   set @Check = -1;
   
   insert into dbo.Synonym values(NEWID(), @SynonymContainerGUID, 'Publisher',  '%Logitech%', 'Logitech Incorporated');
   if (@@Error <> 0)
   set @Check = -1;
      
   insert into dbo.Synonym values(NEWID(), @SynonymContainerGUID, 'Publisher',  '%Macromedia%', 'Macromedia Incorporated');
   if (@@Error <> 0)
   set @Check = -1;
      
   insert into dbo.Synonym values(NEWID(), @SynonymContainerGUID, 'Publisher',  '%Nero AG%', 'Nero AG Incorporated');
   if (@@Error <> 0)
   set @Check = -1;
     
   insert into dbo.Synonym values(NEWID(), @SynonymContainerGUID, 'Publisher',  '%Raize Software%', 'Raize Software Incorporated');
   if (@@Error <> 0)
   set @Check = -1;
      
   insert into dbo.Synonym values(NEWID(), @SynonymContainerGUID, 'Publisher',  '%Sybase%', 'Sybase Incorporated');
   if (@@Error <> 0)
   set @Check = -1;
      
   insert into dbo.Synonym values(NEWID(), @SynonymContainerGUID, 'Publisher',  '%TomTom%', 'TomTom International B.V.');
   if (@@Error <> 0)
   set @Check = -1;
      
   insert into dbo.Synonym values(NEWID(), @SynonymContainerGUID, 'Publisher',  '%VanDyke Software%', 'VanDyke Software Incorporated');
   if (@@Error <> 0)
   set @Check = -1;
    
   insert into dbo.Synonym values(NEWID(), @SynonymContainerGUID, 'Publisher',  '%VIA Technologies%', 'VIA Technologies Incorporated');
   if (@@Error <> 0)
   set @Check = -1;
     
   insert into dbo.Synonym values(NEWID(), @SynonymContainerGUID, 'Publisher',  '%XviD%Team%', 'XviD Development Team');
   if (@@Error <> 0)
   set @Check = -1;
   
   insert into dbo.Synonym values(NEWID(), @SynonymContainerGUID, 'Publisher',  '%Xiph.%Org%', 'Xiph. Org');
   if (@@Error <> 0)
   set @Check = -1;
      
   insert into dbo.Synonym values(NEWID(), @SynonymContainerGUID, 'Publisher',  '%WinZip%', 'WinZip Computing Incorporated');
   if (@@Error <> 0)
   set @Check = -1;

   insert into dbo.Synonym values(NEWID(), @SynonymContainerGUID, 'Publisher',  '%Крипто-Про%', 'Компания Крипто-Про');
   if (@@Error <> 0)
   set @Check = -1;
 
   insert into dbo.Synonym values(NEWID(), @SynonymContainerGUID, 'Publisher',  '%Название организации%', 'Название организации');
   if (@@Error <> 0)
   set @Check = -1;
  
   insert into dbo.Synonym values(NEWID(), @SynonymContainerGUID, 'Publisher',  '%Autodesk%', 'Autodesk Incorporated');
   if (@@Error <> 0)
   set @Check = -1;
   
   insert into dbo.Synonym values(NEWID(), @SynonymContainerGUID, 'Publisher',  '%Boson Software%', 'Boson Software Incorporated');
   if (@@Error <> 0)
   set @Check = -1;
   
   insert into dbo.Synonym values(NEWID(), @SynonymContainerGUID, 'Publisher',  '%Cisco Systems%', 'Cisco Systems Incorporated');
   if (@@Error <> 0)
   set @Check = -1;
   
   insert into dbo.Synonym values(NEWID(), @SynonymContainerGUID, 'Publisher',  '%Cisra%', 'Cisra');
   if (@@Error <> 0)
   set @Check = -1;
  
   insert into dbo.Synonym values(NEWID(), @SynonymContainerGUID, 'Publisher',  '%Infosec%', 'Infosec');
   if (@@Error <> 0)
   set @Check = -1;
  
   insert into dbo.Synonym values(NEWID(), @SynonymContainerGUID, 'Publisher',  '%InstallShield Software%', 'InstallShield Software Corporation');
   if (@@Error <> 0)
   set @Check = -1;
  
   insert into dbo.Synonym values(NEWID(), @SynonymContainerGUID, 'Publisher',  '%Canon%', 'Canon');
   if (@@Error <> 0)
   set @Check = -1;
  
   insert into dbo.Synonym values(NEWID(), @SynonymContainerGUID, 'Publisher',  '%activePDF%', 'ActivePDF Incorporated');
   if (@@Error <> 0)
   set @Check = -1;
   
   insert into dbo.Synonym values(NEWID(), @SynonymContainerGUID, 'Publisher',  'ООО%Код Безопасности%', 'Security Code');
   if (@@Error <> 0)
   set @Check = -1;

   insert into dbo.Synonym values(NEWID(), @SynonymContainerGUID, 'Publisher',  'Security Code %', 'Security Code');
   if (@@Error <> 0)
   set @Check = -1;
   
   insert into dbo.Synonym values(NEWID(), @SynonymContainerGUID, 'Publisher',  '%Лаборатория Касперского%', 'Kaspersky Lab');
   if (@@Error <> 0)
   set @Check = -1;
   
   insert into dbo.Synonym values(NEWID(), @SynonymContainerGUID, 'Publisher',  '%ОАО%Инфотекс%', 'Infotecs');
   if (@@Error <> 0)
   set @Check = -1;
   
   insert into dbo.Synonym values(NEWID(), @SynonymContainerGUID, 'Publisher',  'ЗАО%Смарт Лайн Инк%', 'DeviceLock Incorporated');
   if (@@Error <> 0)
   set @Check = -1;
   
   insert into dbo.Synonym values(NEWID(), @SynonymContainerGUID, 'Publisher',  'Aladdin Knowledge Systems%', 'Aladdin');
   if (@@Error <> 0)
   set @Check = -1;
   
   insert into dbo.Synonym values(NEWID(), @SynonymContainerGUID, 'Publisher',  'Aladdin Software Security R.D.', 'Aladdin');
   if (@@Error <> 0)
   set @Check = -1;
   
   insert into dbo.Synonym values(NEWID(), @SynonymContainerGUID, 'Publisher',  'AVAST Software', 'ALWIL Software');
   if (@@Error <> 0)
   set @Check = -1;  
   
   insert into dbo.Synonym values(NEWID(), @SynonymContainerGUID, 'Publisher',  'Check Point Software Ltd.%', 'Check Point');
   if (@@Error <> 0)
   set @Check = -1;  
   
   insert into dbo.Synonym values(NEWID(), @SynonymContainerGUID, 'Publisher',  'CheckPoint%', 'Check Point');
   if (@@Error <> 0)
   set @Check = -1;

   insert into dbo.Synonym values(NEWID(), @SynonymContainerGUID, 'Publisher',  'Hewlett-Packard%', 'Hewlett-Packard');
   if (@@Error <> 0)
   set @Check = -1;

   insert into dbo.Synonym values(NEWID(), @SynonymContainerGUID, 'Publisher',  'ИнфоТеКС', 'ОАО &quot;Инфотекс&quot;');
   if (@@Error <> 0)
   set @Check = -1;
   
   --------------------------------------------------------------------------------------------------------------------------------- 

   insert into dbo.Synonym values(NEWID(), @SynonymContainerGUID, 'ProductName', 'ABBYY FineReader%Corporate Edition', 'ABBYY FineReader Corporate Edition');
   if (@@Error <> 0)
   set @Check = -1;

   insert into dbo.Synonym values(NEWID(), @SynonymContainerGUID, 'ProductName', 'Microsoft Office Professional Edition 2003', 'Microsoft Office Профессиональный 2003');
   if (@@Error <> 0)
   set @Check = -1;
  
   insert into dbo.Synonym values(NEWID(), @SynonymContainerGUID, 'ProductName', 'Microsoft Office Project Professional 2007', 'Microsoft Office Project Профессиональный 2007');
   if (@@Error <> 0)
   set @Check = -1;
  
   insert into dbo.Synonym values(NEWID(), @SynonymContainerGUID, 'ProductName', 'Microsoft Office Visio Professional 2007', 'Microsoft Office Visio Профессиональный 2007');
   if (@@Error <> 0)
   set @Check = -1;
  
   insert into dbo.Synonym values(NEWID(), @SynonymContainerGUID, 'ProductName', 'ABBYY Lingvo%English-Russian Dictionary', 'ABBYY Lingvo English-Russian Dictionary');
   if (@@Error <> 0)
   set @Check = -1;
  
   insert into dbo.Synonym values(NEWID(), @SynonymContainerGUID, 'ProductName', 'ABBYY Lingvo%Multilingual Dictionary', 'ABBYY Lingvo Multilingual Dictionary');
   if (@@Error <> 0)
   set @Check = -1;
 
   insert into dbo.Synonym values(NEWID(), @SynonymContainerGUID, 'ProductName', 'ABBYY Lingvo%Six Languages', 'ABBYY Lingvo Six Languages');
   if (@@Error <> 0)
   set @Check = -1;
  
   insert into dbo.Synonym values(NEWID(), @SynonymContainerGUID, 'ProductName', 'ACDSee%Photo Manager', 'ACDSee Photo Manager');
   if (@@Error <> 0)
   set @Check = -1;
    
   insert into dbo.Synonym values(NEWID(), @SynonymContainerGUID, 'ProductName', 'ACDSee%PowerPack', 'ACDSee PowerPack');
   if (@@Error <> 0)
   set @Check = -1;
   
   insert into dbo.Synonym values(NEWID(), @SynonymContainerGUID, 'ProductName', 'ACDSee%Standard', 'ACDSee Standard');
   if (@@Error <> 0)
   set @Check = -1;
     
   insert into dbo.Synonym values(NEWID(), @SynonymContainerGUID, 'ProductName', 'Adobe Acrobat%Professional', 'Adobe Acrobat Professional');
   if (@@Error <> 0)
   set @Check = -1;
  
   insert into dbo.Synonym values(NEWID(), @SynonymContainerGUID, 'ProductName', 'Adobe Acrobat%and Reader%Update', 'Adobe Acrobat and Reader Update');
   if (@@Error <> 0)
   set @Check = -1;
    
   insert into dbo.Synonym values(NEWID(), @SynonymContainerGUID, 'ProductName', 'Adobe Flash Player%ActiveX', 'Adobe Flash Player ActiveX');
   if (@@Error <> 0)
   set @Check = -1;
  
   insert into dbo.Synonym values(NEWID(), @SynonymContainerGUID, 'ProductName', 'Adobe Flash Player%Plugin', 'Adobe Flash Player Plugin');
   if (@@Error <> 0)
   set @Check = -1;
   
   insert into dbo.Synonym values(NEWID(), @SynonymContainerGUID, 'ProductName', 'DocsVision ___ Client', 'DocsVision Client');
   if (@@Error <> 0)
   set @Check = -1;
   
   insert into dbo.Synonym values(NEWID(), @SynonymContainerGUID, 'ProductName', 'DocsVision%Workflow Client', 'DocsVision Workflow Client');
   if (@@Error <> 0)
   set @Check = -1;
 
   insert into dbo.Synonym values(NEWID(), @SynonymContainerGUID, 'ProductName', 'DocsVision ___ Prerequisites', 'DocsVision Prerequisites');
   if (@@Error <> 0)
   set @Check = -1;
  
   insert into dbo.Synonym values(NEWID(), @SynonymContainerGUID, 'ProductName', 'DocsVision%Take%Office%Client', 'DocsVision Take Office Client');
   if (@@Error <> 0)
   set @Check = -1;
  
   insert into dbo.Synonym values(NEWID(), @SynonymContainerGUID, 'ProductName', 'InstallShield __ MSI Objects', 'InstallShield MSI Objects');
   if (@@Error <> 0)
   set @Check = -1;
   
   insert into dbo.Synonym values(NEWID(), @SynonymContainerGUID, 'ProductName', 'InstallShield __ MSI Setup Prerequisites', 'InstallShield MSI Setup Prerequisites');
   if (@@Error <> 0)
   set @Check = -1;
  
   insert into dbo.Synonym values(NEWID(), @SynonymContainerGUID, 'ProductName', 'Quest SQL Optimizer%for Oracle', 'Quest SQL Optimizer for Oracle');
   if (@@Error <> 0)
   set @Check = -1;
  
   insert into dbo.Synonym values(NEWID(), @SynonymContainerGUID, 'ProductName', 'Яндекс.Бар%для Internet Explorer', 'Яндекс.Бар для Internet Explorer');
   if (@@Error <> 0)
   set @Check = -1;
 
   insert into dbo.Synonym values(NEWID(), @SynonymContainerGUID, 'ProductName', 'DocsVision%Managed Client', 'DocsVision Managed Client');
   if (@@Error <> 0)
   set @Check = -1;
  
   insert into dbo.Synonym values(NEWID(), @SynonymContainerGUID, 'ProductName', 'CodeSite%Client Tools', 'CodeSite Client Tools');
   if (@@Error <> 0)
   set @Check = -1;

------------------------------------------------------------------------------------------------------------------------
  
   insert into dbo.Synonym values(NEWID(), @SynonymContainerGUID, 'DeviceDesc', 'VIA OHCI Compliant IEEE 1394 Host Controller', 'VIA OHCI-совместимый IEEE 1394 хост-контроллер');
   if (@@Error <> 0)
   set @Check = -1;
   
   insert into dbo.Synonym values(NEWID(), @SynonymContainerGUID, 'DeviceDesc', 'HID UPS Battery', 'Батарея ИБП HID');
   if (@@Error <> 0)
   set @Check = -1;
   
   insert into dbo.Synonym values(NEWID(), @SynonymContainerGUID, 'DeviceDesc', 'Floppy disk drive', 'Дисковод гибких дисков');
   if (@@Error <> 0)
   set @Check = -1;
   
   insert into dbo.Synonym values(NEWID(), @SynonymContainerGUID, 'DeviceDesc', 'Standard PS/2 Keyboard', 'Стандартная клавиатура PS/2');
   if (@@Error <> 0)
   set @Check = -1;
 
   insert into dbo.Synonym values(NEWID(), @SynonymContainerGUID, 'DeviceDesc', 'Standard 101/102-Key or Microsoft Natural PS/2 Keyboard', 'Стандартная (101/102 клавиши) или клавиатура PS/2 Microsoft Natural');
   if (@@Error <> 0)
   set @Check = -1;
   
   insert into dbo.Synonym values(NEWID(), @SynonymContainerGUID, 'DeviceDesc', 'Default Monitor', 'Монитор по умолчанию');
   if (@@Error <> 0)
   set @Check = -1;
  
   insert into dbo.Synonym values(NEWID(), @SynonymContainerGUID, 'DeviceDesc', 'Generic PnP Monitor', 'Универсальный монитор PnP');
   if (@@Error <> 0)
   set @Check = -1;
 
   insert into dbo.Synonym values(NEWID(), @SynonymContainerGUID, 'DeviceDesc', 'Generic Non-PnP Monitor', 'Универсальный монитор не PnP');
   if (@@Error <> 0)
   set @Check = -1;
   
   insert into dbo.Synonym values(NEWID(), @SynonymContainerGUID, 'DeviceDesc', 'HID-compliant mouse', 'HID-совместимая мышь');
   if (@@Error <> 0)
   set @Check = -1;

   ------------------------------------------------------------------------------------
   -- db migration. ORDER OF INSERT IS IMPORTANT!

insert into dbo.DBMigration values('Category', 1, 'Name, Description, InitLibFlag', '', 0, 'Library');
if (@@Error <> 0)  set @Check = -1;
insert into dbo.DBMigration values('Publisher', 1, 'Name, Website, Phone, Address, InitLibFlag', '', 0, 'Library');
if (@@Error <> 0)  set @Check = -1;
insert into dbo.DBMigration values('SoftType', 1, 'PublisherGUID, ProductName, VersionMajor, CategoryGUID, InitLibFlag', '', 0, 'Library');
if (@@Error <> 0)  set @Check = -1;
insert into dbo.DBMigration values('LibProductFile', 0, 'ParentGUID, FileVersion, OriginalFileName, Company, ProductName, ProductVersion, Language, Size, InitLibFlag', '', 0, 'Library');
if (@@Error <> 0)  set @Check = -1;
insert into dbo.DBMigration values('Soft', 1, 'SoftTypeGUID, VersionMinor, Localization, ProductKey, IsOperatingSystem, InspectionDate, IsVisible, NULL, NULL', '', 0, 'Library');
if (@@Error <> 0)  set @Check = -1;
insert into dbo.DBMigration values('Hardware', 1, 'Class, DeviceDesc, Memory, Frequency, Quantity, Speed, IsVisible, NULL, NULL, NULL, NULL, NULL, NULL, NULL', '', 0, 'Library');
if (@@Error <> 0)  set @Check = -1;
insert into dbo.DBMigration values('FoundProductFile', 1, 'FileName, FileVersion, OriginalFileName, Company, ProductName, ProductVersion, Language, Size', '', 0, 'Library');
if (@@Error <> 0)  set @Check = -1;

insert into dbo.DBMigration values('Subdivision', 1, 'Name, Description', '', 1, 'Computers');
if (@@Error <> 0)  set @Check = -1;
insert into dbo.DBMigration values('Computer', 1, 'Name, Status, Description, SubdivisionGUID', '', 1, 'Computers');
if (@@Error <> 0)  set @Check = -1;

insert into dbo.DBMigration values('Task', 0, 'Login, Password, Domain, SearchFileMask, SearchPath, SyncDN, SyncSchedule, InspectionType, 
                                          InspectionOptions, UseAlternativeCredentials, SearchFilesLimit, Timeout, NextInspectTime, 
                                          LastInspectTime, InspectionStoringPeriod, MailTo, SendMail, SharedFolder, ReportList, ScanWithAgent,
                                          Schedule, ScanUninspectedOnly, SearchFiles, LimitFiles, UseDNSubChildren, ProgramComponentsSearch,
                                          ProgramComponentsSearchMasks, ProgramComponentsSearchDiskList, ProgramComponentsSearchExcludeSystemDir,
                                          ProgramComponentsSearchExcludeHiddenOrSystem, ProgramComponentsSearchLastModifiedDate, 
                                          ScanWithAgentTimeout, AlternativeScan', 'NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL', 0, 'Tasks');  -- first and last 0 becouse of data are given only from table task, objectrelations is filled separately
if (@@Error <> 0)  set @Check = -1;
insert into dbo.DBMigration values('ComputerLink', 0, 'ParentGUID, ComputerGUID', '', 1, 'Tasks');
if (@@Error <> 0)  set @Check = -1; 

insert into dbo.DBMigration values('LicenseRule', 1, 'Name, ProductName, Publisher, Localization, ProductVersion, ProductKey, Description, MaxInstallAmount, SoftType, CreatedForPassportGUID', 'NULL', 1, 'Rules');
if (@@Error <> 0)  set @Check = -1;
insert into dbo.DBMigration values('LicenseImage', 0, 'Name, ParentGUID, LicenseType, Description, BinaryData', '', 1, 'Rules');
if (@@Error <> 0)  set @Check = -1;
insert into dbo.DBMigration values('HardwareLicenseRule', 1, 'Name, Class, DeviceDesc, Description, MinMemory, MaxMemory, MinFreeMemory, MaxFreeMemory, MinFrequency, MaxFrequency, MinQuantity, MaxQuantity, CreatedForPassportGUID, NULL, ''%%'', ''%%'', ''%%'', ''%%'', ''%%'', ''%%''', 'NULL, NULL', 1, 'Rules');
if (@@Error <> 0)  set @Check = -1;
insert into dbo.DBMigration values('ProhibitionRule', 1, 'Name, ProductName, Publisher, Localization, ProductVersion, ProductKey, Description, SoftType', '', 1, 'Rules');
if (@@Error <> 0)  set @Check = -1;

insert into dbo.DBMigration values('Passport', 1, 'Name, Description', '', 1, 'Passports');
if (@@Error <> 0)  set @Check = -1;
insert into dbo.DBMigration values('LicenseRuleLink', 0, 'ParentGUID, LicenseRuleGUID', '', 1, 'Passports');
if (@@Error <> 0)  set @Check = -1;
insert into dbo.DBMigration values('HardwareLicenseRuleLink', 0, 'ParentGUID, HardwareLicenseRuleGUID', '', 1, 'Passports');
if (@@Error <> 0)  set @Check = -1;
insert into dbo.DBMigration values('PassportLink', 0, 'ParentGUID, PassportGUID', '', 1, 'Passports');
if (@@Error <> 0)  set @Check = -1;

insert into dbo.DBMigration values('Action', 0, 'ActionType, ActionDate, ResultCode, ErrorText, InspectionType, ParentGUID, CreationDate, IP, Architecture, Domain, Users, Virtuality, ProductFilesIdentifier, BootIni', 'NULL, NULL, NULL, NULL', 0, 'Inspections');
if (@@Error <> 0)  set @Check = -1;
insert into dbo.DBMigration values('Installation', 0, 'SoftGUID, ParentGUID, CreationDate, SoftInstallDate', '', 0, 'Inspections');
if (@@Error <> 0)  set @Check = -1;
insert into dbo.DBMigration values('HardInstallation', 0, 'HardwareGUID, ParentGUID, CreationDate, Mfg, FreeMemory, NULL, NULL, NULL, NULL, NULL', '', 0, 'Inspections');
if (@@Error <> 0)  set @Check = -1;
insert into dbo.DBMigration values('FileInstallation', 0, 'ParentGUID, FoundProductFileGUID, FileName', '', 0, 'Inspections');
if (@@Error <> 0)  set @Check = -1;
insert into dbo.DBMigration values('FilesFound', 0, 'ParentGUID, CreationDate, FileMask, FilePath, Size', '', 0, 'Inspections');
if (@@Error <> 0)  set @Check = -1; 


         
   if (@Check = 0)

        COMMIT TRANSACTION
   else 
   begin 
        RAISERROR('Транзакция отменена', 12, 1);
        Return; 
   end

END TRY
BEGIN CATCH

if (Error_Message()  is NULL or Error_Message() = '')
   set @Message = 'Ошибка БД при добавлении первичных объектов';
else
   set @Message = Error_Message();
   
ROLLBACK TRANSACTION
RAISERROR(@Message, 12, 1);
Return;

END CATCH

END


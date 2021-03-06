CREATE TABLE [dbo].[ObjectRelations](
	[ObjectGUID] [uniqueidentifier] NOT NULL,
	[ObjectType] [nvarchar](50) NOT NULL,
	[Name] [nvarchar](400) NOT NULL,
	[ParentGUID] [uniqueidentifier] NOT NULL,
	[CreationDate] [datetime] NOT NULL,
PRIMARY KEY  
(
	[ObjectGUID] 
)WITH (PAD_INDEX  = OFF, STATISTICS_NORECOMPUTE  = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS  = ON, ALLOW_PAGE_LOCKS  = ON) ON [PRIMARY],
CONSTRAINT TypeName UNIQUE(Name, ObjectType)
) ON [PRIMARY];
 
CREATE NONCLUSTERED INDEX [Index_ParentGUID] ON [dbo].[ObjectRelations] 
(
	[ParentGUID] ASC
)WITH (PAD_INDEX  = OFF, STATISTICS_NORECOMPUTE  = OFF, SORT_IN_TEMPDB = OFF, IGNORE_DUP_KEY = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS  = ON, ALLOW_PAGE_LOCKS  = ON) ON [PRIMARY];

CREATE INDEX ObjectRelations_Name 
      ON dbo.ObjectRelations (Name);
CREATE INDEX ObjectRelations_Type 
      ON dbo.ObjectRelations (ObjectType);

CREATE TABLE [dbo].[Computer](
	[ObjectGUID] [uniqueidentifier] NOT NULL,
	[ParentGUID] [uniqueidentifier] NOT NULL,
    [Name] [nvarchar](450) NOT NULL,
    [Status] [int] NOT NULL,
    [Description] [nvarchar](500) NULL,
	[SubdivisionGUID] [uniqueidentifier] NULL,    
PRIMARY KEY  
(
	[ObjectGUID] 
)WITH (PAD_INDEX  = OFF, STATISTICS_NORECOMPUTE  = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS  = ON, ALLOW_PAGE_LOCKS  = ON) ON [PRIMARY],
foreign key (ParentGUID) REFERENCES ObjectRelations(ObjectGUID)
ON UPDATE CASCADE
ON DELETE CASCADE
) ON [PRIMARY];

CREATE INDEX Computer_Name 
      ON dbo.Computer (Name);
CREATE INDEX Computer_ParentGUID 
      ON dbo.Computer (ParentGUID);

CREATE TABLE [dbo].[Passport](
	[ObjectGUID] [uniqueidentifier] NOT NULL,
	[ParentGUID] [uniqueidentifier] NOT NULL,
    [Name] [nvarchar](450) NOT NULL,
	[Description] [nvarchar](1000) NULL
PRIMARY KEY CLUSTERED 
(
	[ObjectGUID] 
)WITH (PAD_INDEX  = OFF, STATISTICS_NORECOMPUTE  = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS  = ON, ALLOW_PAGE_LOCKS  = ON) ON [PRIMARY],
foreign key (ParentGUID) REFERENCES ObjectRelations(ObjectGUID)
ON UPDATE CASCADE
ON DELETE CASCADE
) ON [PRIMARY];

CREATE INDEX Passport_Name 
      ON dbo.Passport (Name);
CREATE INDEX Passport_ParentGUID 
      ON dbo.Passport (ParentGUID);

CREATE TABLE [dbo].[Action](
	[ObjectGUID] [uniqueidentifier] NOT NULL,
	[ActionType] [smallint] NOT NULL,
	[ActionDate] [datetime] NOT NULL,
	[ResultCode] [smallint] NOT NULL,
	[ErrorText] [nvarchar](2000) NULL,
	[InspectionType] [smallint] NULL,
	[ParentGUID] [uniqueidentifier] NOT NULL,
	[CreationDate] [datetime] NOT NULL,
	[IP] [nvarchar](500) NULL,
	[Architecture] [nvarchar](100) NULL,
	[Domain] [nvarchar](300) NULL,
	[Users] [nvarchar](max) NULL,
	[Virtuality] [nvarchar](300) NULL,
    [ProductFilesIdentifier] [nvarchar](300) NULL,
    [BootIni] [nvarchar](max) NULL,
    [IncrementalSearchData] [nvarchar](max) NULL,
	[IncrementalSearch] [bit] NULL,
	[FileSearch] [bit] NULL,
	[ProgramSearch] [bit] NULL,
PRIMARY KEY CLUSTERED 
(
	[ObjectGUID] 
)WITH (PAD_INDEX  = OFF, STATISTICS_NORECOMPUTE  = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS  = ON, ALLOW_PAGE_LOCKS  = ON) ON [PRIMARY],
foreign key (ParentGUID) REFERENCES Computer(ObjectGUID)
ON UPDATE CASCADE
ON DELETE CASCADE
) ON [PRIMARY];

CREATE INDEX Action_ParentGUID 
      ON dbo.Action (ParentGUID);


CREATE TABLE [dbo].[FoundProductFile](
	[ObjectGUID] [uniqueidentifier] NOT NULL,
	[ParentGUID] [uniqueidentifier] NOT NULL,
	[FileName]   [nvarchar](450) NULL,
    [FileVersion]   [nvarchar](450) NULL,
    [OriginalFileName]   [nvarchar](450) NULL,  
    [Company]    [nvarchar](450) NULL,
    [ProductName]   [nvarchar](450) NULL, 
    [ProductVersion]    [nvarchar](450) NULL,  
    [Language]    [nvarchar](450) NULL,
    [Size]    [bigint] NULL,
PRIMARY KEY CLUSTERED 
(
	[ObjectGUID] 
)WITH (PAD_INDEX  = OFF, STATISTICS_NORECOMPUTE  = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS  = ON, ALLOW_PAGE_LOCKS  = ON) ON [PRIMARY],
foreign key (ParentGUID) REFERENCES ObjectRelations(ObjectGUID)
ON UPDATE CASCADE
ON DELETE CASCADE) ON [PRIMARY];

CREATE INDEX FoundProductFile_ParentGUID 
      ON dbo.FoundProductFile (ParentGUID);  
CREATE INDEX FoundProductFile_OriginalFileName 
      ON dbo.FoundProductFile (OriginalFileName);
CREATE INDEX FoundProductFile_FileName 
      ON dbo.FoundProductFile (FileName);  
CREATE INDEX FoundProductFile_Company 
      ON dbo.FoundProductFile (Company); 
CREATE INDEX FoundProductFile_ProductName 
      ON dbo.FoundProductFile (ProductName); 
CREATE INDEX FoundProductFile_FileVersion 
      ON dbo.FoundProductFile (FileVersion);
CREATE INDEX FoundProductFile_ProductVersion 
      ON dbo.FoundProductFile (ProductVersion);
CREATE INDEX FoundProductFile_Language 
      ON dbo.FoundProductFile (Language);
      
CREATE TABLE [dbo].[USBDevice](
	[ObjectGUID] [uniqueidentifier] NOT NULL,
	[ParentGUID] [uniqueidentifier] NOT NULL,
	[Name] [nvarchar](450) NULL,
	[DeviceType] [nvarchar](450) NULL,  
	[Description] [nvarchar](1000) NULL,
	[SerialNumber] [nvarchar](1000) NULL, 
	[LastUsing] [datetime] NULL,
	[Status] [int] NULL,
PRIMARY KEY CLUSTERED 
(
	[ObjectGUID] 
)WITH (PAD_INDEX  = OFF, STATISTICS_NORECOMPUTE  = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS  = ON, ALLOW_PAGE_LOCKS  = ON) ON [PRIMARY],
foreign key (ParentGUID) REFERENCES Action(ObjectGUID)
ON UPDATE CASCADE
ON DELETE CASCADE
) ON [PRIMARY];

CREATE INDEX USBDevice_ParentGUID 
      ON dbo.USBDevice (ParentGUID);
CREATE INDEX USBDevice_Name 
      ON dbo.USBDevice (Name);


CREATE TABLE [dbo].[Synonym](
	[ObjectGUID] [uniqueidentifier] NOT NULL,
    [ParentGUID] [uniqueidentifier] NOT NULL,
	[AttributeName] [nvarchar](100) NOT NULL,
	[Template] [nvarchar](2000) NULL,
	[DisplayName] [nvarchar](2000) NULL,
PRIMARY KEY CLUSTERED 
(
	[ObjectGUID] 
)WITH (PAD_INDEX  = OFF, STATISTICS_NORECOMPUTE  = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS  = ON, ALLOW_PAGE_LOCKS  = ON) ON [PRIMARY],
foreign key (ParentGUID) REFERENCES ObjectRelations(ObjectGUID)
ON UPDATE CASCADE
ON DELETE CASCADE) ON [PRIMARY];

CREATE INDEX Synonym_ParentGUID 
      ON dbo.Synonym (ParentGUID);

CREATE TABLE [dbo].[Publisher](
	[ObjectGUID] [uniqueidentifier] NOT NULL,
	[ParentGUID] [uniqueidentifier] NOT NULL,
	[Name] [nvarchar](450) NULL,
	[Website] [nvarchar](3000) NULL,
	[Phone] [nvarchar](100) NULL,
	[Address] [nvarchar](3000) NULL,
	[InitLibFlag] [bit] NOT NULL,
PRIMARY KEY CLUSTERED 
(
	[ObjectGUID] 
)WITH (PAD_INDEX  = OFF, STATISTICS_NORECOMPUTE  = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS  = ON, ALLOW_PAGE_LOCKS  = ON) ON [PRIMARY],
foreign key (ParentGUID) REFERENCES ObjectRelations(ObjectGUID)
ON UPDATE CASCADE
ON DELETE CASCADE) ON [PRIMARY];

CREATE INDEX Publisher_Name 
      ON dbo.Publisher (Name);

CREATE TABLE [dbo].[Category](
	[ObjectGUID] [uniqueidentifier] NOT NULL,
	[ParentGUID] [uniqueidentifier] NOT NULL,
	[Name] [nvarchar](450) NULL,
	[Description] [nvarchar](1000) NULL,
	[InitLibFlag] [bit] NOT NULL,
PRIMARY KEY CLUSTERED 
(
	[ObjectGUID] 
)WITH (PAD_INDEX  = OFF, STATISTICS_NORECOMPUTE  = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS  = ON, ALLOW_PAGE_LOCKS  = ON) ON [PRIMARY],
foreign key (ParentGUID) REFERENCES ObjectRelations(ObjectGUID)
ON UPDATE CASCADE
ON DELETE CASCADE,
CONSTRAINT Category_Name UNIQUE(Name)
) ON [PRIMARY];

--CREATE INDEX Category_Name 
      --ON dbo.Category (Name);

CREATE TABLE [dbo].[SoftType](
	[ObjectGUID] [uniqueidentifier] NOT NULL,
	[ParentGUID] [uniqueidentifier] NOT NULL,
	[PublisherGUID] [uniqueidentifier] NULL,
	[ProductName] [nvarchar](450) NULL,
	[VersionMajor] [nvarchar](450) NULL,   
	[CategoryGUID] [uniqueidentifier] NULL,
	[InitLibFlag] [bit] NOT NULL,	
PRIMARY KEY CLUSTERED 
(
	[ObjectGUID] 
)WITH (PAD_INDEX  = OFF, STATISTICS_NORECOMPUTE  = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS  = ON, ALLOW_PAGE_LOCKS  = ON) ON [PRIMARY],
foreign key (ParentGUID) REFERENCES ObjectRelations(ObjectGUID),
foreign key (PublisherGUID) REFERENCES Publisher(ObjectGUID)
ON UPDATE CASCADE
ON DELETE CASCADE) ON [PRIMARY];

CREATE INDEX SoftType_ProductName 
      ON dbo.SoftType (ProductName);
CREATE INDEX SoftType_CategoryGUID 
      ON dbo.SoftType (CategoryGUID);
CREATE INDEX SoftType_PublisherGUID 
      ON dbo.SoftType (PublisherGUID);


CREATE TABLE [dbo].[Soft](
	[ObjectGUID] [uniqueidentifier] NOT NULL,
	[ParentGUID] [uniqueidentifier] NOT NULL,
	[SoftTypeGUID] [uniqueidentifier] NOT NULL,
	[VersionMinor] [nvarchar](50) NULL,
	[Localization] [nvarchar](50) NULL,
	[ProductKey] [nvarchar](300) NULL,
	[IsOperatingSystem] [smallint] NULL,
	[InspectionDate] [datetime] NOT NULL,
	[IsVisible] [smallint] NOT NULL,
	[VersionBuildMinor] [nvarchar](150) NULL,
	[VersionBuildMajor] [nvarchar](150) NULL,	
PRIMARY KEY CLUSTERED 
(
	[ObjectGUID] 
)WITH (PAD_INDEX  = OFF, STATISTICS_NORECOMPUTE  = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS  = ON, ALLOW_PAGE_LOCKS  = ON) ON [PRIMARY],
foreign key (ParentGUID) REFERENCES ObjectRelations(ObjectGUID),
foreign key (SoftTypeGUID) REFERENCES SoftType(ObjectGUID)
ON UPDATE CASCADE
ON DELETE CASCADE,) ON [PRIMARY]; 

CREATE INDEX Soft_SoftTypeGUID 
      ON dbo.Soft (SoftTypeGUID);
CREATE INDEX Soft_Localization 
      ON dbo.Soft (Localization);
CREATE INDEX Soft_ProductKey 
      ON dbo.Soft (ProductKey);

CREATE TABLE [dbo].[LibProductFile](
	[ObjectGUID] [uniqueidentifier] NOT NULL,
    [ParentGUID] [uniqueidentifier] NOT NULL,
    [FileVersion]   [nvarchar](450) NULL,
    [OriginalFileName]   [nvarchar](450) NULL,  
    [Company]    [nvarchar](450) NULL,
    [ProductName]   [nvarchar](450) NULL, 
    [ProductVersion]    [nvarchar](450) NULL,  
    [Language]    [nvarchar](450) NULL,
    [Size] [bigint] NULL, 
    [InitLibFlag] [bit] NOT NULL,  
PRIMARY KEY CLUSTERED 
(
	[ObjectGUID] 
)WITH (PAD_INDEX  = OFF, STATISTICS_NORECOMPUTE  = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS  = ON, ALLOW_PAGE_LOCKS  = ON) ON [PRIMARY],
foreign key (ParentGUID) REFERENCES SoftType(ObjectGUID)
ON UPDATE CASCADE
ON DELETE CASCADE
) ON [PRIMARY];

CREATE INDEX LibProductFile_ParentGUID 
      ON dbo.LibProductFile (ParentGUID);
CREATE INDEX LibProductFile_OriginalFileName 
      ON dbo.LibProductFile (OriginalFileName);
CREATE INDEX LibProductFile_Company 
      ON dbo.LibProductFile (Company);
CREATE INDEX LibProductFile_ProductName 
      ON dbo.LibProductFile (ProductName);
 

CREATE TABLE [dbo].[Subdivision](
	[ObjectGUID] [uniqueidentifier] NOT NULL,
	[ParentGUID] [uniqueidentifier] NOT NULL,
    [Name] [nvarchar](450) NOT NULL,
	[Description] [nvarchar](2000) NULL,
PRIMARY KEY CLUSTERED 
(
	[ObjectGUID] 
)WITH (PAD_INDEX  = OFF, STATISTICS_NORECOMPUTE  = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS  = ON, ALLOW_PAGE_LOCKS  = ON) ON [PRIMARY],
foreign key (ParentGUID) REFERENCES ObjectRelations(ObjectGUID)
ON UPDATE CASCADE
ON DELETE CASCADE
) ON [PRIMARY];

CREATE INDEX Subdivision_Name 
      ON dbo.Subdivision (Name);
CREATE INDEX Subdivision_ParentGUID 
      ON dbo.Subdivision (ParentGUID);

CREATE TABLE [dbo].[Installation](
	[ObjectGUID] [uniqueidentifier] NOT NULL,
	[SoftGUID] [uniqueidentifier] NOT NULL,
	[ParentGUID] [uniqueidentifier] NOT NULL,
	[CreationDate] [datetime] NOT NULL,
	[SoftInstallDate] [datetime] NULL,
PRIMARY KEY CLUSTERED 
(
	[ObjectGUID] 
)WITH (PAD_INDEX  = OFF, STATISTICS_NORECOMPUTE  = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS  = ON, ALLOW_PAGE_LOCKS  = ON) ON [PRIMARY],
foreign key (SoftGUID) REFERENCES Soft(ObjectGUID),
foreign key (ParentGUID) REFERENCES Action(ObjectGUID)
ON UPDATE CASCADE
ON DELETE CASCADE
) ON [PRIMARY];

CREATE INDEX Installation_SoftGUID 
      ON dbo.Installation (SoftGUID);
CREATE INDEX Installation_ParentGUID 
      ON dbo.Installation (ParentGUID);

CREATE TABLE [dbo].[FileInstallation](
	[ObjectGUID] [uniqueidentifier] NOT NULL,
	[ParentGUID] [uniqueidentifier] NOT NULL,
	[FoundProductFileGUID] [uniqueidentifier] NOT NULL,
	[FileName]   [nvarchar](3000) NULL,
PRIMARY KEY CLUSTERED 
(
	[ObjectGUID] 
)WITH (PAD_INDEX  = OFF, STATISTICS_NORECOMPUTE  = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS  = ON, ALLOW_PAGE_LOCKS  = ON) ON [PRIMARY],
foreign key (FoundProductFileGUID) REFERENCES FoundProductFile(ObjectGUID),
foreign key (ParentGUID) REFERENCES Action(ObjectGUID)
ON UPDATE CASCADE
ON DELETE CASCADE
) ON [PRIMARY];

CREATE INDEX FileInstallation_FoundProductFileGUID 
      ON dbo.FileInstallation (FoundProductFileGUID);
CREATE INDEX FileInstallation_ParentGUID 
      ON dbo.FileInstallation (ParentGUID);

CREATE TABLE [dbo].[LicenseRule](
	[ObjectGUID] [uniqueidentifier] NOT NULL,
	[ParentGUID] [uniqueidentifier] NOT NULL,
    [Name] [nvarchar](450) NOT NULL,
	[ProductName] [nvarchar](450)NULL,
    [Publisher] [nvarchar](450)NULL,
	[Localization] [nvarchar](50)NULL,
	[ProductVersion] [nvarchar](450)NULL,
	[ProductKey] [nvarchar](300)NULL,
	[Description] [nvarchar](4000)NULL,
	[MaxInstallAmount] [int] NULL,
	[SoftType] [tinyint] NULL,
	[CreatedForPassportGUID] [uniqueidentifier] NULL,
	[LicenseExpire] [datetime] NULL,
PRIMARY KEY CLUSTERED 
(
	[ObjectGUID] 
)WITH (PAD_INDEX  = OFF, STATISTICS_NORECOMPUTE  = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS  = ON, ALLOW_PAGE_LOCKS  = ON) ON [PRIMARY],
foreign key (ParentGUID) REFERENCES ObjectRelations(ObjectGUID)
ON UPDATE CASCADE
ON DELETE CASCADE
) ON [PRIMARY];

CREATE INDEX LicenseRule_Name 
      ON dbo.LicenseRule (Name);
CREATE INDEX LicenseRule_ParentGUID 
      ON dbo.LicenseRule (ParentGUID);
CREATE INDEX LicenseRule_ProductName 
      ON dbo.LicenseRule (ProductName);
CREATE INDEX LicenseRule_Publisher 
      ON dbo.LicenseRule (Publisher);
CREATE INDEX LicenseRule_CreatedForPassportGUID 
      ON dbo.LicenseRule (CreatedForPassportGUID);

CREATE TABLE [dbo].[LicenseImage](
	[ObjectGUID] [uniqueidentifier] NOT NULL,
    [Name] [nvarchar](450) NOT NULL,
	[ParentGUID] [uniqueidentifier] NOT NULL, 
	[LicenseType] [nvarchar](10) NOT NULL,
	[Description] [nvarchar](4000) NULL, 
	[BinaryData] [image] NOT NULL,
PRIMARY KEY CLUSTERED 
(
	[ObjectGUID] 
)WITH (PAD_INDEX  = OFF, STATISTICS_NORECOMPUTE  = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS  = ON, ALLOW_PAGE_LOCKS  = ON) ON [PRIMARY],
foreign key (ParentGUID) REFERENCES LicenseRule(ObjectGUID)
ON UPDATE CASCADE
ON DELETE CASCADE
) ON [PRIMARY];

CREATE TABLE [dbo].[ObjectSchema](
	[ObjectGUID] [uniqueidentifier] NOT NULL,
	[ParentGUID] [uniqueidentifier] NOT NULL,
	[Name] [nvarchar](450) NOT NULL,
	[Attributes] [nvarchar](1500) NOT NULL,
	[PossibleSuperiors] [nvarchar](100) NOT NULL,
	[PossibleInferiors] [nvarchar](100) NOT NULL,
	[IsEdited] [bit] NOT NULL,
PRIMARY KEY  
(
	[ObjectGUID] 
)WITH (PAD_INDEX  = OFF, STATISTICS_NORECOMPUTE  = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS  = ON, ALLOW_PAGE_LOCKS  = ON) ON [PRIMARY],
foreign key (ParentGUID) REFERENCES ObjectRelations(ObjectGUID)
ON UPDATE CASCADE
ON DELETE CASCADE
) ON [PRIMARY];

CREATE TABLE [dbo].[ObjectRoot](
	[ObjectGUID] [uniqueidentifier] NOT NULL,
	[Version] [int] NULL,
	[DN] [nvarchar](300) NULL,
	[DefaultPolicy] [bit] NULL,
	[TurnOffBackgroundRefresh] [bit] NULL,
    [RefreshInterval] [int] NULL,
    [DoNotAllowExceptions] [bit] NULL,
    [FileAndPrinterSharing] [bit] NULL,
    [UserPicture] [image]  NULL,
	[UserCompanyName] [nvarchar](450) NULL,
	[IsUserPicture] [bit]  NULL,
    [DefaultPublisher] [nvarchar](50)  NULL,
	[DefaultProductName] [nvarchar](450) NULL,
	[DefaultVersion] [nvarchar](50)  NULL,
	[DefaultLanguage] [nvarchar](450)  NULL,
	[DefaultUnitName] [nvarchar](450) NULL,
	[DefaultManufacturer] [nvarchar](100)  NULL,
	[DefaultClass] [nvarchar](450)  NULL,
	[IsReportTitle] [bit]  NOT NULL,
	[SMTPHost] [nvarchar](200) NULL,
	[Port] [int] NOT NULL CONSTRAINT [DF_ObjectRoot_Port]  DEFAULT ((25)),
	[CopyCompDescToAD] [bit]  NULL,
PRIMARY KEY  
(
	[ObjectGUID] 
)WITH (PAD_INDEX  = OFF, STATISTICS_NORECOMPUTE  = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS  = ON, ALLOW_PAGE_LOCKS  = ON) ON [PRIMARY],
foreign key (ObjectGUID) REFERENCES ObjectRelations(ObjectGUID)
ON UPDATE CASCADE
ON DELETE CASCADE
) ON [PRIMARY];


CREATE TABLE [dbo].[Localization](
	[ID] [int] IDENTITY(1,1) NOT NULL,
	[Language] [nvarchar](450) NOT NULL,
	[LocalizationCode] [nvarchar](10) NOT NULL,
) ON [PRIMARY];

CREATE INDEX Localization_Language 
      ON dbo.Localization (Language);
CREATE INDEX Localization_LocalizationCode 
      ON dbo.Localization (LocalizationCode);


CREATE TABLE [dbo].[Task](
	[ObjectGUID] [uniqueidentifier] NOT NULL,
	[Login] [nvarchar](150) NULL,
	[Password] [varbinary](256) NULL,
	[Domain] [nvarchar](300) NULL,
	[SearchFileMask] [nvarchar](300) NOT NULL,
	[SearchPath] [nvarchar](300) NULL,
	[SyncDN] [nvarchar](300) NOT NULL,
	[SyncSchedule] [nvarchar](max) NULL,
	[InspectionType] [smallint] NOT NULL,
    [InspectionOptions] [int] NULL,
    [UseAlternativeCredentials] [bit] NOT NULL,
    [SearchFilesLimit] [int] NULL,
    [Timeout] [int] NOT NULL CONSTRAINT [DF_Task_Timeout]  DEFAULT ((60)),
    [NextInspectTime] [datetime] NULL,
    [LastInspectTime] [datetime] NULL,
    [InspectionStoringPeriod] [int] NULL,
    [MailTo] [nvarchar](100) NULL,
    [SendMail] [bit] NOT NULL,
    [SharedFolder] [nvarchar](300) NULL,
    [ReportList] [nvarchar](300) NULL,
    [ScanWithAgent] [bit] NOT NULL,
    [Schedule] [nvarchar](max) NULL,
    [ScanUninspectedOnly] [bit] NOT NULL DEFAULT (0),
    [SearchFiles] [bit] NOT NULL DEFAULT (0),
    [LimitFiles] [bit] NOT NULL DEFAULT (0),
    [UseDNSubChildren] [bit] NULL DEFAULT (0),
    [ProgramComponentsSearch] [bit] NULL,
    [ProgramComponentsSearchMasks] [nvarchar](300) NULL,
    [ProgramComponentsSearchDiskList] [nvarchar](300) NULL,
    [ProgramComponentsSearchExcludeSystemDir] [bit] NULL,
    [ProgramComponentsSearchExcludeHiddenOrSystem] [bit] NULL,
    [ProgramComponentsSearchLastModifiedDate] [datetime] NULL,
    [ScanWithAgentTimeout] [int] NOT NULL,
    [AlternativeScan] [bit] NOT NULL,
    [CheckHeader] [bit] NULL,
    [IncrementalSearch] [bit] NULL,
	[SNChanged] [bigint] NULL,
	[SearchFilesModifiedDate] [datetime] NULL, 
	[SearchFilesMinSize] [bigint] NULL, 
	[SearchFilesMaxSize] [bigint] NULL,
	[IsSearchFilesByModifiedDate] [bit] NULL,
	[IsSearchFilesByRangeSizes] [bit] NULL,
	[ProgramComponentsLimitFiles] [int] NULL,
	[IsProgramComponentsLimitFiles] [bit] NULL
	PRIMARY KEY CLUSTERED 
(
	[ObjectGUID] 
)WITH (PAD_INDEX  = OFF, STATISTICS_NORECOMPUTE  = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS  = ON, ALLOW_PAGE_LOCKS  = ON) ON [PRIMARY],
foreign key (ObjectGUID) REFERENCES ObjectRelations(ObjectGUID)
ON UPDATE CASCADE
ON DELETE CASCADE
) ON [PRIMARY];

CREATE TABLE [dbo].[ComputerLink](
	[ObjectGUID] [uniqueidentifier] NOT NULL,
    [ParentGUID] [uniqueidentifier] NOT NULL,
	[ComputerGUID] [uniqueidentifier] NOT NULL,
foreign key (ComputerGUID) REFERENCES Computer(ObjectGUID),
foreign key (ParentGUID) REFERENCES Task(ObjectGUID)
ON UPDATE CASCADE
ON DELETE CASCADE
) ON [PRIMARY];

CREATE INDEX ComputerLink_ParentGUID 
      ON dbo.ComputerLink (ParentGUID);
CREATE INDEX ComputerLink_ObjectGUID 
      ON dbo.ComputerLink (ObjectGUID);
CREATE INDEX ComputerLink_ComputerGUID 
      ON dbo.ComputerLink (ComputerGUID);      


CREATE TABLE [dbo].[PassportLink](
	[ObjectGUID] [uniqueidentifier] NOT NULL,
    [ParentGUID] [uniqueidentifier] NOT NULL,
	[PassportGUID] [uniqueidentifier] NOT NULL,
foreign key (PassportGUID) REFERENCES Passport(ObjectGUID),
foreign key (ParentGUID) REFERENCES Computer(ObjectGUID)
ON UPDATE CASCADE
ON DELETE CASCADE
) ON [PRIMARY];

CREATE INDEX PassportLink_ParentGUID 
      ON dbo.PassportLink (ParentGUID);
CREATE INDEX PassportLink_ObjectGUID 
      ON dbo.PassportLink (ObjectGUID);
CREATE INDEX PassportLink_PassportGUID 
      ON dbo.PassportLink (PassportGUID);

CREATE TABLE [dbo].[LicenseRuleLink](
	[ObjectGUID] [uniqueidentifier] NOT NULL,
    [ParentGUID] [uniqueidentifier] NOT NULL,
	[LicenseRuleGUID] [uniqueidentifier] NOT NULL,
foreign key (LicenseRuleGUID) REFERENCES LicenseRule(ObjectGUID),
foreign key (ParentGUID) REFERENCES Passport(ObjectGUID)
ON UPDATE CASCADE
ON DELETE CASCADE
) ON [PRIMARY];

CREATE INDEX LicenseRuleLink_ParentGUID 
      ON dbo.LicenseRuleLink (ParentGUID);
CREATE INDEX LicenseRuleLink_ObjectGUID 
      ON dbo.LicenseRuleLink (ObjectGUID);
CREATE INDEX LicenseRuleLink_LicenseRuleGUID 
      ON dbo.LicenseRuleLink (LicenseRuleGUID);

CREATE TABLE [dbo].[Hardware](
	[ObjectGUID] [uniqueidentifier] NOT NULL,
    [ParentGUID] [uniqueidentifier] NOT NULL,
	[Class] [nvarchar](450) NULL,
	[DeviceDesc] [nvarchar](450) NULL,
	[Memory] [bigint] NULL,
	[Frequency] [int] NULL,
	[Quantity] [int]  NULL,
	[Speed] [int] NULL,
	[IsVisible] [smallint] NOT NULL,
	[Version]	[nvarchar](500) NULL,
	[WakeupType]	[nvarchar](500) NULL,
	[BusWidth]	[nvarchar](500) NULL,
	[BusLength] [nvarchar](500) NULL,
	[Socket] [nvarchar](500) NULL,
	[Service] [nvarchar](100) NULL,	
	[Chipset] [nvarchar](450) NULL,	
	PRIMARY KEY CLUSTERED 
(
	[ObjectGUID] 
)WITH (PAD_INDEX  = OFF, STATISTICS_NORECOMPUTE  = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS  = ON, ALLOW_PAGE_LOCKS  = ON) ON [PRIMARY],
foreign key (ParentGUID) REFERENCES ObjectRelations(ObjectGUID)
ON UPDATE CASCADE
ON DELETE CASCADE) ON [PRIMARY];


CREATE INDEX Hardware_ParentGUID 
      ON dbo.Hardware (ParentGUID);
CREATE INDEX Hardware_Class 
      ON dbo.Hardware (Class);
CREATE INDEX Hardware_DeviceDesc 
      ON dbo.Hardware (DeviceDesc);

CREATE TABLE [dbo].[HardwareLicenseRule](
	[ObjectGUID] [uniqueidentifier] NOT NULL,
	[ParentGUID] [uniqueidentifier] NOT NULL,
    [Name] [nvarchar](450) NOT NULL,
	[Class] [nvarchar](450) NULL,
	[DeviceDesc] [nvarchar](450) NULL,
	[Description] [nvarchar](4000)NULL,
	[MinMemory] [int] NULL,
	[MaxMemory] [int] NULL,
	[MinFreeMemory] [int] NULL,
	[MaxFreeMemory] [int] NULL,
	[MinFrequency] [int] NULL,
	[MaxFrequency] [int] NULL,
	[MinQuantity] [int]  NULL,
	[MaxQuantity] [int]  NULL,
	[CreatedForPassportGUID] [uniqueidentifier] NULL,
	[MaxDeviceAmount] [int] NULL,
	[Version]	[nvarchar](500) NULL,
	[BusWidth]	[nvarchar](500) NULL,
	[BusLength] [nvarchar](500) NULL,
	[Socket] [nvarchar](500) NULL,
	[Service] [nvarchar](100) NULL,	
	[Chipset] [nvarchar](450) NULL,
	[ReleaseDateFrom] [DateTime] NULL,
	[ReleaseDateTo] [DateTime] NULL,
	PRIMARY KEY CLUSTERED 
(
	[ObjectGUID] 
)WITH (PAD_INDEX  = OFF, STATISTICS_NORECOMPUTE  = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS  = ON, ALLOW_PAGE_LOCKS  = ON) ON [PRIMARY],
foreign key (ParentGUID) REFERENCES ObjectRelations(ObjectGUID)
ON UPDATE CASCADE
ON DELETE CASCADE
) ON [PRIMARY];

CREATE INDEX HardwareLicenseRule_ParentGUID 
      ON dbo.HardwareLicenseRule (ParentGUID);
CREATE INDEX HardwareLicenseRule_Name 
      ON dbo.HardwareLicenseRule (Name);
CREATE INDEX HardwareLicenseRule_Class 
      ON dbo.HardwareLicenseRule (Class);
CREATE INDEX HardwareLicenseRule_DeviceDesc 
      ON dbo.HardwareLicenseRule (DeviceDesc);

CREATE TABLE [dbo].[HardwareLicenseRuleLink](
	[ObjectGUID] [uniqueidentifier] NOT NULL,
    [ParentGUID] [uniqueidentifier] NOT NULL,
	[HardwareLicenseRuleGUID] [uniqueidentifier] NOT NULL,
foreign key (HardwareLicenseRuleGUID) REFERENCES HardwareLicenseRule(ObjectGUID),
foreign key (ParentGUID) REFERENCES Passport(ObjectGUID)
ON UPDATE CASCADE
ON DELETE CASCADE
) ON [PRIMARY];

CREATE INDEX HardwareLicenseRuleLink_ParentGUID 
      ON dbo.HardwareLicenseRuleLink (ParentGUID);
CREATE INDEX HardwareLicenseRuleLink_ObjectGUID 
      ON dbo.HardwareLicenseRuleLink (ObjectGUID);
CREATE INDEX HardwareLicenseRuleLink_HardwareLicenseRuleGUID 
      ON dbo.HardwareLicenseRuleLink (HardwareLicenseRuleGUID);

CREATE TABLE [dbo].[HardInstallation](
	[ObjectGUID] [uniqueidentifier] NOT NULL,
	[HardwareGUID] [uniqueidentifier] NOT NULL,
	[ParentGUID] [uniqueidentifier] NOT NULL,
	[CreationDate] [datetime] NOT NULL,
	[Mfg] [nvarchar](500) NULL,
	[FreeMemory] [bigint] NULL,
	[SerialNumber] [nvarchar](500) NULL,
	[UniversalUniqueID]	[uniqueidentifier] NULL,
	[CurrentUsage] [nvarchar](100) NULL,
	[ReleaseDate] [DateTime] NULL,
	[MAC] [nvarchar](450) NULL,
PRIMARY KEY CLUSTERED 
(
	[ObjectGUID] 
)WITH (PAD_INDEX  = OFF, STATISTICS_NORECOMPUTE  = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS  = ON, ALLOW_PAGE_LOCKS  = ON) ON [PRIMARY],
foreign key (HardwareGUID) REFERENCES Hardware(ObjectGUID),
foreign key (ParentGUID) REFERENCES Action(ObjectGUID)
ON UPDATE CASCADE
ON DELETE CASCADE
) ON [PRIMARY];

CREATE INDEX HardInstallation_HardwareGUID 
      ON dbo.HardInstallation (HardwareGUID);
CREATE INDEX HardInstallation_ParentGUID 
      ON dbo.HardInstallation (ParentGUID);

CREATE TABLE [dbo].[FilesFound](
	[ObjectGUID] [uniqueidentifier] NOT NULL,
	[ParentGUID] [uniqueidentifier] NOT NULL,
	[CreationDate] [datetime] NOT NULL,
	[FileMask] [nvarchar](300) NOT NULL, 
	[FilePath] [nvarchar](300) NOT NULL,
	[Size]    [bigint] NULL,
PRIMARY KEY CLUSTERED 
(
	[ObjectGUID] 
)WITH (PAD_INDEX  = OFF, STATISTICS_NORECOMPUTE  = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS  = ON, ALLOW_PAGE_LOCKS  = ON) ON [PRIMARY],
foreign key (ParentGUID) REFERENCES Action(ObjectGUID)
ON UPDATE CASCADE
ON DELETE CASCADE
) ON [PRIMARY];

CREATE INDEX FilesFound_ParentGUID 
      ON dbo.FilesFound (ParentGUID);
CREATE INDEX FilesFound_FileMask 
      ON dbo.FilesFound (FileMask);
CREATE INDEX FilesFound_FilePath 
      ON dbo.FilesFound (FilePath);


CREATE TABLE [dbo].[InspectionTypeLU](
	[ID] [tinyint] NOT NULL,
	[Name] [nvarchar](50) NOT NULL
) ON [PRIMARY];

CREATE TABLE [dbo].[ResultCodeLU](
	[ID] [tinyint] NOT NULL,
	[CodeDescription] [nvarchar](20) NOT NULL
) ON [PRIMARY];

CREATE TABLE [dbo].[SoftTypeLU](
	[ID] [tinyint] NOT NULL,
	[SoftDescription] [nvarchar](50) NOT NULL,
	[ReportSortOrder] [tinyint] NOT NULL
) ON [PRIMARY];

CREATE TABLE [dbo].[InspectionListParamLU](
	[ID] [tinyint] NOT NULL,
	[InspectionListParam] [nvarchar](50) NULL,
	[ResultCode1] [tinyint] NOT NULL,
	[ResultCode2] [tinyint] NOT NULL
) ON [PRIMARY];

CREATE TABLE [dbo].[ActionTypeLU](
	[ID] [tinyint] NOT NULL,
	[ActionDescription] [nvarchar](50) NOT NULL
) ON [PRIMARY];

CREATE TABLE [dbo].[SecurityTypeLU](
	[ID] [tinyint] NOT NULL,
	[SecurityType] [nvarchar](300) NOT NULL
) ON [PRIMARY];

CREATE TABLE [dbo].[Report](
	[ObjectGUID] [uniqueidentifier] NOT NULL,
	[ReportType] [nvarchar](70) NOT NULL,
	[ParameterList] [nvarchar](700) NULL
PRIMARY KEY CLUSTERED 
(
	[ObjectGUID] 
)WITH (PAD_INDEX  = OFF, STATISTICS_NORECOMPUTE  = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS  = ON, ALLOW_PAGE_LOCKS  = ON) ON [PRIMARY],
foreign key (ObjectGUID) REFERENCES ObjectRelations(ObjectGUID)
ON UPDATE CASCADE
ON DELETE CASCADE
) ON [PRIMARY];

CREATE TABLE [dbo].[ProhibitionRule](
	[ObjectGUID] [uniqueidentifier] NOT NULL,
	[ParentGUID] [uniqueidentifier] NOT NULL,
    [Name] [nvarchar](450) NOT NULL,
	[ProductName] [nvarchar](450)NULL,
    [Publisher] [nvarchar](450)NULL,
	[Localization] [nvarchar](50)NULL,
	[ProductVersion] [nvarchar](450)NULL,
	[ProductKey] [nvarchar](300)NULL,
	[Description] [nvarchar](4000)NULL,
	[SoftType] [tinyint] NULL,
PRIMARY KEY CLUSTERED 
(
	[ObjectGUID] 
)WITH (PAD_INDEX  = OFF, STATISTICS_NORECOMPUTE  = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS  = ON, ALLOW_PAGE_LOCKS  = ON) ON [PRIMARY],
foreign key (ParentGUID) REFERENCES ObjectRelations(ObjectGUID)
ON UPDATE CASCADE
ON DELETE CASCADE
) ON [PRIMARY];

CREATE INDEX ProhibitionRule_ParentGUID 
      ON dbo.ProhibitionRule (ParentGUID);
CREATE INDEX ProhibitionRule_Name 
      ON dbo.ProhibitionRule (Name);
CREATE INDEX ProhibitionRule_ProductName 
      ON dbo.ProhibitionRule (ProductName);
CREATE INDEX ProhibitionRule_Publisher 
      ON dbo.ProhibitionRule (Publisher);      

CREATE TABLE [dbo].[HardwareProhibitionRule](
	[ObjectGUID] [uniqueidentifier] NOT NULL,
	[DeviceDesc] [nvarchar](450)NULL,
    [Class] [nvarchar](450)NULL,
	[Description] [nvarchar](4000)NULL,
PRIMARY KEY CLUSTERED 
(
	[ObjectGUID] 
)WITH (PAD_INDEX  = OFF, STATISTICS_NORECOMPUTE  = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS  = ON, ALLOW_PAGE_LOCKS  = ON) ON [PRIMARY],
foreign key (ObjectGUID) REFERENCES ObjectRelations(ObjectGUID)
ON UPDATE CASCADE
ON DELETE CASCADE
) ON [PRIMARY];


CREATE TABLE [dbo].[StatusLU](
	[ID] [tinyint] NOT NULL,
	[StatusDescription] [nvarchar](50) NOT NULL
) ON [PRIMARY];

CREATE TABLE [dbo].[DBMigration](
    [InsertOrder] [int] IDENTITY(1,1), 
	[ObjectType] [nvarchar](450) NOT NULL, 
	[IsParentContainer] [bit], 
	[AttrList] [nvarchar](max) NOT NULL,   
	[NewAttrValuesList] [nvarchar](4000), 
	[WithObjRel] [bit],
	[MigrationPart] [nvarchar](100) NOT NULL
) ON [PRIMARY];

CREATE TABLE [dbo].[ProductList](
	[ID] [int] NOT NULL,
	[ProductName] [nvarchar](450) NOT NULL
) ON [PRIMARY];

CREATE TABLE [dbo].[Languages](
	[ID] [int] NOT NULL,
	[LanguageDesc] [nvarchar](450) NOT NULL
) ON [PRIMARY];

CREATE TABLE [dbo].[ChipsetInfo](
     [Vendor] [nvarchar](150) NULL,
     [ChipsetName] [nvarchar](150) NULL,
     [Northbridge] [nvarchar](150) NULL,
     [ChipsetCodename] [nvarchar](150) NULL
) ON [PRIMARY];

-- В таблице [dbo].[ChipsetInfo] поля ChipsetName и Northbridge не могут быть оба пустыми.
-- Если при создании базы вы получили ошибку с именем этого ограничения (CK_ChipsetInfo), то проверьте
-- правильность заполнения xml-данных о чипсетах в InsertChipsetLibrary.sql и ocrcfg/ChipsetInfo.xml
ALTER TABLE [dbo].[ChipsetInfo]  WITH CHECK ADD  CONSTRAINT [CK_ChipsetInfo] CHECK  ((NOT (([ChipsetName] IS NULL OR len([ChipsetName])=(0)) AND ([Northbridge] IS NULL OR len([Northbridge])=(0)))))
ALTER TABLE [dbo].[ChipsetInfo] CHECK CONSTRAINT [CK_ChipsetInfo]

CREATE INDEX Languages_LanguageDesc 
      ON dbo.Languages (LanguageDesc); 





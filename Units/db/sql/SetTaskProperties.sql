CREATE PROCEDURE [dbo].[SetTaskProperties] 
(@ObjectGUID nvarchar(50), @XMLObject nvarchar(max))

AS
BEGIN
	 
	SET NOCOUNT ON;

declare @Object_GUID  nvarchar(50);
declare @XMLDoc int;
declare @Message  nvarchar(1000);
declare @Check int;
declare @AttrNames nvarchar(4000);
declare @ForExecute  nvarchar(4000);
declare @Length int;  
declare @String nvarchar(2000);

declare @ObjectName nvarchar(450);
declare @Login  nvarchar(150);
declare @Domain nvarchar(300);
declare @SyncDN nvarchar(300);
declare @SyncSchedule nvarchar(max);
declare @SearchFileMask nvarchar(300);
declare @SearchPath nvarchar(300);
declare @SharedFolder nvarchar(300);
declare @ReportList nvarchar(300);
declare @InspectionType smallint;
declare @InspectionOptions int;
declare @UseAlternativeCredentials bit;
declare @LastInspectTime datetime;
declare @CharNextInspectTime nvarchar(150);
declare @NextInspectTime datetime;
declare @SearchFilesLimit int;
declare @Timeout int;
declare @InspectionStoringPeriod int;
declare @MailTo nvarchar(50);
declare @SendMail bit; 
declare @ScanWithAgent bit;
declare @Schedule nvarchar(max);
declare @ScanUninspectedOnly bit;
declare @SearchFiles bit;
declare @LimitFiles bit;
declare @UseDNSubChildren bit;
declare @ProgramComponentsSearch bit;
declare @ProgramComponentsSearchMasks nvarchar(300);
declare @ProgramComponentsSearchDiskList nvarchar(300);
declare @ProgramComponentsSearchExcludeSystemDir bit;
declare @ProgramComponentsSearchExcludeHiddenOrSystem bit;
declare @ProgramComponentsSearchLastModifiedDate datetime;
declare @ScanWithAgentTimeout int;
declare @AlternativeScan bit;
declare @CheckHeader bit;
declare @IncrementalSearch bit;
declare @SNChanged bigint;
declare @SearchFilesModifiedDate datetime; 
declare @SearchFilesMinSize bigint; 
declare @SearchFilesMaxSize bigint;
declare @IsSearchFilesByModifiedDate bit;
declare @IsSearchFilesByRangeSizes bit;
declare @ProgramComponentsLimitFiles int;
declare @IsProgramComponentsLimitFiles bit;


SET DATEFORMAT ymd;

BEGIN TRY

    set @XMLObject = RTRIM(@XMLObject);

    EXEC sp_xml_preparedocument @XMLDoc OUTPUT, @XMLObject;

END TRY
BEGIN CATCH

    if (Error_Message()  is NULL or Error_Message() = '')
	   set @Message = 'Ошибка при чтении XML-документа';
	else
	   set @Message = Error_Message();
	   
    RAISERROR(@Message, 12, 1);
    Return -1;

END CATCH



BEGIN TRY

BEGIN TRANSACTION

    set @Check = 0;


        select *
        into #TaskTbl
        from openxml(@XMLDoc, '/Object', 1)
        with (  Name  nvarchar(450), 
                Login  nvarchar(150),
                SyncDN nvarchar(300),
                SyncSchedule nvarchar(max),
                SearchFileMask nvarchar(300),
                SearchPath nvarchar(300),
                Domain nvarchar(300),
                SharedFolder nvarchar(300),
                ReportList nvarchar(300),
                InspectionType smallint,
                InspectionOptions int,
                UseAlternativeCredentials bit,
                LastInspectTime datetime,
                NextInspectTime nvarchar(150),
                SearchFilesLimit int,
                Timeout int,
                InspectionStoringPeriod int,
                MailTo nvarchar(50),
                SendMail bit, 
                ScanWithAgent bit,
                Schedule nvarchar(max),
                ScanUninspectedOnly bit,
                SearchFiles bit,
                LimitFiles bit,
                UseDNSubChildren bit,
                ProgramComponentsSearch bit,
				ProgramComponentsSearchMasks nvarchar(300),
				ProgramComponentsSearchDiskList nvarchar(300),
				ProgramComponentsSearchExcludeSystemDir bit,
				ProgramComponentsSearchExcludeHiddenOrSystem bit,
				ProgramComponentsSearchLastModifiedDate datetime,
				ScanWithAgentTimeout int,
                AlternativeScan bit,
                CheckHeader bit,
                IncrementalSearch bit,
				SNChanged bigint,
				SearchFilesModifiedDate datetime, 
	            SearchFilesMinSize bigint, 
	            SearchFilesMaxSize bigint,
				IsSearchFilesByModifiedDate bit,
	            IsSearchFilesByRangeSizes bit,
				ProgramComponentsLimitFiles int,
	            IsProgramComponentsLimitFiles bit);
                
                
        select  @ProgramComponentsSearch = ProgramComponentsSearch,
				@ProgramComponentsSearchMasks = ProgramComponentsSearchMasks,
				@ProgramComponentsSearchDiskList = ProgramComponentsSearchDiskList,
				@ProgramComponentsSearchExcludeSystemDir = ProgramComponentsSearchExcludeSystemDir,
				@ProgramComponentsSearchExcludeHiddenOrSystem = ProgramComponentsSearchExcludeHiddenOrSystem,
				@ProgramComponentsSearchLastModifiedDate = ProgramComponentsSearchLastModifiedDate,
				@ObjectName = Name, @Login = Login, @SyncDN = SyncDN, @SyncSchedule = SyncSchedule,
				@SearchFileMask = SearchFileMask, @SearchPath = SearchPath, @Domain = Domain, 
				@InspectionType = InspectionType, @InspectionOptions = InspectionOptions, 
				@UseAlternativeCredentials = UseAlternativeCredentials, @SearchFilesLimit = SearchFilesLimit, 
				@Timeout = Timeout, @LastInspectTime = nullif(LastInspectTime, ''), 
				@CharNextInspectTime = NextInspectTime,
				@InspectionStoringPeriod = InspectionStoringPeriod,
				@MailTo = MailTo, @SendMail = SendMail, @SharedFolder = SharedFolder,
				@ReportList = ReportList, @ScanWithAgent = ScanWithAgent,
				@Schedule = Schedule, @ScanUninspectedOnly = ScanUninspectedOnly,
				@SearchFiles = SearchFiles, @LimitFiles = LimitFiles,
				@UseDNSubChildren = UseDNSubChildren, @ScanWithAgentTimeout = ScanWithAgentTimeout,
				@AlternativeScan = AlternativeScan, @CheckHeader = CheckHeader, @IncrementalSearch = IncrementalSearch,
				@SNChanged = SNChanged, @SearchFilesModifiedDate = SearchFilesModifiedDate, @SearchFilesMinSize = SearchFilesMinSize,
	            @SearchFilesMaxSize = SearchFilesMaxSize, @IsSearchFilesByModifiedDate  = IsSearchFilesByModifiedDate,
                @IsSearchFilesByRangeSizes = IsSearchFilesByRangeSizes, @ProgramComponentsLimitFiles = ProgramComponentsLimitFiles,
	            @IsProgramComponentsLimitFiles = IsProgramComponentsLimitFiles
        from    #TaskTbl;
        
        
        
        set @Login = Replace(@Login, '''', '''''');
        set @ReportList = Replace(@ReportList, '''', ''''''); 
        set @MailTo = Replace(@MailTo, '''', '''''');
        set @SyncDN = Replace(@SyncDN, '''', ''''''); 
        set @SyncSchedule = Replace(@SyncSchedule, '''', ''''''); 
        set @SearchFileMask = Replace(@SearchFileMask, '''', '''''');
        set @SearchPath = Replace(@SearchPath, '''', ''''''); 
        set @Domain = Replace(@Domain, '''', '''''');
        set @SharedFolder = Replace(@SharedFolder, '''', ''''''); 
        set @Schedule = Replace(@Schedule, '''', ''''''); 


        set @AttrNames = '';

        if (@Login is not NULL)
        begin
        set @AttrNames = @AttrNames +  ',  Login = '  + '''' + @Login + '''';
        end 

        --if (@Password is not NULL)
        --begin
        --set @AttrNames = @AttrNames +  ',  Password = '  + '''' + @Password + '''';
        --end

        if (@ProgramComponentsSearch  is not NULL)
        begin
           set @String = @ProgramComponentsSearch;
           set @AttrNames = @AttrNames +  ',  ProgramComponentsSearch = '  + @String;
        end
        
        if (@ProgramComponentsSearchExcludeSystemDir  is not NULL)
        begin
           set @String = @ProgramComponentsSearchExcludeSystemDir;
           set @AttrNames = @AttrNames +  ',  ProgramComponentsSearchExcludeSystemDir = '  + @String;
        end
        
        if (@ProgramComponentsSearchExcludeHiddenOrSystem  is not NULL)
        begin
           set @String = @ProgramComponentsSearchExcludeHiddenOrSystem;
           set @AttrNames = @AttrNames +  ',  ProgramComponentsSearchExcludeHiddenOrSystem = '  + @String;
        end
        
        if (@ProgramComponentsSearchMasks is not NULL)
        begin
        set @AttrNames = @AttrNames +  ',  ProgramComponentsSearchMasks = '  + '''' + @ProgramComponentsSearchMasks + '''';
        end 
        
        if (@ProgramComponentsSearchDiskList is not NULL)
        begin
        set @AttrNames = @AttrNames +  ',  ProgramComponentsSearchDiskList = '  + '''' + @ProgramComponentsSearchDiskList + '''';
        end
        
        if (@ReportList is not NULL)
        begin
        set @AttrNames = @AttrNames +  ',  ReportList = '  + '''' + @ReportList + '''';
        end 
        
        if (@MailTo is not NULL)
        begin
        set @AttrNames = @AttrNames +  ',  MailTo = '  + '''' + @MailTo + '''';
        end

        if (@SyncDN is not NULL)
        begin
        set @AttrNames = @AttrNames +  ',  SyncDN = '  + '''' + @SyncDN + '''';
        end 

        if (@SyncSchedule is not NULL)
        begin
        set @AttrNames = @AttrNames +  ',  SyncSchedule = '  + '''' + @SyncSchedule + '''';
        end

        if (@SearchFileMask is not NULL)
        begin
        set @AttrNames = @AttrNames +  ',  SearchFileMask = '  + '''' + @SearchFileMask + '''';
        end 

        if (@SearchPath is not NULL)
        begin
        set @AttrNames = @AttrNames +  ',  SearchPath = '  + '''' + @SearchPath + '''';
        end

        if (@Domain is not NULL)
        begin
        set @AttrNames = @AttrNames +  ',  Domain = '  + '''' + @Domain + '''';
        end 
        
        if (@SharedFolder is not NULL)
        begin
        set @AttrNames = @AttrNames +  ',  SharedFolder = '  + '''' + @SharedFolder + '''';
        end

        if (@Schedule is not NULL)
        begin
        set @AttrNames = @AttrNames +  ',  Schedule = '  + '''' + @Schedule + '''';
        end

        if (@InspectionType is not NULL)
        begin
           set @String = @InspectionType;
           set @AttrNames = @AttrNames +  ',  InspectionType = '  + @String;
        end

        if (@InspectionOptions is not NULL)
        begin
           set @String = @InspectionOptions;
           set @AttrNames = @AttrNames +  ',  InspectionOptions = '  + @String;
        end

        if (@UseAlternativeCredentials  is not NULL)
        begin
           set @String = @UseAlternativeCredentials;
           set @AttrNames = @AttrNames +  ',  UseAlternativeCredentials = '  + @String;
        end
        
        if (@SendMail  is not NULL)
        begin
           set @String = @SendMail;
           set @AttrNames = @AttrNames +  ',  SendMail = '  + @String;
        end
        
        if (@ScanWithAgent  is not NULL)
        begin
           set @String = @ScanWithAgent;
           set @AttrNames = @AttrNames +  ',  ScanWithAgent = '  + @String;
        end

        if (@ScanUninspectedOnly  is not NULL)
        begin
           set @String = @ScanUninspectedOnly;
           set @AttrNames = @AttrNames +  ',  ScanUninspectedOnly = '  + @String;
        end

        if (@SearchFiles  is not NULL)
        begin
           set @String = @SearchFiles;
           set @AttrNames = @AttrNames +  ',  SearchFiles = '  + @String;
        end

        if (@LimitFiles  is not NULL)
        begin
           set @String = @LimitFiles;
           set @AttrNames = @AttrNames +  ',  LimitFiles = '  + @String;
        end

        if (@UseDNSubChildren  is not NULL)
        begin
           set @String = @UseDNSubChildren;
           set @AttrNames = @AttrNames +  ',  UseDNSubChildren = '  + @String;
        end

        if (@SearchFilesLimit  is not NULL)
        begin
           set @String = @SearchFilesLimit;
           set @AttrNames = @AttrNames +  ',  SearchFilesLimit = '  + @String;
        end
        
        if (@InspectionStoringPeriod  is not NULL)
        begin
           set @String = @InspectionStoringPeriod;
           set @AttrNames = @AttrNames +  ',  InspectionStoringPeriod = '  + @String;
        end

		if (@ProgramComponentsLimitFiles  is not NULL)
        begin
           set @String = @ProgramComponentsLimitFiles;
           set @AttrNames = @AttrNames +  ',  ProgramComponentsLimitFiles = '  + @String;
        end
	    
		if (@IsProgramComponentsLimitFiles  is not NULL)
        begin
           set @String = @IsProgramComponentsLimitFiles;
           set @AttrNames = @AttrNames +  ',  IsProgramComponentsLimitFiles = '  + @String;
        end
        
        if (@Timeout  is not NULL)
        begin
           set @String = @Timeout;
           set @AttrNames = @AttrNames +  ',  Timeout = '  + @String;
        end
        
        if (@ScanWithAgentTimeout  is not NULL)
        begin
           set @String = @ScanWithAgentTimeout;
           set @AttrNames = @AttrNames +  ',  ScanWithAgentTimeout = '  + @String;
        end
        
        if (@AlternativeScan  is not NULL)
        begin
           set @String = @AlternativeScan;
           set @AttrNames = @AttrNames +  ',  AlternativeScan = '  + @String;
        end
        
        if (@CheckHeader  is not NULL)
        begin
           set @String = @CheckHeader;
           set @AttrNames = @AttrNames +  ',  CheckHeader = '  + @String;
        end
        
        if (@IncrementalSearch  is not NULL)
        begin
           set @String = @IncrementalSearch;
           set @AttrNames = @AttrNames +  ',  IncrementalSearch = '  + @String;
        end 		
		
		if (@SNChanged  is not NULL)
        begin
           set @String = @SNChanged;
           set @AttrNames = @AttrNames +  ',  SNChanged = '  + @String;
        end  

		if (@SearchFilesMinSize  is not NULL)
        begin
           set @String = @SearchFilesMinSize;
           set @AttrNames = @AttrNames +  ',  SearchFilesMinSize = '  + @String;
        end

		if (@SearchFilesMaxSize  is not NULL)
        begin
           set @String = @SearchFilesMaxSize;
           set @AttrNames = @AttrNames +  ',  SearchFilesMaxSize = '  + @String;
        end

		if (@IsSearchFilesByRangeSizes  is not NULL)
        begin
           set @String = @IsSearchFilesByRangeSizes;
           set @AttrNames = @AttrNames +  ',  IsSearchFilesByRangeSizes = '  + @String;
        end

		if (@IsSearchFilesByModifiedDate  is not NULL)
        begin
           set @String = @IsSearchFilesByModifiedDate;
           set @AttrNames = @AttrNames +  ',  IsSearchFilesByModifiedDate = '  + @String;
        end

        
        if (substring(@AttrNames, 1, 1)= ',')
        begin
        set @Length = LEN(@AttrNames) - 1;
        set @AttrNames = Right(@AttrNames, @Length);
        end
        
        if (@AttrNames <> '')
        begin

            set @ForExecute = 'update Task  set ' + @AttrNames + ' where ObjectGUID = ' + '''' + @ObjectGUID + '''';
            execute (@ForExecute);
             if (@@Error <> 0)  set @Check = -1;
        
        end

       --   Time parameters set separately cause  under conversion to string seconds are lost
        
        if (@ProgramComponentsSearchLastModifiedDate is not NULL)
        begin
        update Task   set ProgramComponentsSearchLastModifiedDate = @ProgramComponentsSearchLastModifiedDate
        where ObjectGUID = @ObjectGUID;
         if (@@Error <> 0)  set @Check = -1;
        end
        
        if (@LastInspectTime is not NULL)
        begin
        update Task   set LastInspectTime = @LastInspectTime
        where ObjectGUID = @ObjectGUID;
         if (@@Error <> 0)  set @Check = -1;
        end

		if (@SearchFilesModifiedDate is not NULL)
        begin
        update Task   set SearchFilesModifiedDate = @SearchFilesModifiedDate
        where ObjectGUID = @ObjectGUID;
         if (@@Error <> 0)  set @Check = -1;
        end
        
        if (@CharNextInspectTime is not NULL)
        begin
		        if(@CharNextInspectTime = '')
				begin
					update Task   set NextInspectTime = NULL  where ObjectGUID = @ObjectGUID;
					if (@@Error <> 0)  set @Check = -1;
                end
				else
				begin

				    if(isdate(@CharNextInspectTime) = 1)
						set @NextInspectTime = @CharNextInspectTime;
					else
						set @NextInspectTime = NULL;

                    update Task   set NextInspectTime = @NextInspectTime  where ObjectGUID = @ObjectGUID;
					if (@@Error <> 0)  set @Check = -1;

				end

        end
        
        if (@ObjectName is not NULL)
        begin
        update ObjectRelations
        set Name = @ObjectName
        where ObjectGUID = @ObjectGUID;
         if (@@Error <> 0)  set @Check = -1;
        end 

    if (@Check = 0 )
            COMMIT TRANSACTION
    else 
    begin 
            RAISERROR('Транзакция отменена', 16, 1);
    end 
    
    EXEC sp_xml_removedocument @XMLDoc;                      

END TRY
BEGIN CATCH

    EXEC sp_xml_removedocument @XMLDoc;
    
    if (Error_Message()  is NULL or Error_Message() = '')
	   set @Message = 'Ошибка БД при изменении свойств задания';
	else
	   set @Message = Error_Message();
	   
    ROLLBACK TRANSACTION
    RAISERROR(@Message, 12, 1);
    Return -1;

END CATCH 

END

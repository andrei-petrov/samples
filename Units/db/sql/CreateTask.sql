CREATE PROCEDURE [dbo].[CreateTask] 
(@XMLObject nvarchar(max))

AS
BEGIN
	
	SET NOCOUNT ON;

declare @ObjectGUID  nvarchar(50);
declare @ParentGUID  nvarchar(50);
declare @Name  nvarchar(450);
declare @ObjectType  nvarchar(100);
declare @Login  nvarchar(150);
declare @SyncDN nvarchar(300);
declare @SearchFileMask nvarchar(300);
declare @InspectionType smallint;
declare @UseAlternativeCredentials bit;
declare @SearchFilesLimit int;
declare @SendMail bit;
declare @ScanWithAgent bit;
declare @ScanUninspectedOnly bit;
declare @SearchFiles bit;
declare @LimitFiles bit;
declare @UseDNSubChildren bit;
declare @Timeout int;
declare @ScanWithAgentTimeout int;
declare @AlternativeScan bit;

declare @XMLDoc int;
declare @Message  nvarchar(1000);
declare @Check int;
declare @AddingDate datetime;
declare @Object_Count int;


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

    select * into #ObjectInfoTbl
    from openxml(@XMLDoc, '/Object', 1)
    with (ObjectGUID  nvarchar(50),
          ParentGUID  nvarchar(50),
          Name  nvarchar(450),
          Login  nvarchar(150),
          Password  varbinary(256),
          SyncDN nvarchar(300),
          SyncSchedule nvarchar(max),
          SearchFileMask nvarchar(300),
          SearchPath nvarchar(300),
          Domain nvarchar(300),
          ReportList nvarchar(300),
          SharedFolder nvarchar(300),
          InspectionType smallint,
          InspectionOptions int,
          UseAlternativeCredentials bit,
          SearchFilesLimit int,

          LastInspectTime datetime,
          NextInspectTime datetime,

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
          CheckHeader  bit,
          IncrementalSearch  bit,
		  SNChanged bigint,
		  
		  SearchFilesModifiedDate datetime, 
	      SearchFilesMinSize bigint, 
	      SearchFilesMaxSize bigint,
		  IsSearchFilesByModifiedDate bit,
	      IsSearchFilesByRangeSizes bit,
		  ProgramComponentsLimitFiles int,
	      IsProgramComponentsLimitFiles bit);
 
    
    select @ObjectGUID = ObjectGUID, @ParentGUID = ParentGUID,
    @Name = Name, @SendMail = SendMail,
    @ScanWithAgent = ScanWithAgent, @Login = Login,
    @SyncDN = SyncDN, @SearchFileMask = SearchFileMask,          
    @InspectionType = InspectionType, @UseAlternativeCredentials = UseAlternativeCredentials, 
    @Timeout = Timeout, @ScanUninspectedOnly = ScanUninspectedOnly,
    @SearchFiles = SearchFiles, @LimitFiles = LimitFiles,
    @UseDNSubChildren = UseDNSubChildren, @ScanWithAgentTimeout = ScanWithAgentTimeout,
    @AlternativeScan = AlternativeScan
    from #ObjectInfoTbl;
 

if (@ObjectGUID = '' or @ObjectGUID is NULL)
begin
    Set @Message = 'GUID объекта не определен ';
    RAISERROR(@Message, 12, 1);
    Return -1;
end

if (@ParentGUID = '' or @ParentGUID is NULL)
begin
    Set @Message = 'GUID объекта-родителя не определен ';
    RAISERROR(@Message, 12, 1);
    Return -1;
end

if (@Name = '' or @Name is NULL)
begin
    Set @Message = 'Имя не определено ';
    RAISERROR(@Message, 12, 1);
    Return -1;
end

if ( (@Login = '' or @Login is NULL)  and (@UseAlternativeCredentials = 1))
begin
    Set @Message = 'Login не задан.';
    RAISERROR(@Message, 12, 1);
    Return -1;
end
    

if (@Timeout is NULL)
begin
    Set @Timeout = 30;
end

if (@ScanWithAgent is NULL)
begin
    Set @ScanWithAgent = 0;
end

if (@InspectionType is NULL)
begin
    Set @Message = 'Тип инспектирования не определен ';
    RAISERROR(@Message, 12, 1);
    Return -1;
end

if (@UseAlternativeCredentials  is NULL)
begin
    Set @Message = 'Не определено, будет ли использоваться дополнительная учетная запись';
    RAISERROR(@Message, 12, 1);
    Return -1;
end

if (@SendMail  is NULL)
begin
    Set @SendMail = 0;
end

if (@SyncDN is NULL)
begin
    Set @SyncDN = '';
end

if (@SearchFileMask is NULL)
begin
    Set @SearchFileMask = '';
end

if (@ScanUninspectedOnly is NULL)
begin
    Set @ScanUninspectedOnly = 0;
end

if (@SearchFiles is NULL)
begin
    Set @SearchFiles = 0;
end

if (@LimitFiles is NULL)
begin
    Set @LimitFiles = 0;
end

if (@UseDNSubChildren is NULL)
begin
    Set @UseDNSubChildren = 0;
end

if (@ScanWithAgentTimeout is NULL)
begin
    Set @ScanWithAgentTimeout = 1200;
end

if (@AlternativeScan is NULL)
begin
    Set @AlternativeScan = 0;
end

select @ObjectType = ObjectType from ObjectRelations
where ObjectGUID = @ParentGUID;

if(@ObjectType <> 'TaskContainer')
begin
    Set @Message = 'Тип объекта-родителя должен быть TaskContainer';
	RAISERROR(@Message, 12, 1);
	Return -1;
end 


          set @Check = 0; 

          select @Object_Count = Count(*) from ObjectRelations
          where ObjectRelations.ObjectGUID = @ObjectGUID;

          if (@Object_Count > 0)
          begin
                RAISERROR('Объект с таким GUID уже существует в БД', 16, 1);
                Return -1;
                end
          else
          begin
          
               select @Object_Count = Count(*) from ObjectRelations R
               where R.ObjectType = 'Task' and R.Name = @Name;
               
               if (@Object_Count > 0)
               begin
                    RAISERROR('Задание с таким именем уже существует', 16, 1);
                    Return -1;
               end
               else
               begin
                    set @AddingDate = getutcdate();
                    insert into dbo.ObjectRelations values(@ObjectGUID, 'Task', @Name, @ParentGUID, @AddingDate);  
                    if (@@Error <> 0)set @Check = -1;
                    
                    insert into dbo.Task 
                    select @ObjectGUID, @Login, Password, Domain, @SearchFileMask, SearchPath, @SyncDN, SyncSchedule,
                           @InspectionType, InspectionOptions, @UseAlternativeCredentials,  SearchFilesLimit, @Timeout, 
                           NextInspectTime, LastInspectTime, InspectionStoringPeriod, MailTo, @SendMail, SharedFolder, 
                           ReportList, @ScanWithAgent, Schedule, @ScanUninspectedOnly, @SearchFiles, @LimitFiles, @UseDNSubChildren,
                           ProgramComponentsSearch, ProgramComponentsSearchMasks, ProgramComponentsSearchDiskList,
                           ProgramComponentsSearchExcludeSystemDir, ProgramComponentsSearchExcludeHiddenOrSystem, ProgramComponentsSearchLastModifiedDate,
                           @ScanWithAgentTimeout, @AlternativeScan, CheckHeader, IncrementalSearch, SNChanged, SearchFilesModifiedDate, SearchFilesMinSize, SearchFilesMaxSize,
						   IsSearchFilesByModifiedDate, IsSearchFilesByRangeSizes, ProgramComponentsLimitFiles, IsProgramComponentsLimitFiles 
                    from   #ObjectInfoTbl;  
                    if (@@Error <> 0) set @Check = -1;   

                    if (@Check = 0)
                    COMMIT TRANSACTION
                    else 
                    begin 
                         RAISERROR('Транзакция отменена', 16, 1);
                    end
               end     
         end
         
         EXEC sp_xml_removedocument @XMLDoc;


END TRY
BEGIN CATCH

    EXEC sp_xml_removedocument @XMLDoc;
    
    if (Error_Message()  is NULL or Error_Message() = '')
	   set @Message = 'Ошибка БД при создании задания';
	else
	   set @Message = Error_Message();
   
    ROLLBACK TRANSACTION
    RAISERROR(@Message, 16, 1);
    Return -1;

END CATCH

END
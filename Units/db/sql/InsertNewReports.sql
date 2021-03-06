CREATE PROCEDURE [dbo].[InsertNewReports] 
(@XMLReportList nvarchar(max))
-- Adding installed soft information from XML 
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;

declare @XML int;
declare @AddingDate  Datetime;
declare @Message  nvarchar(1000);
declare @ReportAmount int;
declare @ReportGUID  uniqueidentifier;
declare @GeneralReportContainerGUID  uniqueidentifier;
declare @ReportContainerGUID  uniqueidentifier;
declare @ReportContainerName  nvarchar(450);
declare @I int;

declare @Check int;
set @Check = 0;

-- Creating of help document @XML,  reading a data of the researched computer from XML  --
-------------------------------------------------------------------------------------------------------
SET DATEFORMAT ymd;

BEGIN TRY

    BEGIN TRANSACTION

    set @XMLReportList = RTRIM(@XMLReportList);

    EXEC sp_xml_preparedocument @XML OUTPUT, @XMLReportList;

    CREATE TABLE #XMLReportTable(
	                      [ID] [int] IDENTITY(1,1)  NOT NULL,
	                      [ReportFolder]  [nvarchar](300) collate Cyrillic_General_CI_AS  NOT NULL,
                          [VisibleName]    [nvarchar](450) collate Cyrillic_General_CI_AS  NOT NULL,
                          [ReportType]    [nvarchar](300) collate Cyrillic_General_CI_AS  NOT NULL,
                          [ParameterList]   [nvarchar](300) collate Cyrillic_General_CI_AS  NOT NULL
                     ) ON [PRIMARY]; 

    insert into #XMLReportTable
    select * 
    from openxml(@XML, '/deploy-info/reports/report', 1)
    with (report_folder  nvarchar(300),
            visible_name    nvarchar(450),
            report_type     nvarchar(300),
            parameter_list  nvarchar(300));               

    set @AddingDate = getutcdate();

    select @ReportAmount = Count(*) from #XMLReportTable;

    if (@ReportAmount <1 or @ReportAmount is null)
    begin
              RAISERROR ('Список отчетов пуст', 12, 1);
    end
    else
    begin  
                 
                       select @GeneralReportContainerGUID = ObjectGUID from ObjectRelations
                       where Name = 'Отчеты' and ObjectType = 'ReportContainer';

                       set @I = 1;
        
                       while (@I <= @ReportAmount )
                       begin
                               
                               set @ReportGUID = NULL;

                               select @ReportGUID = ObjectGUID  from ObjectRelations R, #XMLReportTable RT
                               where R.Name = RT.VisibleName  and R.ObjectType =  'Report' and RT.ID = @I;
                      
                                if (@ReportGUID is NULL)
                                begin
                
                                          set @ReportGUID = NEWID();
                                 
                                          set @ReportContainerGUID = NULL;
                                          select @ReportContainerGUID = ObjectGUID  from ObjectRelations R, #XMLReportTable RT
                                          where R.Name = RT.ReportFolder and R.ObjectType =  'ReportContainer'  and RT.ID = @I;

                                           if (@ReportContainerGUID is NULL)
                                           begin

                                                   set @ReportContainerGUID = NEWID(); 
                                                   
                                                   select @ReportContainerName = ReportFolder from  #XMLReportTable where ID = @I;  

                                                   insert into dbo.ObjectRelations values(@ReportContainerGUID, 'ReportContainer', @ReportContainerName, @GeneralReportContainerGUID, @AddingDate)
                                                   if (@@Error <> 0)   set @Check = -1; 

                                           end

                                          insert into dbo.ObjectRelations 
                                          select  @ReportGUID, 'Report', VisibleName, @ReportContainerGUID, @AddingDate from #XMLReportTable RT
                                          where RT.ID = @I; 
                                          if (@@Error <> 0)   set @Check = -1;                       
           
                                          insert into dbo.Report
                                          select @ReportGUID, ReportType, ParameterList
                                          from #XMLReportTable  where ID = @I;
                                          if (@@Error <> 0)   set @Check = -1; 
                     
                                end      
                                    
                           set @I = @I+1;

                  end 
     end
                        

     if (@Check = 0)
                     COMMIT TRANSACTION
     else 
     begin 
                      Set @Message = 'Транзакция отменена';
                      RAISERROR(@Message, 12, 1);
                      Return -1;
     end
     
     EXEC sp_xml_removedocument @XML;

END TRY
BEGIN CATCH

 EXEC sp_xml_removedocument @XML;

 if (Error_Message()  is NULL or Error_Message() = '')
	 set @Message = 'Ошибка БД при добавлении новых отчетов';
 else
	 set @Message = Error_Message();

ROLLBACK TRANSACTION
RAISERROR(@Message, 12, 1);
Return -1;

END CATCH


END
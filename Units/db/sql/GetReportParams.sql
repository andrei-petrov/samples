CREATE PROCEDURE [dbo].[GetReportParams]
(@ReportName nvarchar(450),   @ParamList  nvarchar(500) output, @DefaultVersion  nvarchar(500) output, 
 @IsTitleList bit output, @ReportFriendlyName  nvarchar(450) output, @ReportSubFolder  nvarchar(500) output)

AS
BEGIN

SET NOCOUNT ON;
	
declare @Message     nvarchar(1000);

BEGIN TRY
    select  @ParamList =  Report.ParameterList, @ReportFriendlyName =  ObjectRelations.Name
    from Report, ObjectRelations
    where  Report.ReportType = @ReportName and ObjectRelations.ObjectGUID = Report.ObjectGUID;

    select  @DefaultVersion =  DefaultVersion,  @IsTitleList =  IsReportTitle from ObjectRoot;
    
    select  @ReportSubFolder =  Name  from  ObjectRelations
    where  ObjectGUID = (select ObjectRelations.ParentGUID from ObjectRelations, Report
                         where  Report.ReportType = @ReportName 
                         and ObjectRelations.ObjectGUID = Report.ObjectGUID);
           
END TRY
BEGIN CATCH

    if (Error_Message()  is NULL or Error_Message() = '')
	   set @Message = 'БД: ошибка запроса параметров отчета';
	else
	   set @Message = Error_Message();
   
    RAISERROR(@Message, 12, 1);
    Return -1;

END CATCH

END
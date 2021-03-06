CREATE PROCEDURE [dbo].[GetReportParamsValues] 
(@ObjectGUID nvarchar(50), @ReportName nvarchar(450))

AS
BEGIN

SET NOCOUNT ON;
	
declare @Message     nvarchar(1000);
declare @ParamList     nvarchar(200);
declare @ForExecute  nvarchar(4000);

BEGIN TRY

      select @ParamList  = ParameterList from Report
      where ReportType = @ReportName;

      set @ForExecute =  'select  ' + '' + @ParamList  + ''
                                   +  '  from  Hardware  where ObjectGUID = '    + '''' + @ObjectGUID + '''';

      execute (@ForExecute);
     
           
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

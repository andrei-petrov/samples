CREATE PROCEDURE [dbo].[GetReports] 

AS
BEGIN

	SET NOCOUNT ON;
	
declare @Message     nvarchar(500);

SET DATEFORMAT ymd;

BEGIN TRY

SELECT ReportType
  FROM Report


END TRY
BEGIN CATCH

    if (Error_Message()  is NULL or Error_Message() = '')
	   set @Message = 'БД: ошибка запроса списка отчетов';
	else
	   set @Message = Error_Message();
   
    RAISERROR(@Message, 12, 1);
    Return -1;

END CATCH

END


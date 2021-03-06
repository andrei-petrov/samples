CREATE PROCEDURE [dbo].[GetInspectionsFromOldDB] 
(@OldDB nvarchar(300), @ObjectGUIDList nvarchar(max) = NULL)

AS
BEGIN

SET NOCOUNT ON;

SET DATEFORMAT ymd;
	
declare @Message  nvarchar(max);

BEGIN TRY


exec dbo.GetDataFromOldDB  @OldDB, 'Inspections', @ObjectGUIDList;

         
END TRY
BEGIN CATCH

    if (Error_Message()  is NULL or Error_Message() = '')
	   set @Message = 'Ошибка переноса инспекций в новую БД';
	else
	   set @Message = 'Ошибка переноса инспекций в новую БД. '  + Error_Message();
    
  
    RAISERROR(@Message, 12, 1);
    Return -1;

END CATCH

END


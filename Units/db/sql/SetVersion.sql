CREATE PROCEDURE [dbo].[SetVersion] 
(@Version int)

AS
BEGIN

SET NOCOUNT ON;
	
declare @Message     nvarchar(1000);

BEGIN TRY

      update ObjectRoot
      Set Version =  @Version;
           
END TRY
BEGIN CATCH

    
    if (Error_Message()  is NULL or Error_Message() = '')
	   set @Message = 'Ошибка БД при изменении ее версии';
	else
	   set @Message = Error_Message();
	   
    RAISERROR(@Message, 12, 1);
    Return -1;

END CATCH

END

CREATE PROCEDURE [dbo].[GetVersion] 
(@Version int output)

AS
BEGIN

SET NOCOUNT ON;
	
declare @Message     nvarchar(1000);

BEGIN TRY

      select @Version = R.Version from ObjectRoot R;
           
END TRY
BEGIN CATCH

    if (Error_Message()  is NULL or Error_Message() = '')
	   set @Message = 'Ошибка запроса версии БД';
	else
	   set @Message = Error_Message();
   
    RAISERROR(@Message, 12, 1);
    Return -1;

END CATCH

END

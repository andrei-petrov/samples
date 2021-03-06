CREATE PROCEDURE [dbo].[RetrievePassword] 
(@TaskGUID nvarchar(50),  @PassPhrase nvarchar(max), @Password nvarchar(150) output)
AS
BEGIN

SET NOCOUNT ON;
	
declare @Message     nvarchar(1000);

BEGIN TRY

-- Decrypt the encrypted record.
   SELECT @Password = CONVERT(nvarchar,
    DecryptByPassphrase(@PassPhrase, Password, 1 
    , CONVERT(varbinary, ObjectGuid)))
   FROM Task
   WHERE ObjectGuid = @TaskGUID;

END TRY
BEGIN CATCH

    if (Error_Message()  is NULL or Error_Message() = '')
	   set @Message = 'Ошибка БД при запросе пароля';
	else
	   set @Message = Error_Message();
	   
    RAISERROR(@Message, 12, 1);
    Return -1;

END CATCH

END


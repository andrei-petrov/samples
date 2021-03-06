CREATE PROCEDURE [dbo].[SavePassword] 
(@TaskGUID nvarchar(50),  @PassPhrase nvarchar(max), @Password nvarchar(150))

AS
BEGIN

SET NOCOUNT ON;
	
declare @Message     nvarchar(1000);

BEGIN TRY
	--DECLARE @encrypted_password  varbinary(256) 
	--SET @encrypted_password = EncryptByPassPhrase(@PassPhrase, @Password);

	UPDATE Task
	SET Password = EncryptByPassPhrase(@PassPhrase, @Password, 1, CONVERT( varbinary, ObjectGuid))
	WHERE ObjectGuid = @TaskGUID;
END TRY
BEGIN CATCH

    if (Error_Message()  is NULL or Error_Message() = '')
	   set @Message = 'Ошибка БД при сохранении пароля';
	else
	   set @Message = Error_Message();
	   
    RAISERROR(@Message, 12, 1);
    Return -1;

END CATCH

END
CREATE PROCEDURE [dbo].[SetUserPicture] 
(@Image varbinary(max),  @IsUserPicture bit) 

AS
BEGIN

SET NOCOUNT ON;
	
declare @Message     nvarchar(1000);

BEGIN TRY

      update ObjectRoot
      Set UserPicture =  @Image,  IsUserPicture = @IsUserPicture;
           
END TRY
BEGIN CATCH

    
    if (Error_Message()  is NULL or Error_Message() = '')
	   set @Message = 'Ошибка БД при изменении логотипа';
	else
	   set @Message = Error_Message();
	   
    RAISERROR(@Message, 12, 1);
    Return -1;

END CATCH

END
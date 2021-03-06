CREATE PROCEDURE [dbo].[SetRootDN] 
(@RootDN nvarchar(300))

AS
BEGIN

SET NOCOUNT ON;
	
declare @Message     nvarchar(1000);

BEGIN TRY

      update ObjectRoot
      Set DN =  @RootDN;
           
END TRY
BEGIN CATCH

    
    if (Error_Message()  is NULL or Error_Message() = '')
	   set @Message = 'Ошибка БД при изменении DN';
	else
	   set @Message = Error_Message();
	   
    RAISERROR(@Message, 12, 1);
    Return -1;

END CATCH

END
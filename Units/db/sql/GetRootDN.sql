CREATE PROCEDURE [dbo].[GetRootDN] 
(@RootDN nvarchar(300) output)

AS
BEGIN

SET NOCOUNT ON;
	
declare @Message  nvarchar(1000);

BEGIN TRY

      select @RootDN = R.DN from ObjectRoot R;
           
END TRY
BEGIN CATCH

    if (Error_Message()  is NULL or Error_Message() = '')
	   set @Message = 'БД: ошибка запроса DN';
	else
	   set @Message = Error_Message();
   
    RAISERROR(@Message, 12, 1);
    Return -1;

END CATCH

END

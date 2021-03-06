CREATE PROCEDURE [dbo].[SPGetLanguage]
(@Localization nvarchar(450),  @Language  nvarchar(450) output)

AS
BEGIN

SET NOCOUNT ON;
	
declare @Message     nvarchar(1000);

BEGIN TRY
    set  @Language = dbo.GetLanguage(@Localization);
           
END TRY
BEGIN CATCH

    
    if (Error_Message()  is NULL or Error_Message() = '')
	   set @Message = 'Ошибка БД при запросе языка';
	else
	   set @Message = Error_Message();
	   
    RAISERROR(@Message, 12, 1);
    Return -1;

END CATCH 

END
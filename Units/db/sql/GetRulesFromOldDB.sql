CREATE PROCEDURE [dbo].[GetRulesFromOldDB] 
(@OldDB nvarchar(300))

AS
BEGIN

SET NOCOUNT ON;
	
declare @Message  nvarchar(max);

BEGIN TRY

exec dbo.GetDataFromOldDB  @OldDB, 'Rules';

--Замена термина Многоязыковая на Многоязычная. См. bug 1807
declare @Localization nvarchar(50)
select @Localization = DefaultLanguage from dbo.ObjectRoot
update dbo.LicenseRule
set Localization = @Localization
where Localization = 'Многоязыковая' or Localization = 'многоязыковая' 
           
END TRY
BEGIN CATCH

    if (Error_Message()  is NULL or Error_Message() = '')
	   set @Message = 'Ошибка переноса правил в новую БД';
	else
	   set @Message = 'Ошибка переноса правил в новую БД. '  + Error_Message();
   
    RAISERROR(@Message, 12, 1);
    Return -1;

END CATCH

END
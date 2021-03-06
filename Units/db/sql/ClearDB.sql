CREATE PROCEDURE [dbo].[ClearDB] 

AS
BEGIN

SET NOCOUNT ON;
	
declare @Message  nvarchar(max);
declare @Check smallint;

BEGIN TRY

BEGIN TRANSACTION

set @Check = 0;

delete from Action;
if (@@Error <> 0)   set @Check = -1;

delete from Soft;
if (@@Error <> 0)   set @Check = -1;
delete from Hardware;
if (@@Error <> 0)   set @Check = -1;
delete from FoundProductFile;
if (@@Error <> 0)   set @Check = -1;

delete from HardwareLicenseRuleLink;
if (@@Error <> 0)   set @Check = -1;
delete from LicenseRuleLink;
if (@@Error <> 0)   set @Check = -1;
delete from HardwareLicenseRule;
if (@@Error <> 0)   set @Check = -1;
delete from LicenseRule;
if (@@Error <> 0)   set @Check = -1;
delete from ProhibitionRule;
if (@@Error <> 0)   set @Check = -1;
delete from PassportLink;
if (@@Error <> 0)   set @Check = -1;
delete from Passport;
if (@@Error <> 0)   set @Check = -1;

delete from ComputerLink;
if (@@Error <> 0)   set @Check = -1;
delete from ObjectRelations where ObjectType = 'Task';
if (@@Error <> 0)   set @Check = -1;
delete from Subdivision;
if (@@Error <> 0)   set @Check = -1;
delete from Computer;
if (@@Error <> 0)   set @Check = -1;


delete from Category;
if (@@Error <> 0)   set @Check = -1;
delete from Publisher;
if (@@Error <> 0)   set @Check = -1;


if (@Check = 0)
        COMMIT TRANSACTION
else 
begin 
    RAISERROR('Транзакция отменена', 12, 1);
end

           
END TRY
BEGIN CATCH

    if (Error_Message()  is NULL or Error_Message() = '')
	   set @Message = 'Ошибка удаления данных из БД';
	else
	   set @Message = 'Ошибка удаления данных из БД. '  + Error_Message();
    

    ROLLBACK TRANSACTION    
    RAISERROR(@Message, 12, 1);
    Return -1;

END CATCH

END
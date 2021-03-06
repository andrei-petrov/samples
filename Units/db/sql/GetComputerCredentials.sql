
CREATE PROCEDURE [dbo].[GetComputerCredentials] 
(@ObjectGUID nvarchar(50), @Name nvarchar(450) output, @Login nvarchar(100) output, @Domain nvarchar(100) output, @Password nvarchar(100) output, @DN nvarchar(100) output)

AS
BEGIN
	
	SET NOCOUNT ON;

declare @ComputerDN nvarchar(100);
declare @N int;
declare @Length int;
declare @Message  nvarchar(1000);

SET DATEFORMAT ymd;

if (@ObjectGUID = '' or @ObjectGUID is NULL)
begin
    Set @Message = 'GUID объекта не определен ';
    RAISERROR(@Message, 12, 1);
    Return -1;
end

BEGIN TRY  

select @Name =  Name, @Login = Login, @Password = Password, @DN = DN, @Domain = Domain 
from Computer  where ObjectGUID = @ObjectGUID;

if (@Login = '' or @Password = '' or @Password is NULL or @Login is NULL) 
begin 

     set @ComputerDN = @DN;

     if (@ComputerDN <> '' and @ComputerDN is not NULL )
     begin

         set @N = CHARINDEX (',', @ComputerDN);
         set @Length = LEN(@ComputerDN);

         set @ComputerDN = RIGHT(@ComputerDN, @Length - @N); 
         set @ComputerDN = RTRIM(@ComputerDN);
         set @ComputerDN = LTRIM(@ComputerDN);
         set @ComputerDN = '%' + @ComputerDN + '%';
     
         select @Login = Login, @Password = Password 
         from  ManagedOU  where DN like  @ComputerDN;

     end 

end

END TRY
BEGIN CATCH

    if (Error_Message()  is NULL or Error_Message() = '')
	   set @Message = 'БД: ошибка запроса данных для компьютера';
	else
	   set @Message = Error_Message();
   
    RAISERROR(@Message, 16, 1);
    Return -1;

END CATCH
	
END
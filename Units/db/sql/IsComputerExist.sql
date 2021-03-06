CREATE PROCEDURE [dbo].[IsComputerExist] 
(@ComputerName nvarchar(450),  @IsExist bit  output)

AS
BEGIN
	
	SET NOCOUNT ON;

declare @Message  nvarchar(1000);
declare @Object_Count int;

BEGIN TRY  
    select @Object_Count = Count(*) from Computer
    where Name = @ComputerName; 
    
    if (@Object_Count > 0)
    begin
         set @IsExist = 1;
    end
    else
    begin
         set @IsExist = 0;
    end


END TRY
BEGIN CATCH

    if (Error_Message()  is NULL or Error_Message() = '')
	   set @Message = 'Ошибка БД при запросе о существовании компьютера';
	else
	   set @Message = Error_Message();
	   
    RAISERROR(@Message, 16, 1);
    Return -1;

END CATCH
	
END
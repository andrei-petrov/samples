CREATE PROCEDURE [dbo].[GetComputersFromOldDB] 
(@OldDB nvarchar(300))

AS
BEGIN

SET NOCOUNT ON;
	
declare @Message  nvarchar(max);


BEGIN TRY


exec dbo.GetDataFromOldDB  @OldDB, 'Computers';

           
END TRY
BEGIN CATCH

    if (Error_Message()  is NULL or Error_Message() = '')
	   set @Message = 'Ошибка переноса компьютеров в новую БД';
	else
	   set @Message = 'Ошибка переноса компьютеров в новую БД. '  + Error_Message();
    
   
    RAISERROR(@Message, 12, 1);
    Return -1;

END CATCH

END
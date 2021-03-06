CREATE PROCEDURE [dbo].[GetLibraryFromOldDB] 
(@OldDB nvarchar(300))

AS
BEGIN

SET NOCOUNT ON;
	
declare @Message  nvarchar(max);

BEGIN TRY


exec dbo.GetDataFromOldDB  @OldDB, 'Library';

update Publisher
set Phone = '' where Phone is NULL;

update Publisher
set Address = '' where Address is NULL; 

           
END TRY
BEGIN CATCH

    if (Error_Message()  is NULL or Error_Message() = '')
	   set @Message = 'Ошибка переноса библиотеки в новую БД';
	else
	   set @Message = 'Ошибка переноса библиотеки в новую БД. '  + Error_Message();
        
    RAISERROR(@Message, 12, 1);
    Return -1;

END CATCH

END
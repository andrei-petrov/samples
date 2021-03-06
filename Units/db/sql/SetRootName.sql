CREATE PROCEDURE [dbo].[SetRootName] 
(@RootName nvarchar(450))

AS
BEGIN

SET NOCOUNT ON;
	
declare @Message     nvarchar(1000);

BEGIN TRY
  
      UPDATE ObjectRelations
      SET Name = @RootName 
      FROM ObjectRelations
      INNER JOIN ObjectRoot
      ON (ObjectRelations.ObjectGUID = ObjectRoot.ObjectGUID);
           
END TRY
BEGIN CATCH

    
    if (Error_Message()  is NULL or Error_Message() = '')
	   set @Message = 'Ошибка БД при изменении имени рутового объекта';
	else
	   set @Message = Error_Message();
	   
    RAISERROR(@Message, 12, 1);
    Return -1;

END CATCH

END
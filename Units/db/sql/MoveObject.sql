CREATE PROCEDURE [dbo].[MoveObject] 
(@ObjectGUID nvarchar(50),  @NewParentGuid nvarchar(50))
-- на данный момент используется только для компьютеров

AS
BEGIN

	SET NOCOUNT ON;
	
declare @N int;
declare @Message     nvarchar(500);
 
BEGIN TRY

      select @N = count(*) from Task
      where ObjectGUID = @NewParentGuid; 
       
                
      if (@N > 0)
      begin 
            update ComputerLink
            set ParentGuid =  @NewParentGuid
            where ObjectGuid =  @ObjectGUID;                     
      end
      else
      begin
            Set @Message = 'Объектом-родителем должен быть Task ';
            RAISERROR(@Message, 12, 1);
      end
END TRY
BEGIN CATCH

    if (Error_Message()  is NULL or Error_Message() = '')
	   set @Message = 'Ошибка БД при перемещении компьютера в другое задание';
	else
	   set @Message = Error_Message();
	   
    RAISERROR(@Message, 12, 1);
    Return -1;

END CATCH

END

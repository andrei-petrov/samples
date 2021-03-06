CREATE PROCEDURE [dbo].[GetTasksFromOldDB] 
(@OldDB nvarchar(300))

AS
BEGIN

SET NOCOUNT ON;
	
declare @Message  nvarchar(max);
declare @Check smallint;
declare @OldObjectRel nvarchar(450);
declare @ParentContainerGUID nvarchar(450);
declare @ForExec  nvarchar(max);

BEGIN TRY

BEGIN TRANSACTION

set @Check = 0;

set @OldObjectRel = '[' + @OldDB + ']' + '.dbo.ObjectRelations';

select @ParentContainerGUID = ObjectGUID 
from ObjectRelations
where ObjectType = 'TaskContainer';

set @ForExec  = 'insert into ObjectRelations' +        
				' select ObjectGUID, ObjectType, Name, ' + '''' + @ParentContainerGUID + '''' + ' , CreationDate '  + 
				' from '  + '' + @OldObjectRel  + '' + 
				' where ObjectType = ''Task'' ';

execute(@ForExec);
if (@@Error <> 0)  set @Check = -1;

exec dbo.GetDataFromOldDB  @OldDB, 'Tasks';
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
	   set @Message = 'Ошибка переноса заданий в новую БД';
	else
	   set @Message = 'Ошибка переноса заданий в новую БД. '  + Error_Message();
    

    ROLLBACK TRANSACTION    
    RAISERROR(@Message, 12, 1);
    Return -1;

END CATCH

END
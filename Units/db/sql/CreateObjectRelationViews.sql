CREATE PROCEDURE [dbo].[CreateObjectRelationViews] 
--  creates views for standart objects that where are  in ObjectRelations
AS
BEGIN

SET NOCOUNT ON;
	
declare @ForExecute     nvarchar(1000);
declare @Message     nvarchar(1000);
declare @Object      nvarchar(50);
declare @I int;

BEGIN TRY

      declare @TabObjects Table(ID int IDENTITY(1,1), Object nvarchar(50) collate Cyrillic_General_CI_AS );

      insert into @TabObjects values('Task');
      insert into @TabObjects values('Report');
      insert into @TabObjects values('ObjectRoot');
      --insert into @TabObjects values('ObjectSchema');
      
      set @I = 1;
      
      while (@I <= 3)
      begin

            select @Object = Object from   @TabObjects where ID = @I;
            set @ForExecute =  'CREATE VIEW View' + @Object +  
                               ' AS select ObjectRelations.ObjectType, ObjectRelations.Name, ObjectRelations.ParentGUID, ObjectRelations.CreationDate, ' 
                                   + '' +  @Object  + '' +  '.* from ObjectRelations, '  + '' +  @Object  + '' + 
                                   ' where '  + '' + @Object  + '' +  '.ObjectGUID = ObjectRelations.ObjectGUID;'

            execute (@ForExecute);

            set @I = @I + 1;
      end 

      
           
END TRY
BEGIN CATCH

	if (Error_Message()  is NULL or Error_Message() = '')
	   set @Message = 'Ошибка БД при создании представлений, связанных с ObjectRelations';
	else
	   set @Message = Error_Message();
   
    RAISERROR(@Message, 12, 1);
    Return -1;

END CATCH

END

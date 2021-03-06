CREATE PROCEDURE [dbo].[SearchObjects] 
(@ObjectGUID nvarchar(50), @SubTree bit, @AttrNames nvarchar(4000),  @Filter nvarchar(4000))

AS
BEGIN

	SET NOCOUNT ON;
	
declare @ObjectType  nvarchar(100);
declare @Message     nvarchar(500);
declare @ForExecute     nvarchar(1500);

SET DATEFORMAT ymd;

BEGIN TRY

if (@AttrNames = '' or @AttrNames is NULL)
begin  
      Set @Message = 'Атрибуты не определены ';    
      RAISERROR(@Message, 12, 1);
end
else 
begin
     if (@SubTree = 0)
     begin 

          set @ForExecute = 'select ' +  @AttrNames + ' from ObjectRelations  where ObjectRelations.ParentGUID = ' + '''' + @ObjectGUID + ''''; 

          if (@Filter <> '')
          begin 
               set @ForExecute = @ForExecute + ' and ' + @Filter;              
          end
    
     end
     else
     begin

           WITH ChildObjects(ObjectGuid, objectType, Name, ParentGUID, CreationDate) AS
           (
           SELECT Rel.ObjectGUID, Rel.objectType,  Rel.Name, Rel.ParentGUID, Rel.CreationDate FROM ObjectRelations  Rel 
              WHERE Rel.ParentGUID = @ObjectGUID 
	       UNION ALL
	       SELECT Rel.ObjectGUID, Rel.objectType,  Rel.Name, Rel.ParentGUID, Rel.CreationDate FROM ObjectRelations  Rel
	          INNER JOIN ChildObjects ON ChildObjects.ObjectGuid = Rel.ParentGUID
           )

           select  *  into #Childs from ChildObjects; 

           set @ForExecute = 'select ' + @AttrNames + ' from #Childs';

           if (@Filter <> '')
           begin  
                 set @ForExecute = @ForExecute + ' where ' + @Filter; 
           end

     end 

     execute (@ForExecute);
end 
  

END TRY
BEGIN CATCH

    if (Error_Message()  is NULL or Error_Message() = '')
	   set @Message = 'Ошибка БД при поиске объектов';
	else
	   set @Message = Error_Message();
	   
    RAISERROR(@Message, 12, 1);
    Return -1;

END CATCH

END


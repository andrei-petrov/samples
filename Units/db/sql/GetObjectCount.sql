---------------------------------------------------------------------------------------------------------
-- Author:      Dmitry Karpov
-- Date:        September 7, 2009
-- Notes:       This procedure returns number of objects with given type.
-- Parameters:  
--              @ParentGuid - guid of parent
--              @ObjectType - type of child objects to be count
--	            @RecursiveSearch - recursive search or not
--              @ObjectCount - output object count
---------------------------------------------------------------------------------------------------------

CREATE PROCEDURE [dbo].[GetObjectCount] 
(@ParentGuid nvarchar(50),  @ObjectType nvarchar(50),  @RecursiveSearch bit, @ObjectCount int  output)

AS
BEGIN
	
	SET NOCOUNT ON;

	declare @Message  nvarchar(1000);
BEGIN TRY  


if ( (@ObjectType like '%Container')  
     or  (@ObjectType in ('Task', 'AttrSchema', 'ObjectRoot', 'ObjectSchema', 'Report', 'SchemaRoot') ))
begin
     
		if (@RecursiveSearch = 1)
		begin
			WITH ChildObjects(ObjectGuid, ObjectType, ParentGUID) AS
			(
				SELECT Rel.ObjectGUID, Rel.objectType, Rel.ParentGUID  FROM ObjectRelations  Rel 
				WHERE  Rel.ParentGuid = @ParentGuid
					UNION ALL
				SELECT Rel.ObjectGUID, Rel.objectType, Rel.ParentGUID  FROM ObjectRelations  Rel
				INNER JOIN ChildObjects ON ChildObjects.ObjectGuid = Rel.ParentGUID
			)
			Select @ObjectCount = count(*) from ChildObjects where ChildObjects.objectType = @ObjectType;    
		end
		else
        begin
		      select @ObjectCount = count(*) from ObjectRelations 
              where ObjectRelations.objectType = @ObjectType and ObjectRelations.ParentGuid = @ParentGuid;	    
        end
end
else
begin
			  
               CREATE TABLE #ObjectCount(  
	                     [ObjCount] [int]  NULL
                     ) ON [PRIMARY];    

               execute ('insert into #ObjectCount select count(*) from ' + @ObjectType);

               select @ObjectCount = ObjCount from #ObjectCount;
               delete from #ObjectCount;
end

END TRY
BEGIN CATCH

    if (Error_Message()  is NULL or Error_Message() = '')
	   set @Message = 'БД: ошибка запроса количества объектов';
	else
	   set @Message = Error_Message();
   
    RAISERROR(@Message, 16, 1);
    Return -1;

END CATCH
	
END
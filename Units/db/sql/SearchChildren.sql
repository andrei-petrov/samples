CREATE PROCEDURE [dbo].[SearchChildren] 
(@ObjectGUID nvarchar(50), @ObjectType nvarchar(50), @AttrNames nvarchar(4000), @Filter nvarchar(4000))

AS
BEGIN

	SET NOCOUNT ON;
	
declare @Message     nvarchar(500);
declare @ForExecute  nvarchar(4000);
declare @ParentType  nvarchar(100);

SET DATEFORMAT ymd;

BEGIN TRY

if (@AttrNames = '' or @AttrNames is NULL)
begin  
            Set @Message = 'Атрибуты не заданы';    
            RAISERROR(@Message, 12, 1);
end

if (@ObjectGUID = '' or @ObjectGUID is NULL)
begin  
            Set @Message = 'GUID объекта не определен';    
            RAISERROR(@Message, 12, 1);
end

if (@ObjectType = '' or @ObjectType is NULL)
begin  
            Set @Message = 'Тип объекта не определен ';    
            RAISERROR(@Message, 12, 1);
end

-- parent type for links
if exists (select top(1) * from Task where ObjectGUID = @ObjectGUID)
set @ParentType = 'Task';
if exists (select top(1) * from Computer where ObjectGUID = @ObjectGUID)
set @ParentType = 'Computer';
if exists (select top(1) * from Passport where ObjectGUID = @ObjectGUID)
set @ParentType = 'Passport';



if ( @ObjectGUID = '00000000-0000-0000-0000-000000000000') 
begin

       if  (  ( @ObjectType like '%Container%' ) or  ( @ObjectType = 'SchemaRoot' )     )  
       begin
              set @ForExecute =   'select ' + '' + @AttrNames  + ''
				+ ' from  ObjectRelations where  ObjectType = ' + '''' + @ObjectType + '''';

              if (@Filter <> '')   
              set @ForExecute =   @ForExecute + ' and ' + @Filter;
       end 
       else 
       begin       /*  View name consists of 'View' and object name */
              set @ForExecute =     'select '    + '' + @AttrNames  + '' 
						    +  ' from  View' + '' + @ObjectType + ''; 

              if (@Filter <> '')   
              set @ForExecute =   @ForExecute + ' where ' + @Filter;   
       end 
end
else
begin   --  first 4 cases is moved from server, for link relations, are formed special way

       if  (       (( @ParentType = 'Task' )     and  ( @ObjectType = 'Computer' ))
              or   (( @ParentType = 'Computer' ) and  ( @ObjectType = 'Passport' ))
              or   (( @ParentType = 'Passport' ) and  ( @ObjectType = 'LicenseRule' ))
              or   (( @ParentType = 'Passport' ) and  ( @ObjectType = 'HardwareLicenseRule' )))
       begin
			   set @ForExecute =     'select '    + '' + @AttrNames  + '' 
						           +  ' from  View' + '' + @ParentType + '' + '' + @ObjectType + ''
                                   +  ' where  ParentGUID = '   + '''' + @ObjectGUID + ''''; 
       end     
       else if  (  ( @ObjectType like '%Container%' ) or  ( @ObjectType = 'SchemaRoot' )  )  
       begin
              set @ForExecute =   'select ' + '' + @AttrNames  + ''
				+ ' from  ObjectRelations where  ObjectType = ' + '''' + @ObjectType + '''' 
                                      + ' and  ParentGUID = '   + '''' + @ObjectGUID + '''';
       end
       else 
       begin       /*  View name consists of 'View' and object name */
              set @ForExecute =     'select '    + '' + @AttrNames  + '' 
						    +  ' from  View' + '' + @ObjectType + ''
                            +  ' where  ParentGUID = '   + '''' + @ObjectGUID + ''''; 
       end   

       if (@Filter <> '')   
       set @ForExecute =   @ForExecute + ' and ' + @Filter;   

end


execute (@ForExecute);
        

END TRY
BEGIN CATCH

    if (Error_Message()  is NULL or Error_Message() = '')
	   set @Message = 'Ошибка БД при поиске объектов со свойствами';
	else
	   set @Message = Error_Message();
	   
    RAISERROR(@Message, 12, 1);
    Return -1;

END CATCH

END
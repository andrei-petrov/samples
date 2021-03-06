CREATE PROCEDURE [dbo].[GetProperties] 
(@AttrNames nvarchar(3000), @ObjectGUID nvarchar(50), @ObjectType  nvarchar(350))

AS
BEGIN

SET NOCOUNT ON;
	
declare @Message     nvarchar(1000);
declare @ForExecute  nvarchar(4000);

SET DATEFORMAT ymd;

BEGIN TRY

      if (@ObjectType is  NULL or @ObjectType = '')
      begin
           Set @Message = 'Не задан тип объекта';
		   RAISERROR(@Message, 12, 1);
		   Return -1;
	  end	   
      else
      begin
         
           if (@AttrNames = '' or @AttrNames is NULL)
           begin 
                Set @Message = 'Атрибуты не определены';    
                RAISERROR(@Message, 12, 1);
           end
           else
           begin
                if ( (@ObjectType like '%Container') or (@ObjectType = 'SchemaRoot'))
                begin
                     set @ForExecute =  'select ' + '' + @AttrNames  + ''
                                   +  ' from ObjectRelations  where ObjectGUID =  ' 
                                   + '''' + @ObjectGUID + '''';
                end                              
                else
                begin         /*  View name consists of 'View' and object name */
                     set @ForExecute =  'select ' + '' + @AttrNames  + ''
                                   +  ' from View' + '' +  @ObjectType  + ''
                                   +  ' V where V.ObjectGUID = '  + '''' + @ObjectGUID + '''';
                end                    

                execute (@ForExecute);
           end  
                        
      end 
   
           
END TRY
BEGIN CATCH

    if (Error_Message()  is NULL or Error_Message() = '')
	   set @Message = 'Ошибка БД при запросе свойств объекта';
	else
	   set @Message = Error_Message();
   
    RAISERROR(@Message, 12, 1);
    Return -1;

END CATCH

END
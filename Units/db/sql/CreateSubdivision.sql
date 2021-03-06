CREATE PROCEDURE [dbo].[CreateSubdivision] 
(@XMLObject nvarchar(max))

AS
BEGIN
	
	SET NOCOUNT ON;

declare @SubdivisionGUID  nvarchar(50);
declare @SubdivisionParentGUID  nvarchar(50);
declare @ObjectName nvarchar(450);
declare @XMLDoc int;
declare @Message  nvarchar(1000);
declare @Check int;
declare @Description nvarchar(2000);
declare @ObjectCount int;
declare @ObjectType nvarchar(300);


SET DATEFORMAT ymd;

BEGIN TRY

    set @XMLObject = RTRIM(@XMLObject);

    EXEC sp_xml_preparedocument @XMLDoc OUTPUT, @XMLObject;

END TRY
BEGIN CATCH

    if (Error_Message()  is NULL or Error_Message() = '')
	   set @Message = 'Ошибка при чтении XML-документа';
	else
	   set @Message = Error_Message();
	   
    RAISERROR(@Message, 12, 1);
    Return -1;

END CATCH

BEGIN TRY

BEGIN TRANSACTION

    select * into #ObjectInfoTbl
    from openxml(@XMLDoc, '/Object', 1)
    with ( ObjectGUID  nvarchar(50),
		   ParentGUID nvarchar(50),
		   Name nvarchar(450),
		   Description nvarchar(2000) );

    select @SubdivisionGUID = ObjectGUID, @SubdivisionParentGUID = ParentGUID,         
           @ObjectName = Name, @Description = Description
    from #ObjectInfoTbl;
 

    if (@SubdivisionGUID = '' or @SubdivisionGUID is NULL)
    begin
        Set @Message = 'GUID синонима не определен ';
        RAISERROR(@Message, 12, 1);
        Return -1;
    end

    if (@SubdivisionParentGUID = '' or @SubdivisionParentGUID is NULL)
    begin
        Set @Message = 'GUID объекта-родителя не определен ';
        RAISERROR(@Message, 12, 1);
        Return -1;
    end
    
    if (@ObjectName = '' or @ObjectName is NULL)
	begin
		Set @Message = 'Имя подразделения не определено ';
		RAISERROR(@Message, 12, 1);
		Return -1;
	end
	
	select @ObjectType = ObjectType from ObjectRelations
    where ObjectGUID = @SubdivisionParentGUID;
    
    if(@ObjectType <> 'SubdivisionContainer')
    begin
        Set @Message = 'Тип объекта-родителя должен быть SubdivisionContainer';
		RAISERROR(@Message, 12, 1);
		Return -1;
    end
    
    select @ObjectCount = Count(*) from Subdivision
    where Name = @ObjectName;
    
    if(@ObjectCount > 0)
    begin
        Set @Message = 'Подразделение с таким именем уже существует в БД';
		RAISERROR(@Message, 12, 1);
		Return -1;
    end

    select @ObjectCount = Count(*) from Subdivision
    where ObjectGUID = @SubdivisionGUID;

    if (@ObjectCount > 0)
    begin
    RAISERROR('Объект с таким GUID уже существует в БД', 16, 1);
    Return -1;
    end
    else
    begin

            set @Check = 0; 

			insert into dbo.Subdivision values(@SubdivisionGUID, @SubdivisionParentGUID, @ObjectName, @Description);  
			 if (@@Error <> 0)   set @Check = -1; 

			if ( @Check = 0  )
				COMMIT TRANSACTION
			else 
			begin 
				RAISERROR('Транзакция отменена', 16, 1);
			end

    end
    
    EXEC sp_xml_removedocument @XMLDoc;

END TRY
BEGIN CATCH

    EXEC sp_xml_removedocument @XMLDoc;
    
    if (Error_Message()  is NULL or Error_Message() = '')
	   set @Message = 'Ошибка БД при создании подразделения';
	else
	   set @Message = Error_Message();
   
    ROLLBACK TRANSACTION
    RAISERROR(@Message, 16, 1);
    Return -1;

END CATCH
	
END

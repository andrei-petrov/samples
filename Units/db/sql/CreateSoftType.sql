CREATE PROCEDURE [dbo].[CreateSoftType] 
(@XMLObject nvarchar(max))

AS
BEGIN
	
	SET NOCOUNT ON;

declare @SoftTypeGUID  nvarchar(50);
declare @PublisherGUID  nvarchar(50);
declare @CategoryGUID  nvarchar(50);
declare @ObjectCount int;
declare @SoftTypeParentGUID  nvarchar(50);
declare @XMLDoc int;
declare @Message  nvarchar(1000);
declare @Check int;
declare @AddingDate datetime;
declare @ProductName nvarchar(450);
declare @VersionMajor  nvarchar(50);
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
    with (ObjectGUID  nvarchar(50),
          PublisherGUID  nvarchar(50),
          CategoryGUID  nvarchar(50),
          ParentGUID nvarchar(50),
          Name nvarchar(450),
          ProductName nvarchar(450),
          VersionMajor  nvarchar(50), 
          IsOperatingSystem smallint);

    select @SoftTypeGUID = ObjectGUID, @PublisherGUID = PublisherGUID, 
		   @CategoryGUID = CategoryGUID, @SoftTypeParentGUID = ParentGUID,         
		   @ProductName = ProductName, @VersionMajor = VersionMajor
    from   #ObjectInfoTbl;
  

    if (@SoftTypeGUID = '' or @SoftTypeGUID is NULL)
    begin
        Set @Message = 'GUID объекта не определен ';
        RAISERROR(@Message, 12, 1);
        Return -1;
    end
    
    if (@PublisherGUID = '' or @PublisherGUID is NULL)
    begin
        Set @Message = 'GUID производителя не определен ';
        RAISERROR(@Message, 12, 1);
        Return -1;
    end

    if (@SoftTypeParentGUID = '' or @SoftTypeParentGUID is NULL)
    begin
        Set @Message = 'GUID объекта-родителя не определен ';
        RAISERROR(@Message, 12, 1);
        Return -1;
    end
    
    if (@ProductName = '' or @ProductName is NULL)
	begin
		Set @Message = 'Имя продукта не определено ';
		RAISERROR(@Message, 12, 1);
		Return -1;
	end
	
	select @ObjectType = ObjectType from ObjectRelations
    where ObjectGUID = @SoftTypeParentGUID;
    
    if(@ObjectType <> 'SoftTypeContainer')
    begin
        Set @Message = 'Тип объекта-родителя должен быть SoftTypeContainer';
		RAISERROR(@Message, 12, 1);
		Return -1;
    end
    
    select @ObjectCount = Count(*) from SoftType
    where ProductName = @ProductName and PublisherGUID = @PublisherGUID and VersionMajor = @VersionMajor;
    
    if(@ObjectCount > 0)
    begin
        Set @Message = 'Продукт с таким названием, версией и производителем уже существует в БД';
		RAISERROR(@Message, 12, 1);
		Return -1;
    end

    select @ObjectCount = Count(*) from ObjectRelations
    where ObjectRelations.ObjectGUID = @SoftTypeGUID;

    if (@ObjectCount > 0)
    begin
    RAISERROR('Объект с таким GUID уже существует в БД', 16, 1);
    Return -1;
    end
    else
    begin

            set @Check = 0; 

			insert into dbo.SoftType values(@SoftTypeGUID, @SoftTypeParentGUID,  @PublisherGUID, @ProductName, @VersionMajor, @CategoryGUID, 0);  
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
	   set @Message = 'Ошибка БД при создании типа ПО';
	else
	   set @Message = Error_Message();
   
    ROLLBACK TRANSACTION
    RAISERROR(@Message, 16, 1);
    Return -1;

END CATCH
	
END
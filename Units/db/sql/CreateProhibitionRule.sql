CREATE PROCEDURE [dbo].[CreateProhibitionRule] 
(@XMLObject nvarchar(max))

AS
BEGIN
	
	SET NOCOUNT ON;

declare @Object_GUID  nvarchar(50);
declare @Object_Count int;
declare @Parent_GUID  nvarchar(50);
declare @Object_Type  nvarchar(100);
declare @Object_Name nvarchar(450);
declare @ProductName nvarchar(450);
declare @Publisher nvarchar(450);
declare @ProductKey nvarchar(300);
declare @Localization nvarchar(50);
declare @ProductVersion nvarchar(450);
declare @XMLDoc int;
declare @Message  nvarchar(1000);
declare @Message1  nvarchar(500);
declare @Message2  nvarchar(500);
declare @Check int;
declare @Description nvarchar(4000);
declare @SoftType tinyint;
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
          ObjectType  nvarchar(100),
          ParentGUID nvarchar(50),
          Name nvarchar(450),
          ProductName nvarchar(450),
          Publisher nvarchar(450),
          Localization nvarchar(50),
          Description nvarchar(4000),
          ProductVersion nvarchar(450),
          ProductKey nvarchar(300),
          SoftType tinyint);

    select  @Object_GUID = ObjectGUID, @Object_Type = ObjectType,
			@Parent_GUID = ParentGUID,  @Object_Name = Name,
			@ProductName = ProductName, @Publisher = Publisher,
			@Localization = Localization, @ProductVersion = ProductVersion,
			@Description = Description, @ProductKey = ProductKey,
			@SoftType = SoftType
    from    #ObjectInfoTbl;
           


    if (@Object_GUID = '' or @Object_GUID is NULL)
    begin
        Set @Message = 'GUID объекта не определен ';
        RAISERROR(@Message, 12, 1);
        Return -1;
    end

    if (@Object_Type = '' or @Object_Type is NULL)
    begin
        Set @Message = 'Тип объекта не определен ';
        RAISERROR(@Message, 12, 1);
        Return -1;
    end

    if (@ProductName = '' or @ProductName = '*' or @ProductName is NULL)
    begin
        Set @ProductName = '%%';
    end
    
    if (@Publisher = '' or @Publisher = '*' or @Publisher is NULL)
    begin
        Set @Publisher = '%%';
    end
    
    if (@Localization = '' or @Localization = '*' or @Localization is NULL)
    begin
        Set @Localization = '%%';
    end
    
    if (@ProductVersion = '' or @ProductVersion = '*' or @ProductVersion is NULL)
    begin
        Set @ProductVersion = '%%';
    end
    
    if (@ProductKey = '' or @ProductKey = '*' or @ProductKey is NULL)
    begin
        Set @ProductKey = '%%';
    end
    
        set @ProductName = Replace(@ProductName, '*', '%'); 
        set @Publisher = Replace(@Publisher, '*', '%');
        set @Localization = Replace(@Localization, '*', '%'); 
        set @ProductVersion = Replace(@ProductVersion, '*', '%'); 
        set @ProductKey = Replace(@ProductKey, '*', '%');

    if (@Parent_GUID = '' or @Parent_GUID is NULL)
    begin
        Set @Message = 'GUID объекта-родителя не определен ';
        RAISERROR(@Message, 12, 1);
        Return -1;
    end

    if (@Object_Name = '' or @Object_Name is NULL)
    begin
        Set @Message = 'Название правила не определено ';
        RAISERROR(@Message, 12, 1);
        Return -1;
    end
    
    select @ObjectType = ObjectType from ObjectRelations
    where ObjectGUID = @Parent_GUID;
    
    if(@ObjectType <> 'ProhibitionRuleContainer')
    begin
        Set @Message = 'Тип объекта-родителя должен быть ProhibitionRuleContainer';
		RAISERROR(@Message, 12, 1);
		Return -1;
    end
    
    select @Object_Count = Count(*) from ProhibitionRule
    where Name = @Object_Name;
    
    if(@Object_Count > 0)
    begin
        Set @Message = 'Правило черного списка с таким именем уже существует в БД';
		RAISERROR(@Message, 12, 1);
		Return -1;
    end
    

    select @Object_Count = Count(*) from ProhibitionRule
    where ObjectGUID = @Object_GUID;

    if (@Object_Count > 0)
    begin
    RAISERROR('Объект с таким GUID уже существует в БД', 16, 1);
    Return -1;
    end
    else
    begin

        insert into dbo.ProhibitionRule values(@Object_GUID, @Parent_GUID, @Object_Name, @ProductName, @Publisher, @Localization, @ProductVersion, @ProductKey, @Description, @SoftType);  
        SET @Check = @@ERROR; 

        if (@Check = 0)
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
	   set @Message = 'Ошибка БД при создании правила черного списка';
	else
	   set @Message = Error_Message();
   
    ROLLBACK TRANSACTION
    RAISERROR(@Message, 16, 1);
    Return -1;

END CATCH
	
END
CREATE PROCEDURE [dbo].[CreateSynonym] 
(@XMLObject nvarchar(max))

AS
BEGIN
	
	SET NOCOUNT ON;

declare @SynonymGUID  nvarchar(50);
declare @SynonymParentGUID  nvarchar(50);
declare @AttributeName nvarchar(50);
declare @Template nvarchar(100);
declare @DisplayName nvarchar(100);
declare @XMLDoc int;
declare @Message  nvarchar(1000);
declare @Check int;
declare @Description nvarchar(1000);
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
		   AttributeName nvarchar(50),
		   Template nvarchar(2000),
		   DisplayName nvarchar(2000) );

    select  @SynonymGUID = ObjectGUID, @SynonymParentGUID = ParentGUID,         
			@AttributeName = AttributeName, @Template = Template, @DisplayName = DisplayName
    from    #ObjectInfoTbl;
            

 

    if (@SynonymGUID = '' or @SynonymGUID is NULL)
    begin
        Set @Message = 'GUID синонима не определен ';
        RAISERROR(@Message, 12, 1);
        Return -1;
    end

    if (@SynonymParentGUID = '' or @SynonymParentGUID is NULL)
    begin
        Set @Message = 'GUID объекта-родителя не определен ';
        RAISERROR(@Message, 12, 1);
        Return -1;
    end

    if (@AttributeName = '' or @AttributeName is NULL)
    begin
        Set @Message = 'Название атрибута не определено ';
        RAISERROR(@Message, 12, 1);
        Return -1;
    end
    
    if (@Template = '' or @Template = '*' or @Template is NULL)
    begin
        Set @Template = '%%';
    end
    
    set @Template = Replace(@Template, '*', '%');
    
    select @ObjectType = ObjectType from ObjectRelations
    where ObjectGUID = @SynonymParentGUID;
    
    if(@ObjectType <> 'SynonymContainer')
    begin
        Set @Message = 'Тип объекта-родителя должен быть SynonymContainer';
		RAISERROR(@Message, 12, 1);
		Return -1;
    end 
    
    select @ObjectCount = Count(*) from Synonym
    where Template = @Template and AttributeName = @AttributeName;
    
    if(@ObjectCount > 0)
    begin
        Set @Message = 'Синоним с таким шаблоном уже существует в БД';
		RAISERROR(@Message, 12, 1);
		Return -1;
    end

    select @ObjectCount = Count(*) from Synonym
    where ObjectGUID = @SynonymGUID;

    if (@ObjectCount > 0)
    begin
    RAISERROR('Объект с таким GUID уже существует в БД', 16, 1);
    Return -1;
    end
    else
    begin

            set @Check = 0; 

			insert into dbo.Synonym values(@SynonymGUID, @SynonymParentGUID, @AttributeName, @Template, @DisplayName);  
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
	   set @Message = 'Ошибка БД при создании синонима';
	else
	   set @Message = Error_Message();
   
    ROLLBACK TRANSACTION
    RAISERROR(@Message, 16, 1);
    Return -1;

END CATCH
	
END

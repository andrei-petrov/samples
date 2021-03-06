CREATE PROCEDURE [dbo].[CreateLibProductFile] 
(@XMLObject nvarchar(max))

AS
BEGIN
	
	SET NOCOUNT ON;

declare @ObjectGUID  nvarchar(50);
declare @ParentGUID  nvarchar(50);
declare @OriginalFileName nvarchar(450);
declare @Company nvarchar(450);
declare @ProductName nvarchar(450);
declare @FileVersion nvarchar(450);
declare @ProductVersion nvarchar(450);
declare @Language nvarchar(450);
declare @Size bigint;


declare @ObjectCount int;
declare @XMLDoc int;
declare @Message  nvarchar(1000);
declare @Check int;
declare @ParentTypeCheck int;
declare @AddingDate datetime;
declare @Count int;


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

    select * into #ObjectInfoTbl
    from openxml(@XMLDoc, '/Object', 1)
    with (ObjectGUID  nvarchar(50),
          ParentGUID nvarchar(50),
		  FileVersion nvarchar(450),
		  OriginalFileName nvarchar(450),  
		  Company nvarchar(450),
		  ProductName nvarchar(450), 
		  ProductVersion nvarchar(450),  
		  Language nvarchar(450),
		  Size bigint);

    select  @ObjectGUID = ObjectGUID, @ParentGUID = ParentGUID,
			@OriginalFileName = OriginalFileName, @FileVersion = FileVersion,
			@Company = Company, @ProductName = ProductName,
			@ProductVersion = ProductVersion, @Language = Language, @Size = Size
    from    #ObjectInfoTbl 


    if (@Company = '' or @Company = '*')
    begin
        Set @Company = '%%';
    end
    
    if (@ProductName = '' or @ProductName = '*')
    begin
        Set @ProductName = '%%';
    end
    
    if (@ProductVersion = '' or @ProductVersion = '*')
    begin
        Set @ProductVersion = '%%';
    end
    
    if (@Language = '' or @Language = '*')
    begin
        Set @Language = '%%';
    end
    
    if (@FileVersion = '' or @FileVersion = '*')
    begin
        Set @FileVersion = '%%';
    end
    
    if (@Size is NULL)
    begin
        Set @Size = 0;
    end
    
    set @Company = Replace(@Company, '*', '%');
    set @ProductName = Replace(@ProductName, '*', '%');
    set @FileVersion = Replace(@FileVersion, '*', '%');
    set @ProductVersion = Replace(@ProductVersion, '*', '%');
    set @Language = Replace(@Language, '*', '%');


    if (@ObjectGUID = '' or @ObjectGUID is NULL)
    begin
        Set @Message = 'GUID объекта не определен ';
        RAISERROR(@Message, 12, 1);
        Return -1;
    end

    if (@ParentGUID = '' or @ParentGUID is NULL)
    begin
        Set @Message = 'GUID объекта-родителя не определен ';
        RAISERROR(@Message, 12, 1);
        Return -1;
    end
    
    if (@OriginalFileName = '' or @OriginalFileName is NULL)
	begin
		Set @Message = 'Имя файла не определено ';
		RAISERROR(@Message, 12, 1);
		Return -1;
	end
	
	select @ParentTypeCheck = count(*) from SoftType
	where ObjectGUID = @ParentGUID;
	
	if(@ParentTypeCheck < 1)
	begin
		Set @Message = 'Объектом-родителем для LibProductFile должен быть SoftType';
		RAISERROR(@Message, 12, 1);
		Return -1;
	end

    select @ObjectCount = Count(*) from ObjectRelations
    where ObjectRelations.ObjectGUID = @ObjectGUID;

    if (@ObjectCount > 0)
    begin
    RAISERROR('Объект с таким GUID уже существует в БД', 16, 1);
    Return -1;
    end
    else
    begin
    
            -- Is there this file already in defined SoftType?
			select @Count = Count(*) from LibProductFile
			where ParentGUID = @ParentGUID and OriginalFileName = @OriginalFileName
			      and  Company = @Company and ProductName = @ProductName 
			      and  FileVersion = @FileVersion and ProductVersion = @ProductVersion
			      and  Language = @Language and Size = @Size;
			
			if (@Count = 0)
			/*begin
					Set @Message = 'Файл  ' + @OriginalFileName + ' уже добавлен к указанному продукту';
					RAISERROR(@Message, 16, 1);
					Return -1;
			end
			else*/
			begin

					set @AddingDate = getutcdate();

					insert into dbo.LibProductFile 
					values (@ObjectGUID,  @ParentGUID,  @FileVersion, @OriginalFileName,  
		                   @Company, @ProductName, @ProductVersion, @Language, @Size, 0);
			
			end

    end
    
    EXEC sp_xml_removedocument @XMLDoc;

END TRY
BEGIN CATCH

    EXEC sp_xml_removedocument @XMLDoc;
    
	if (Error_Message()  is NULL or Error_Message() = '')
	   set @Message = 'Ошибка БД при создании библиотечного файла';
	else
	   set @Message = Error_Message();
   
    RAISERROR(@Message, 16, 1);
    Return -1;

END CATCH
	
END
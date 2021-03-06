CREATE PROCEDURE [dbo].[SetCategoryProperties] 
(@ObjectGUID nvarchar(50), @XMLObject nvarchar(max))

AS
BEGIN
	 
	SET NOCOUNT ON;

declare @Object_GUID  nvarchar(50);
declare @XMLDoc int;
declare @Message  nvarchar(1000);
declare @Check int;
declare @AttrNames nvarchar(4000);
declare @ForExecute  nvarchar(4000);
declare @Length int;
declare @OldName nvarchar(450);
declare @ObjectName nvarchar(450);
declare @Description nvarchar(4000);

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

    set @Check = 0;


        select * into #Category
        from openxml(@XMLDoc, '/Object', 1)
        with (Name nvarchar(450),
              Description nvarchar(1000));

        select @ObjectName = Name, @Description = Description
        from #Category;
        
        -- проверка, что не меняется категория безопасности
        select @OldName = C.Name from SecurityTypeLU S, Category C
        where S.SecurityType = C.Name and C.ObjectGUID = @ObjectGUID;
        
        
        
        set @Description = Replace(@Description, '''', '''''');

        set @AttrNames = '';
        
        if (@Description is not NULL)
        begin
        set @AttrNames = @AttrNames +  ', Description = '  + '''' + @Description + '''';
        end 
        
        if (@ObjectName is not NULL)
        begin
        set @AttrNames = @AttrNames +  ', Name = '  + '''' + @ObjectName + '''';
        end 
        
        if (substring(@AttrNames, 1, 1)= ',')
        begin
        set @Length = LEN(@AttrNames) - 1;
        set @AttrNames = Right(@AttrNames, @Length);
        end
        
        if (@AttrNames <> '')
        begin

            set @ForExecute = 'update Category  set ' + @AttrNames + ' where ObjectGUID = ' + '''' + @ObjectGUID + '''';
            execute (@ForExecute);
            if (@@Error <> 0)  set @Check = -1;
        
        end
        
        if (@OldName is not NULL)
        begin

            update SecurityTypeLU
            set SecurityType = @ObjectName where SecurityType = @OldName;
            if (@@Error <> 0)  set @Check = -1;
        
        end
        
        

        
    if (@Check = 0 )
            COMMIT TRANSACTION
    else 
    begin 
            RAISERROR('Транзакция отменена', 16, 1);
    end 
    
    EXEC sp_xml_removedocument @XMLDoc;                      

END TRY
BEGIN CATCH

    EXEC sp_xml_removedocument @XMLDoc;
    
    if (Error_Message()  is NULL or Error_Message() = '')
	   set @Message = 'Ошибка БД при изменении свойств категории';
	else
	   set @Message = Error_Message();
	   
    ROLLBACK TRANSACTION
    RAISERROR(@Message, 12, 1);
    Return -1;

END CATCH 

END

CREATE PROCEDURE [dbo].[SetLibProductFileProperties] 
(@ObjectGUID nvarchar(50), @XMLObject nvarchar(max))

AS
BEGIN
	 
	SET NOCOUNT ON;

declare @Object_GUID  nvarchar(50);
declare @XMLDoc int;
declare @Message  nvarchar(1000);
declare @Check int;
declare @AttrNames nvarchar(max);
declare @ForExecute  nvarchar(4000);
declare @Length int;
declare @String nvarchar(300);

declare @ProductName nvarchar(450);
declare @ProductVersion  nvarchar(450);
declare @FileVersion nvarchar(450);
declare @OriginalFileName nvarchar(450);
declare @FileName nvarchar(4000);
declare @Company nvarchar (450);
declare @Language nvarchar(450);
declare @Size bigint;


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

        select * into #LibProductFileTbl
        from openxml(@XMLDoc, '/Object', 1)
        with (ProductName nvarchar(450),
			  ProductVersion  nvarchar(450),
			  FileVersion nvarchar(450),
			  OriginalFileName nvarchar(450),
			  FileName nvarchar(4000),
			  Company nvarchar (450),
			  Language nvarchar(450),
			  Size bigint);
       
		select @ProductName = ProductName, @ProductVersion = ProductVersion,
			   @FileVersion = FileVersion, @OriginalFileName = OriginalFileName,
			   @FileName = FileName, @Company = Company, @Language = Language, @Size = Size
	    from   #LibProductFileTbl;
		
        
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
		
		set @ProductName = Replace(@ProductName, '''', '''''');
        set @OriginalFileName = Replace(@OriginalFileName, '''', '''''');
        set @FileName = Replace(@FileName, '''', '''''');
        set @Company = Replace(@Company, '''', ''''''); 
        

        set @AttrNames = '';
        
        if (@ProductName is not NULL)
            set @AttrNames = @AttrNames +  ',  ProductName = '  + '''' + @ProductName + ''''; 
        
        if (@ProductVersion  is not NULL)
            set @AttrNames = @AttrNames +  ',  ProductVersion  = '  + '''' + @ProductVersion  + '''';

		if (@FileVersion is not NULL)
			set @AttrNames = @AttrNames +  ',  FileVersion = '  + '''' + @FileVersion + '''';

		if (@OriginalFileName is not NULL)
			set @AttrNames = @AttrNames +  ',  OriginalFileName = '  + '''' + @OriginalFileName + '''';

		if (@FileName is not NULL)
			set @AttrNames = @AttrNames +  ',  FileName = '  + '''' + @FileName + '''';
        
        if (@Company is not NULL)
			set @AttrNames = @AttrNames +  ',  Company = '  + '''' + @Company + '''';
        
        if (@Language  is not NULL)
           set @AttrNames = @AttrNames +  ',  Language = '  + '''' + @Language + '''';
        
        if (@Size  is not NULL)   
        begin
           set @String = @Size;
           set @AttrNames = @AttrNames +  ',  Size = '  + @String; 
        end     
        
        if (substring(@AttrNames, 1, 1)= ',')
        begin
        set @Length = LEN(@AttrNames) - 1;
        set @AttrNames = Right(@AttrNames, @Length);
        end
        
        if (@AttrNames <> '')
        begin

            set @ForExecute = 'update LibProductFile  set ' + @AttrNames + ' where ObjectGUID = ' + '''' + @ObjectGUID + '''';
            execute (@ForExecute);
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
	   set @Message = 'Ошибка БД при изменении свойств библиотечного файла';
	else
	   set @Message = Error_Message();
	   
    ROLLBACK TRANSACTION
    RAISERROR(@Message, 12, 1);
    Return -1;

END CATCH 

END

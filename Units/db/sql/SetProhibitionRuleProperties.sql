CREATE PROCEDURE [dbo].[SetProhibitionRuleProperties] 
(@ObjectGUID nvarchar(50), @XMLObject nvarchar(max))

AS
BEGIN
	 
	SET NOCOUNT ON;

declare @Object_GUID  nvarchar(50);
declare @XMLDoc int;
declare @Message  nvarchar(1000);
declare @Check int;
declare @AttrNames nvarchar(3000);
declare @ForExecute  nvarchar(4000);
declare @Length int;
declare @String nvarchar(2000);

declare @ObjectName nvarchar(450);
declare @Description nvarchar(4000);
declare @Publisher nvarchar(450);
declare @Localization nvarchar(300);
declare @ProductVersion nvarchar(450);
declare @ProductKey nvarchar(300);
declare @SoftType tinyint;
declare @ProductName nvarchar(450);

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

        select * into #RuleTbl
        from openxml(@XMLDoc, '/Object', 1)
        with (Name nvarchar(450),
              ProductName nvarchar(450),
              Description nvarchar(4000),
              Publisher nvarchar(450),
              Localization nvarchar(50),
              ProductVersion nvarchar(450),
              ProductKey nvarchar(300),
              SoftType tinyint);

        select @ObjectName = Name, @Description = Description,
			   @Publisher = Publisher, @Localization = Localization,
			   @ProductVersion = ProductVersion, @ProductName = ProductName,
			   @ProductKey = ProductKey, @SoftType = SoftType
        from   #RuleTbl;
        
        if (@ProductName = '' or @ProductName = '*')
            Set @ProductName = '%%';
    
        if (@Publisher = '' or @Publisher = '*')
            Set @Publisher = '%%';
    
        if (@Localization = '' or @Localization = '*')
            Set @Localization = '%%';
    
        if (@ProductVersion = '' or @ProductVersion = '*')
            Set @ProductVersion = '%%';
        
        if (@ProductKey = '' or @ProductKey = '*')
            Set @ProductKey = '%%';
        
        set @ProductName = Replace(@ProductName, '*', '%'); 
        set @Publisher = Replace(@Publisher, '*', '%');
        set @Localization = Replace(@Localization, '*', '%'); 
        set @ProductVersion = Replace(@ProductVersion, '*', '%'); 
        set @ProductKey = Replace(@ProductKey, '*', '%');
        
        set @Description = Replace(@Description, '''', '''''');
        set @ProductName = Replace(@ProductName, '''', ''''''); 
        set @Publisher = Replace(@Publisher, '''', '''''');
        set @Localization = Replace(@Localization, '''', ''''''); 
        set @ProductVersion = Replace(@ProductVersion, '''', ''''''); 
        set @ProductKey = Replace(@ProductKey, '''', ''''''); 

        set @AttrNames = '';
       
        if (@Description is not NULL)
		    set @AttrNames = @AttrNames +  ',  Description = '  + '''' + @Description + '''';
 
        if (@ObjectName is not NULL)
		    set @AttrNames = @AttrNames +  ',  Name = '  + '''' + @ObjectName + '''';
 
        if (@ProductName is not NULL)
			set @AttrNames = @AttrNames +  ',  ProductName = '  + '''' + @ProductName + '''';

        if (@ProductVersion is not NULL)
			set @AttrNames = @AttrNames +  ',  ProductVersion = '  + '''' + @ProductVersion + '''';

        if (@Publisher is not NULL)
		    set @AttrNames = @AttrNames +  ',  Publisher = '  + '''' + @Publisher + '''';

        if (@Localization is not NULL)
		    set @AttrNames = @AttrNames +  ',  Localization = '  + '''' + @Localization + '''';
        
        if (@ProductKey is not NULL)
            set @AttrNames = @AttrNames +  ',  ProductKey = '  + '''' + @ProductKey + '''';

        if (@SoftType  is not NULL)
        begin
           set @String = @SoftType;
           set @AttrNames = @AttrNames +  ',  SoftType = '  + @String;
        end 

        if (substring(@AttrNames, 1, 1)= ',')
        begin
			set @Length = LEN(@AttrNames) - 1;
			set @AttrNames = Right(@AttrNames, @Length);
        end
        
        if (@AttrNames <> '')
        begin

            set @ForExecute = 'update ProhibitionRule  set ' + @AttrNames + ' where ObjectGUID = ' + '''' + @ObjectGUID + '''';
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
	   set @Message = 'Ошибка БД при изменении свойств правила ЧС';
	else
	   set @Message = Error_Message();
	   
    ROLLBACK TRANSACTION
    RAISERROR(@Message, 12, 1);
    Return -1;

END CATCH 

END

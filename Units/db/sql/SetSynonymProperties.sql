CREATE PROCEDURE [dbo].[SetSynonymProperties] 
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

declare @Template nvarchar(2000);
declare @DisplayName nvarchar(2000);

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


        select * into #Synonym
        from openxml(@XMLDoc, '/Object', 1)
        with (Template nvarchar(2000),
              DisplayName nvarchar(2000));


        select @Template = Template, @DisplayName = DisplayName
        from #Synonym; 
        
        set @Template = Replace(@Template, '''', '''''');
        set @DisplayName = Replace(@DisplayName, '''', '''''');
        
        if (@Template = '*')
        begin
            Set @Template = '%%';
        end
        
        set @Template = Replace(@Template, '*', '%'); 

        set @AttrNames = '';
        
        if (@Template is not NULL)
			set @AttrNames = @AttrNames +  ', Template = '  + '''' + @Template + '''';
        
        if (@DisplayName is not NULL)
			set @AttrNames = @AttrNames +  ', DisplayName = '  + '''' + @DisplayName + '''';

        
        if (substring(@AttrNames, 1, 1)= ',')
        begin
        set @Length = LEN(@AttrNames) - 1;
        set @AttrNames = Right(@AttrNames, @Length);
        end
        
        if (@AttrNames <> '')
        begin

            set @ForExecute = 'update Synonym  set ' + @AttrNames + ' where ObjectGUID = ' + '''' + @ObjectGUID + '''';
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
	   set @Message = 'Ошибка БД при изменении свойств синонима';
	else
	   set @Message = Error_Message();
	   
    ROLLBACK TRANSACTION
    RAISERROR(@Message, 12, 1);
    Return -1;

END CATCH 

END

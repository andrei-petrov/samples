CREATE PROCEDURE [dbo].[SetComputerProperties] 
(@ObjectGUID nvarchar(50), @XMLObject nvarchar(max))

AS
BEGIN
	 
	SET NOCOUNT ON;

declare @Object_GUID  nvarchar(50);
declare @ObjectName nvarchar(450);
declare @XMLDoc int;
declare @Message  nvarchar(1000);
declare @Check int;
declare @Status int;
declare @Description nvarchar(4000);
declare @SubdivisionGUID nvarchar(50);
declare @AttrNames nvarchar(max);
declare @String nvarchar(2000);
declare @ForExecute nvarchar(max);
declare @Length int;


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

---------------------------------------------------------------------------------------------------------

BEGIN TRY

BEGIN TRANSACTION

    set @Check = 0;

    select * into #ComputerTbl
    from openxml(@XMLDoc, '/Object', 1)
    with (Name nvarchar(450),
          Status smallint,
          Description nvarchar(4000),
          SubdivisionGUID nvarchar(50) );

    select  @ObjectName = Name, @Status = Status,
			@Description = Description,  @SubdivisionGUID = SubdivisionGUID
    from    #ComputerTbl;
    

    set @Description = Replace(@Description, '''', '''''');  

    set @AttrNames = '';
        
    if (@Status is not NULL)
    begin
        set @String = @Status;
        set @AttrNames = @AttrNames +  ',  Status = '  + @String;
    end

    if (@ObjectName is not NULL)
        set @AttrNames = @AttrNames +  ',  Name = '  + '''' + @ObjectName + '''';

    
    if (@Description is not NULL)
        set @AttrNames = @AttrNames +  ',  Description = '  + '''' + @Description + '''';

    
    if (@SubdivisionGUID is not NULL and @SubdivisionGUID <> '')
        set @AttrNames = @AttrNames +  ',  SubdivisionGUID = '  + '''' + @SubdivisionGUID + '''';
    else if(@SubdivisionGUID = '')
        set @AttrNames = @AttrNames +  ',  SubdivisionGUID = NULL';
    
    if (substring(@AttrNames, 1, 1)= ',')
    begin
        set @Length = LEN(@AttrNames) - 1;
        set @AttrNames = Right(@AttrNames, @Length);
    end
        
	if (@AttrNames <> '')
	begin

		set @ForExecute = 'update Computer  set ' + @AttrNames + ' where ObjectGUID = ' + '''' + @ObjectGUID + '''';
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
	   set @Message = 'Ошибка БД при изменении свойств компьютера';
	else
	   set @Message = Error_Message();
	   
    ROLLBACK TRANSACTION
    RAISERROR(@Message, 12, 1);
    Return -1;

END CATCH 

END
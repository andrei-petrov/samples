CREATE PROCEDURE [dbo].[SetPublisherProperties] 
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

declare @ObjectName nvarchar(450);
declare @OldName nvarchar(450);
declare @Website nvarchar(1000);
declare @Phone nvarchar(100);
declare @Address nvarchar(1000);

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

        select * into #Publisher
        from openxml(@XMLDoc, '/Object', 1)
        with (Name nvarchar(450),
              Phone nvarchar(100),
              Website nvarchar(1000),
              Address nvarchar(1000));

        select @ObjectName = Name, @Website = Website,
               @Phone = Phone, @Address = Address
        from #Publisher; 

		-- проверка, что не меняем софт, на который ссылаются инспекции

        select @OldName = Name from Publisher where ObjectGUID = @ObjectGUID;
 
        if (@ObjectName <> @OldName)
        begin
               if exists(select top(1) * from Publisher P, SoftType ST, Soft S, Installation I
                         where P.ObjectGUID = ST.PublisherGUID and ST.ObjectGUID = S.SoftTypeGUID 
                               and S.ObjectGUID = I.SoftGUID and P.ObjectGUID = @ObjectGUID)
               RAISERROR('Данное ПО установлено на проинспектированных компьютерах', 12, 1);
        end 
        
        set @Website = Replace(@Website, '''', ''''''); 
        set @Phone = Replace(@Phone, '''', ''''''); 
        set @Address = Replace(@Address, '''', ''''''); 
        
        set @AttrNames = '';

        if (@ObjectName is not NULL)
        begin
        set @AttrNames = @AttrNames +  ', Name = '  + '''' + @ObjectName + '''';
        end
        
        if (@Website is not NULL)
        begin
        set @AttrNames = @AttrNames +  ', Website = '  + '''' + @Website + '''';
        end
        
        if (@Phone is not NULL)
        begin
        set @AttrNames = @AttrNames +  ', Phone = '  + '''' + @Phone + '''';
        end
        
        if (@Address is not NULL)
        begin
        set @AttrNames = @AttrNames +  ', Address = '  + '''' + @Address + '''';
        end

        
        if (substring(@AttrNames, 1, 1)= ',')
        begin
        set @Length = LEN(@AttrNames) - 1;
        set @AttrNames = Right(@AttrNames, @Length);
        end
        
        if (@AttrNames <> '')
        begin

            set @ForExecute = 'update Publisher  set ' + @AttrNames + ' where ObjectGUID = ' + '''' + @ObjectGUID + '''';
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
	   set @Message = 'Ошибка БД при изменении свойств производителя';
	else
	   set @Message = Error_Message();
	   
    ROLLBACK TRANSACTION
    RAISERROR(@Message, 12, 1);
    Return -1;

END CATCH 

END

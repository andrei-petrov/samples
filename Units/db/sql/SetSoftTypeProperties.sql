CREATE PROCEDURE [dbo].[SetSoftTypeProperties] 
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

declare @CategoryGUID nvarchar(50);
declare @PublisherGUID nvarchar(50);
declare @ProductName nvarchar(450);
declare @VersionMajor nvarchar(450);
declare @OldName nvarchar(450);
declare @OldVersionMajor nvarchar(450);
declare @OldPublisherGUID nvarchar(50);
declare @OldPublisherName nvarchar(450);

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


        select * into #SoftType
        from openxml(@XMLDoc, '/Object', 1)
        with (PublisherGUID nvarchar(100),
              CategoryGUID nvarchar(50),
              ProductName nvarchar(450),
              VersionMajor nvarchar(450));
              
        select  @CategoryGUID = CategoryGUID, @PublisherGUID = PublisherGUID,
                @ProductName = ProductName, @VersionMajor = VersionMajor
        from    #SoftType;  

		-- проверка, что не меняем софт, на который ссылаются инспекции

        select @OldName = S.ProductName, @OldVersionMajor = S.VersionMajor, @OldPublisherGUID = S.PublisherGUID,
		       @OldPublisherName = P.Name 
        from SoftType S, Publisher P where S.ObjectGUID = @ObjectGUID and S.PublisherGUID = P.ObjectGUID;

		set @OldPublisherGUID = '{' + @OldPublisherGUID + '}';

		if ( (@ProductName <> @OldName or @VersionMajor <> @OldVersionMajor) 
				 or (@OldPublisherGUID <> @PublisherGUID and @OldPublisherName <> 'Издатель неизвестен' ) )		
        begin

                if exists(select top(1) * from SoftType ST, Soft S, Installation I
                         where ST.ObjectGUID = S.SoftTypeGUID and S.ObjectGUID = I.SoftGUID 
                               and ST.ObjectGUID = @ObjectGUID)
                   RAISERROR('Данное ПО установлено на проинспектированных компьютерах', 12, 1);		       
		end       
        
        
        set @ProductName = Replace(@ProductName, '''', ''''''); 
        set @AttrNames = '';
        
         if (@CategoryGUID is not NULL and @CategoryGUID <> '')
           set @AttrNames = @AttrNames +  ',  CategoryGUID = '  + '''' + @CategoryGUID + '''';
         else if(@CategoryGUID = '')
           set @AttrNames = @AttrNames +  ',  CategoryGUID = NULL';
        
        if (@PublisherGUID is not NULL)
        begin
        set @AttrNames = @AttrNames +  ', PublisherGUID = '  + '''' + @PublisherGUID + '''';
        end
        
        if (@ProductName is not NULL)
        begin
        set @AttrNames = @AttrNames +  ', ProductName = '  + '''' + @ProductName + '''';
        end
        
        if (@VersionMajor is not NULL)
        begin
        set @AttrNames = @AttrNames +  ', VersionMajor = '  + '''' + @VersionMajor + '''';
        end

        
        if (substring(@AttrNames, 1, 1)= ',')
        begin
        set @Length = LEN(@AttrNames) - 1;
        set @AttrNames = Right(@AttrNames, @Length);
        end
        
        if (@AttrNames <> '')
        begin

            set @ForExecute = 'update SoftType  set ' + @AttrNames + ' where ObjectGUID = ' + '''' + @ObjectGUID + '''';
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
	   set @Message = 'Ошибка БД при изменении свойств типа ПО';
	else
	   set @Message = Error_Message();
	   
    ROLLBACK TRANSACTION
    RAISERROR(@Message, 12, 1);
    Return -1;

END CATCH 

END

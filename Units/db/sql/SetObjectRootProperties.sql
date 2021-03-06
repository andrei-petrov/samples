CREATE PROCEDURE [dbo].[SetObjectRootProperties] 
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

declare @Port int;
declare @DN nvarchar(300);
declare @DefaultPolicy bit;
declare @Version int;
declare @TurnOffBackgroundRefresh bit;
declare @RefreshInterval int;
declare @DoNotAllowExceptions bit;
declare @FileAndPrinterSharing bit;
declare @UserCompanyName nvarchar(450);
declare @IsReportTitle bit;
declare @SMTPHost nvarchar (200);
declare @String nvarchar(2000);
declare @CopyCompDescToAD bit;

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


        select * into #ObjectRootTbl
        from openxml(@XMLDoc, '/Object', 1)
        with (DefaultPolicy bit,
              DN nvarchar(300),
              Version int,
              TurnOffBackgroundRefresh bit,
              RefreshInterval int,
              DoNotAllowExceptions bit,
              FileAndPrinterSharing bit,
              UserCompanyName nvarchar(450),
              IsReportTitle bit,
              SMTPHost nvarchar (200),
              Port int,
			  CopyCompDescToAD bit);

        select @DefaultPolicy = DefaultPolicy, @Version = Version, @DN = DN,
			   @RefreshInterval = RefreshInterval, @DoNotAllowExceptions = DoNotAllowExceptions,
			   @TurnOffBackgroundRefresh = TurnOffBackgroundRefresh, 
			   @FileAndPrinterSharing = FileAndPrinterSharing, @UserCompanyName = UserCompanyName,  
			   @IsReportTitle = IsReportTitle, @SMTPHost = SMTPHost, @Port = Port, @CopyCompDescToAD = CopyCompDescToAD
        from   #ObjectRootTbl;
        
        set @DN = Replace(@DN, '''', '''''');
        set @UserCompanyName = Replace(@UserCompanyName, '''', ''''''); 
        set @SMTPHost = Replace(@SMTPHost, '''', '''''');
       
		set @UserCompanyName = Replace(@UserCompanyName, Char(10), Char(13) + Char(10));

        set @AttrNames = '';
        
        if (@Version is not NULL)
        begin
			set @String = @Version;
			set @AttrNames = @AttrNames +  ',  Version = '  + @String;
        end
        
        if (@Port is not NULL)
        begin
			set @String = @Port;
			set @AttrNames = @AttrNames +  ',  Port = '  + @String;
        end
        
        if (@DefaultPolicy is not NULL)
        begin
			set @String = @DefaultPolicy;
			set @AttrNames = @AttrNames +  ',  DefaultPolicy = '  + @String;
        end
        
        if (@UserCompanyName is not NULL)
			set @AttrNames = @AttrNames +  ',  UserCompanyName = '  + '''' + @UserCompanyName + '''';
 
        if (@DN is not NULL)
			set @AttrNames = @AttrNames +  ',  DN = '  + '''' + @DN + '''';
        
        if (@RefreshInterval is not NULL)
        begin
			set @String = @RefreshInterval;
			set @AttrNames = @AttrNames +  ',  RefreshInterval = '  + @String;
        end
        
        if (@DoNotAllowExceptions is not NULL)
        begin
			set @String = @DoNotAllowExceptions;
			set @AttrNames = @AttrNames +  ',  DoNotAllowExceptions = '  + @String;
        end
        
        if (@TurnOffBackgroundRefresh is not NULL)
        begin
			set @String = @TurnOffBackgroundRefresh;
			set @AttrNames = @AttrNames +  ',  TurnOffBackgroundRefresh = '  + @String;
        end
        
        if (@FileAndPrinterSharing is not NULL)
        begin
			set @String = @FileAndPrinterSharing;
			set @AttrNames = @AttrNames +  ',  FileAndPrinterSharing = '  + @String;
        end

        if (@IsReportTitle is not NULL)
        begin
			set @String = @IsReportTitle;
			set @AttrNames = @AttrNames +  ',  IsReportTitle = '  + @String;
        end

		if (@CopyCompDescToAD is not NULL)
        begin
			set @String = @CopyCompDescToAD;
			set @AttrNames = @AttrNames +  ',  CopyCompDescToAD = '  + @String;
        end
        
        if (@SMTPHost is not NULL)
			set @AttrNames = @AttrNames +  ',  SMTPHost = '  + '''' + @SMTPHost + '''';
        
        
        if (substring(@AttrNames, 1, 1)= ',')
        begin
			set @Length = LEN(@AttrNames) - 1;
			set @AttrNames = Right(@AttrNames, @Length);
        end
        
        if (@AttrNames <> '')
        begin

            set @ForExecute = 'update ObjectRoot  set ' + @AttrNames + ' where ObjectGUID = ' + '''' + @ObjectGUID + '''';
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
	   set @Message = 'Ошибка БД при изменении свойств рутового объекта';
	else
	   set @Message = Error_Message();
	   
    ROLLBACK TRANSACTION
    RAISERROR(@Message, 12, 1);
    Return -1;

END CATCH 

END

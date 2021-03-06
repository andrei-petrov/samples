CREATE PROCEDURE [dbo].[SetHardwareLicenseRuleProperties] 
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

declare @ObjectName nvarchar(450);
declare @Description nvarchar(4000);
declare @MinMemory int;
declare @MinFreeMemory int;
declare @MinFrequency int;
declare @MinQuantity  int;
declare @MaxMemory int;
declare @MaxFreeMemory int;
declare @MaxFrequency int;
declare @MaxQuantity  int;
declare @MaxDeviceAmount int;
declare @DeviceDesc nvarchar(450);
declare @Class nvarchar(450);
declare @CreatedForPassportGUID nvarchar(50);
declare @String nvarchar(2000);

declare @Version nvarchar(500);
declare @Service nvarchar(100);
declare @BusWidth nvarchar(500);
declare @BusLength nvarchar(500);
declare @Socket nvarchar(500);
declare @Chipset nvarchar(450);
declare @ReleaseDateFrom DateTime;
declare @ReleaseDateTo DateTime;


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

        select * into #HardLicenseRuleTbl
        from openxml(@XMLDoc, '/Object', 1)
        with (Class nvarchar(450),
              DeviceDesc nvarchar(450),
              Name nvarchar(450),
              Description nvarchar(4000),
              MinMemory int,
			  MinFreeMemory int,
			  MinFrequency int,
			  MinQuantity int,
			  MaxMemory int,
			  MaxFreeMemory int,
			  MaxFrequency int,
			  MaxQuantity int,
              CreatedForPassportGUID nvarchar(50),
              MaxDeviceAmount int,
			  Version nvarchar(500),
			  BusWidth nvarchar(500),
			  BusLength nvarchar(500),
			  Socket nvarchar(500),
			  Service nvarchar(100),	
			  Chipset nvarchar(450),
			  ReleaseDateFrom DateTime,
			  ReleaseDateTo DateTime);
      
        select @MinMemory = MinMemory, @MinFreeMemory = MinFreeMemory,
		@MinFrequency = MinFrequency, @MinQuantity = MinQuantity,
		@MaxMemory = MaxMemory, @MaxFreeMemory = MaxFreeMemory,
		@MaxFrequency = MaxFrequency, @MaxQuantity = MaxQuantity,   
        @Class = Class, @DeviceDesc = DeviceDesc, @MaxDeviceAmount = MaxDeviceAmount,
        @ObjectName = Name, @Description = Description, @CreatedForPassportGUID = CreatedForPassportGUID,
		@Version  = Version, @Service = Service, @BusWidth = BusWidth, @BusLength = BusLength,
        @Socket  = Socket, @Chipset  = Chipset, @ReleaseDateFrom  = ReleaseDateFrom, @ReleaseDateTo  = ReleaseDateTo
        from #HardLicenseRuleTbl;
        
        set @Class = Replace(@Class, '''', '''''');
        set @DeviceDesc = Replace(@DeviceDesc, '''', ''''''); 
        set @Description = Replace(@Description, '''', '''''');
        set @Version = Replace(@Version, '''', '''''');
        set @Service = Replace(@Service, '''', '''''');
        set @BusWidth = Replace(@BusWidth, '''', '''''');
        set @BusLength = Replace(@BusLength, '''', '''''');
        set @Socket = Replace(@Socket, '''', '''''');
        set @Chipset = Replace(@Chipset, '''', '''''');

        if (@Class = '*')
        begin
            Set @Class = '%%';
        end
    
        if (@DeviceDesc = '*')
        begin
            Set @DeviceDesc = '%%';
        end

		if (@Version = '*')
        begin
            Set @Version = '%%';
        end

        if (@Service = '*')
        begin
            Set @Service = '%%';
        end

        if (@BusWidth = '*')
        begin
            Set @BusWidth = '%%';
        end

        if (@BusLength = '*')
        begin
            Set @BusLength = '%%';
        end

        if (@Socket = '*')
        begin
            Set @Socket = '%%';
        end

        if (@Chipset = '*')
        begin
            Set @Chipset = '%%';
        end

 
        set @Class = Replace(@Class, '*', '%'); 
        set @DeviceDesc = Replace(@DeviceDesc, '*', '%');

		set @Version = Replace(@Version, '*', '%'); 
        set @Service = Replace(@Service, '*', '%');
		set @BusWidth = Replace(@BusWidth, '*', '%'); 
        set @BusLength = Replace(@BusLength, '*', '%');
		set @Socket = Replace(@Socket, '*', '%'); 
        set @Chipset = Replace(@Chipset, '*', '%');
        
        set @AttrNames = '';
        
        if (@Class is not NULL)
            set @AttrNames = ' Class = ' + '''' + @Class + '''';

        if (@DeviceDesc is not NULL)
            set @AttrNames = @AttrNames +  ', DeviceDesc = ' + '''' + @DeviceDesc + '''';

		if (@Version is not NULL)
            set @AttrNames = @AttrNames +  ', Version = ' + '''' + @Version + '''';

		if (@Service is not NULL)
            set @AttrNames = @AttrNames +  ', Service = ' + '''' + @Service + '''';

		if (@BusWidth is not NULL)
            set @AttrNames = @AttrNames +  ', BusWidth = ' + '''' + @BusWidth + '''';

		if (@BusLength is not NULL)
            set @AttrNames = @AttrNames +  ', BusLength = ' + '''' + @BusLength + '''';

		if (@Socket is not NULL)
            set @AttrNames = @AttrNames +  ', Socket = ' + '''' + @Socket + '''';

		if (@Chipset is not NULL)
            set @AttrNames = @AttrNames +  ', Chipset = ' + '''' + @Chipset + '''';


        if (@ObjectName is not NULL)
            set @AttrNames = @AttrNames +  ', Name = ' + '''' + @ObjectName + '''';

        if (@Description is not NULL)
            set @AttrNames = @AttrNames +  ',  Description = '  + '''' + @Description + '''';
 
        if (@CreatedForPassportGUID is not NULL)
            set @AttrNames = @AttrNames +  ',  CreatedForPassportGUID = '  + '''' + @CreatedForPassportGUID + '''';

        
        if (@MinMemory  is not NULL)
        begin
           set @String = @MinMemory;
           set @AttrNames = @AttrNames +  ',  MinMemory = '  + @String;
        end
        
        if (@MinFreeMemory  is not NULL)
        begin
           set @String = @MinFreeMemory;
           set @AttrNames = @AttrNames +  ',  MinFreeMemory = '  + @String;
        end 
        
        if (@MinFrequency  is not NULL)
        begin
           set @String = @MinFrequency;
           set @AttrNames = @AttrNames +  ',  MinFrequency = '  + @String;
        end 
        
        if (@MinQuantity  is not NULL)
        begin
           set @String = @MinQuantity;
           set @AttrNames = @AttrNames +  ',  MinQuantity = '  + @String;
        end 
        
         if (@MaxMemory  is not NULL)
        begin
           set @String = @MaxMemory;
           set @AttrNames = @AttrNames +  ',  MaxMemory = '  + @String;
        end
        
        if (@MaxFreeMemory  is not NULL)
        begin
           set @String = @MaxFreeMemory;
           set @AttrNames = @AttrNames +  ',  MaxFreeMemory = '  + @String;
        end 
        
        if (@MaxFrequency  is not NULL)
        begin
           set @String = @MaxFrequency;
           set @AttrNames = @AttrNames +  ',  MaxFrequency = '  + @String;
        end 
        
        if (@MaxQuantity  is not NULL)
        begin
           set @String = @MaxQuantity;
           set @AttrNames = @AttrNames +  ',  MaxQuantity = '  + @String;
        end
        
        if (@MaxDeviceAmount  is not NULL)
        begin
           set @String = @MaxDeviceAmount;
           set @AttrNames = @AttrNames +  ',  MaxDeviceAmount = '  + @String;
        end  
        
  
        if (substring(@AttrNames, 1, 1)= ',')
        begin
			set @Length = LEN(@AttrNames) - 1;
			set @AttrNames = Right(@AttrNames, @Length);
        end
        
        if (@AttrNames <> '')
        begin

            set @ForExecute = 'update HardwareLicenseRule set ' + @AttrNames + ' where ObjectGUID = ' + '''' + @ObjectGUID + '''';       
            execute (@ForExecute);
            if (@@Error <> 0)  set @Check = -1;
        end

		--   Time parameters set separately cause  under conversion to string seconds are lost
        
        if (@ReleaseDateFrom is not NULL)
        begin
        update HardwareLicenseRule   set ReleaseDateFrom = @ReleaseDateFrom
        where ObjectGUID = @ObjectGUID;
         if (@@Error <> 0)  set @Check = -1;
        end

		if (@ReleaseDateTo is not NULL)
        begin
        update HardwareLicenseRule   set ReleaseDateTo = @ReleaseDateTo
        where ObjectGUID = @ObjectGUID;
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
	   set @Message = 'Ошибка БД при изменении свойств правила БС оборудования';
	else
	   set @Message = Error_Message();
	   
    ROLLBACK TRANSACTION
    RAISERROR(@Message, 12, 1);
    Return -1;

END CATCH 

END



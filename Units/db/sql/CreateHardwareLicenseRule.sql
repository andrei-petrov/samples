CREATE PROCEDURE [dbo].[CreateHardwareLicenseRule] 
(@XMLObject nvarchar(max))

AS
BEGIN
	
	SET NOCOUNT ON;

declare @Object_GUID  nvarchar(50);
declare @Object_Count int;
declare @Parent_GUID  nvarchar(50);
declare @Object_Type  nvarchar(100);
declare @Object_Name nvarchar(450);
declare @XMLDoc int;
declare @Message  nvarchar(1000);
declare @Check int;
declare @ObjectType nvarchar(300);


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

    select * into #ObjectInfoTbl
    from openxml(@XMLDoc, '/Object', 1)
    with (ObjectGUID  nvarchar(50),
          CreatedForPassportGUID  nvarchar(50),
          ParentGUID nvarchar(50),
          Name nvarchar(450),
          Description nvarchar(4000),
          Class nvarchar(450),
          DeviceDesc nvarchar(450),
          MinMemory int,
		  MinFreeMemory int,
		  MinFrequency int,
		  MinQuantity int,
		  MaxMemory int,
		  MaxFreeMemory int,
		  MaxFrequency int,
		  MaxQuantity int,
		  MaxDeviceAmount int,
		  Version nvarchar(500),
		  BusWidth nvarchar(500),
		  BusLength nvarchar(500),
		  Socket nvarchar(500),
		  Service nvarchar(100),	
		  Chipset nvarchar(450),
		  ReleaseDateFrom DateTime,
		  ReleaseDateTo DateTime);
		  
    select 	@Object_GUID = ObjectGUID,  @Object_Name = Name,  @Parent_GUID = ParentGUID
    from    #ObjectInfoTbl;          



    if (@Object_GUID = '' or @Object_GUID is NULL)
    begin
        Set @Message = 'GUID объекта не определен ';
        RAISERROR(@Message, 12, 1);
        Return -1;
    end
    
    if (@Object_Name = '' or @Object_Name is NULL)
	begin
		Set @Message = 'Имя правила не определено ';
		RAISERROR(@Message, 12, 1);
		Return -1;
	end

	update #ObjectInfoTbl
    set Class = '%%'  where Class = '' or Class = '*' or Class is NULL;

    update #ObjectInfoTbl
    set DeviceDesc = '%%' where DeviceDesc = '' or DeviceDesc = '*' or DeviceDesc is NULL;


	update #ObjectInfoTbl
    set Version = '%%' where Version = '' or Version = '*' or Version is NULL;

	update #ObjectInfoTbl
    set Service = '%%' where Service = '' or Service = '*' or Service is NULL;

	update #ObjectInfoTbl
    set Socket = '%%' where Socket = '' or Socket = '*' or Socket is NULL;

	update #ObjectInfoTbl
    set Chipset = '%%' where Chipset = '' or Chipset = '*' or Chipset is NULL;

	update #ObjectInfoTbl
    set BusWidth = '%%' where BusWidth = '' or BusWidth = '*' or BusWidth is NULL;

	update #ObjectInfoTbl
    set BusLength = '%%' where BusLength = '' or BusLength = '*' or BusLength is NULL;

    
    update #ObjectInfoTbl
    set MinMemory = 0 where MinMemory  is NULL;
    
	update #ObjectInfoTbl
    set MinFreeMemory = 0 where MinFreeMemory  is NULL;
    
	update #ObjectInfoTbl
    set MinQuantity = 0 where MinQuantity  is NULL;

	update #ObjectInfoTbl
    set MinFrequency = 0 where MinFrequency  is NULL;
    
	update #ObjectInfoTbl
    set MaxMemory = 0 where MaxMemory  is NULL;
    
	update #ObjectInfoTbl
    set MaxFreeMemory = 0 where MaxFreeMemory  is NULL;
    
	update #ObjectInfoTbl
    set MaxQuantity = 0 where MaxQuantity  is NULL;
    
	update #ObjectInfoTbl
    set MaxFrequency = 0 where MaxFrequency  is NULL;

    update #ObjectInfoTbl
    set DeviceDesc = Replace(DeviceDesc, '*', '%'), Class = Replace(Class, '*', '%');


    if (@Parent_GUID = '' or @Parent_GUID is NULL)
    begin
        Set @Message = 'GUID объекта-родителя не определен';
        RAISERROR(@Message, 12, 1);
        Return -1;
    end
    
    select @ObjectType = ObjectType from ObjectRelations
    where ObjectGUID = @Parent_GUID;
    
    if(@ObjectType <> 'HardwareLicenseRuleContainer')
    begin
        Set @Message = 'Тип объекта-родителя должен быть HardwareLicenseRuleContainer';
		RAISERROR(@Message, 12, 1);
		Return -1;
    end
    
    select @Object_Count = Count(*) from HardwareLicenseRule
    where Name = @Object_Name;
    
    if(@Object_Count > 0)
    begin
        Set @Message = 'Правило белого списка оборудования с таким именем уже существует в БД';
		RAISERROR(@Message, 12, 1);
		Return -1;
    end
  

    select @Object_Count = Count(*) from HardwareLicenseRule 
    where ObjectGUID = @Object_GUID;

    if (@Object_Count > 0)
    begin
    RAISERROR('Объект с таким GUID уже существует в БД', 16, 1);
    Return -1;
    end
    else
    begin

        insert into dbo.HardwareLicenseRule 
		select ObjectGUID, ParentGUID, Name, Class, DeviceDesc, Description, MinMemory, 
		       MaxMemory, MinFreeMemory, MaxFreeMemory, MinFrequency, MaxFrequency, MinQuantity, 
                MaxQuantity, CreatedForPassportGUID, MaxDeviceAmount, Version, BusWidth, BusLength, 
				Socket, Service, Chipset, ReleaseDateFrom, ReleaseDateTo  
        from  #ObjectInfoTbl;

        SET @Check = @@ERROR; 

        if (@Check = 0)
            COMMIT TRANSACTION
        else 
        begin 
            RAISERROR('Транзакция отменена', 16, 1);
        end
    end
    
    EXEC sp_xml_removedocument @XMLDoc;

END TRY
BEGIN CATCH

    EXEC sp_xml_removedocument @XMLDoc;
    
	if (Error_Message()  is NULL or Error_Message() = '')
	   set @Message = 'Ошибка БД при создании правила БС оборудования';
	else
	   set @Message = Error_Message();
   
    ROLLBACK TRANSACTION
    RAISERROR(@Message, 16, 1);
    Return -1;

END CATCH
	
END
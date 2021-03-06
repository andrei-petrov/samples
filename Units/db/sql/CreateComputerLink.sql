CREATE PROCEDURE [dbo].[CreateComputerLink] 
(@XMLObject nvarchar(max))
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;

declare @Object_GUID  nvarchar(50);
declare @ExistComputerGUID  nvarchar(50);
declare @ComputerGUID  nvarchar(50);
declare @Object_Count int;
declare @N int;
declare @Length int;
declare @TaskGUID  nvarchar(50);
declare @Parent_GUID  nvarchar(50);
declare @Object_Name nvarchar(450);
declare @Description nvarchar(300);
declare @XMLDoc int;
declare @Status int;
declare @Message  nvarchar(1000);
declare @Message1  nvarchar(500);
declare @Message2  nvarchar(500);
declare @Check int;
declare @Count int;
declare @ParentTypeCheck int;

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
            ParentGUID nvarchar(50),
            Name nvarchar(450),
            Description nvarchar(300),
            Status int);

    select  @Object_GUID = ObjectGUID, @TaskGUID = ParentGUID,
            @Object_Name = Name, @Status = Status, @Description = Description
    from    #ObjectInfoTbl;
 

if (@Object_GUID = '' or @Object_GUID is NULL)
begin
    Set @Message = 'GUID объекта не определен ';
    RAISERROR(@Message, 12, 1);
    Return -1;
end

if (@Status is NULL)
begin
    Set @Status = 0;
end

if (@Object_Name = '' or @Object_Name is NULL)
begin
    Set @Message = 'Имя компьютера не определено ';
    RAISERROR(@Message, 12, 1);
    Return -1;
end

select @ParentTypeCheck = count(*) from Task
where ObjectGUID = @TaskGUID;
	
if(@ParentTypeCheck < 1)
begin
	Set @Message = 'Объектом-родителем  должен быть Task';
	RAISERROR(@Message, 12, 1);
	Return -1;
end

    set @Check = 0;

    select @ComputerGUID  = ObjectGUID  from Computer where  Name = @Object_Name;
    
    if (@ComputerGUID  is null)
    begin
   
        select @Parent_GUID = ObjectGUID from ObjectRelations
        where  ObjectType = 'ComputerContainer';

        set @ComputerGUID = NEWID();
        
        insert into dbo.Computer values(@ComputerGUID, @Parent_GUID, @Object_Name, @Status, @Description, NULL);  
        if (@@Error <> 0) set @Check = -1; 
  
     end

      select @Count = Count(*) from ComputerLink 
      where ComputerGUID = @ComputerGUID and ParentGUID =  @TaskGUID;

   if (@Count = 0 )
   begin 

      insert into dbo.ComputerLink values(@Object_GUID, @TaskGUID,  @ComputerGUID);
      if (@@Error <> 0) set @Check = -1;
    
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
	   set @Message = 'Ошибка БД при добавлении компьютера в задание';
	else
	   set @Message = Error_Message();
 
    ROLLBACK TRANSACTION
    RAISERROR(@Message, 16, 1);
    Return -1;

END CATCH

END
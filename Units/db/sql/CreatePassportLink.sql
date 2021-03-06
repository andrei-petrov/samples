
CREATE PROCEDURE [dbo].[CreatePassportLink] 
(@XMLObject nvarchar(max))
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;

declare @PassportGUID  nvarchar(50);
declare @ObjectGUID  nvarchar(50);
declare @ComputerGUID  nvarchar(50);
declare @Object_Type  nvarchar(100);
declare @XMLDoc int;
declare @Message  nvarchar(1000);
declare @Check int;
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
            PassportGUID  nvarchar(50),
            ObjectType  nvarchar(100),
            ParentGUID nvarchar(50),
            Name nvarchar(100));


    select @ObjectGUID = ObjectGUID, @Object_Type = ObjectType,
           @ComputerGUID = ParentGUID, @PassportGUID = PassportGUID
    from   #ObjectInfoTbl; 
 


if (@ObjectGUID = '' or @ObjectGUID is NULL)
begin
    Set @Message = 'GUID объекта не определен ';
    RAISERROR(@Message, 12, 1);
    Return -1;
end

if (@Object_Type = '' or @Object_Type is NULL)
begin
    Set @Message = 'Тип объекта не определен ';
    RAISERROR(@Message, 12, 1);
    Return -1;
end

if (@PassportGUID = '' or @PassportGUID is NULL)
begin
    Set @Message = 'GUID объекта Паспорт не определен ';
    RAISERROR(@Message, 12, 1);
    Return -1;
end

if (@ComputerGUID = '' or @ComputerGUID is NULL)
begin
    Set @Message = 'GUID объекта Компьютер не определен ';
    RAISERROR(@Message, 12, 1);
    Return -1;
end

select @ParentTypeCheck = count(*) from Computer
where ObjectGUID = @ComputerGUID;
	
if(@ParentTypeCheck < 1)
begin
	Set @Message = 'Объектом-родителем  должен быть Computer';
	RAISERROR(@Message, 12, 1);
	Return -1;
end

    set @Check = 0;

        insert into dbo.PassportLink values(@ObjectGUID, @ComputerGUID, @PassportGUID);
        if (@@Error <> 0)   set @Check = -1;

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
	   set @Message = 'Ошибка БД при присоединении паспорта к компьютеру';
	else
	   set @Message = Error_Message();
   
    ROLLBACK TRANSACTION
    RAISERROR(@Message, 16, 1);
    Return -1;

END CATCH

END
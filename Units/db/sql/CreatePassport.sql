CREATE PROCEDURE [dbo].[CreatePassport] 
(@XMLObject nvarchar(max))

AS
BEGIN
	
	SET NOCOUNT ON;

declare @PassportGUID  nvarchar(50);
declare @Object_Count int;
declare @PassportParentGUID  nvarchar(50);
declare @Object_Name nvarchar(450);
declare @XMLDoc int;
declare @Message  nvarchar(1000);
declare @Check int;
declare @Description nvarchar(1000);
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
          ParentGUID nvarchar(50),
          Name nvarchar(450),
          Description nvarchar(1000));

    select @PassportGUID = ObjectGUID, @PassportParentGUID = ParentGUID,       
           @Object_Name = Name, @Description = Description
    from   #ObjectInfoTbl;
           

    if (@PassportGUID = '' or @PassportGUID is NULL)
    begin
        Set @Message = 'GUID паспорта не определен ';
        RAISERROR(@Message, 12, 1);
        Return -1;
    end

    if (@PassportParentGUID = '' or @PassportParentGUID is NULL)
    begin
        Set @Message = 'GUID объекта-родителя не определен ';
        RAISERROR(@Message, 12, 1);
        Return -1;
    end
    
    if (@Object_Name = '' or @Object_Name is NULL)
	begin
		Set @Message = 'Имя паспорта не определено ';
		RAISERROR(@Message, 12, 1);
		Return -1;
	end
	
	select @ObjectType = ObjectType from ObjectRelations
    where ObjectGUID = @PassportParentGUID;
    
    if(@ObjectType <> 'PassportContainer')
    begin
        Set @Message = 'Тип объекта-родителя должен быть PassportContainer';
		RAISERROR(@Message, 12, 1);
		Return -1;
    end
    
    select @Object_Count = Count(*) from Passport
    where Name = @Object_Name;
    
    if(@Object_Count > 0)
    begin
        Set @Message = 'Паспорт с таким именем уже существует в БД';
		RAISERROR(@Message, 12, 1);
		Return -1;
    end

    select @Object_Count = Count(*) from Passport
    where ObjectGUID = @PassportGUID;

    if (@Object_Count > 0)
    begin
    RAISERROR('Объект с таким GUID уже существует в БД', 16, 1);
    Return -1;
    end
    else
    begin

            set @Check = 0; 

			insert into dbo.Passport values(@PassportGUID, @PassportParentGUID, @Object_Name, @Description);  
			 if (@@Error <> 0)   set @Check = -1; 

			if ( @Check = 0  )
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
	   set @Message = 'Ошибка БД при создании паспорта';
	else
	   set @Message = Error_Message();
   
    ROLLBACK TRANSACTION
    RAISERROR(@Message, 16, 1);
    Return -1;

END CATCH
	
END
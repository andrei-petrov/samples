---------------------------------------------------------------------------------------------------------
-- Author:      Dmitry Karpov
-- Date:        19.01.2010
-- Notes:       Create license image (picture or document).
--
-- Parameters:  
--				@ObjectGUID - guid of computer
--				@ParentGUID - guid of LicenseRule 
--				@Name 
--				@LicenseType  - type of license document, e.g. png or doc
--				@Description 
--				@BinaryData   - binary data
---------------------------------------------------------------------------------------------------------
CREATE PROCEDURE [dbo].[CreateLicenseImage] 
(@ObjectGUID nvarchar(50), @ParentGUID nvarchar(50), @Name nvarchar(450), @LicenseType nvarchar(10), @Description nvarchar(4000), @BinaryData varbinary(max)) 

AS
BEGIN

SET NOCOUNT ON;
	
declare @Message  nvarchar(1000);
declare @Check int;
declare @Object_Count int;
declare @ParentTypeCheck int;

SET DATEFORMAT ymd;


BEGIN TRY

    BEGIN TRANSACTION 

-- check input parameters 
	if (@ObjectGUID = '' or @ObjectGUID is NULL)
	begin
		Set @Message = 'GUID объекта не определен ';
		RAISERROR(@Message, 12, 1);
		Return -1;
	end

	if (@ParentGUID = '' or @ParentGUID is NULL)
	begin
	    Set @Message = 'GUID объекта-родителя не определен ';
	    RAISERROR(@Message, 12, 1);
	    Return -1;
	end

	if (@Name = '' or @Name is NULL)
	begin
	    Set @Message = 'Имя изображения не определено ';
	    RAISERROR(@Message, 12, 1);
	    Return -1;
	end

	if (@LicenseType = '' or @LicenseType is NULL)
	begin
	    Set @Message = 'Тип файла лицензии не определен ';
	    RAISERROR(@Message, 12, 1);
	    Return -1;
	end

	if (@BinaryData is NULL)
	begin
		Set @Message = 'Данные файла лицензии не заданы ';
		RAISERROR(@Message, 12, 1);
		Return -1;
	end
	
	select @ParentTypeCheck = count(*) from LicenseRule
	where ObjectGUID = @ParentGUID;
		
	if(@ParentTypeCheck < 1)
	begin
		Set @Message = 'Объектом-родителем  должен быть LicenseRule';
		RAISERROR(@Message, 12, 1);
		Return -1;
	end


-- check ObjectGuid parameter
	select @Object_Count = Count(*) from LicenseImage
    where ObjectGUID = @ObjectGUID;

	if (@Object_Count > 0)
	begin
		RAISERROR('Объект с таким GUID уже существует в БД', 16, 1);
		Return -1;
	end


	insert into dbo.LicenseImage values(@ObjectGUID, @Name, @ParentGUID, @LicenseType, @Description, @BinaryData);  
	SET @Check = @@ERROR;    

	if (@Check = 0)
	COMMIT TRANSACTION
	else 
	begin 
		 RAISERROR('Транзакция отменена', 16, 1);
	end

END TRY
BEGIN CATCH

	if (Error_Message()  is NULL or Error_Message() = '')
	   set @Message = 'Ошибка БД при добавлении изображения лицензии';
	else
	   set @Message = Error_Message();
   
    ROLLBACK TRANSACTION
    RAISERROR(@Message, 16, 1);
    Return -1;
END CATCH

END
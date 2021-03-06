CREATE PROCEDURE [dbo].[CreateLicenseRuleLink] 
(@XMLObject nvarchar(max))
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;

declare @LinkGUID  nvarchar(50);
declare @LicenseRuleContainerGUID  nvarchar(50);
declare @LicenseRuleGUID  nvarchar(50);
declare @ObjectGUID  nvarchar(50);
declare @PassportGUID  nvarchar(50);
declare @Object_Type  nvarchar(100);
declare @RuleName nvarchar(450);
declare @XMLDoc int;
declare @Message  nvarchar(1000);
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
              ObjectType  nvarchar(100),
              ParentGUID nvarchar(50),
              LicenseRuleGUID nvarchar(50));


    select @LinkGUID = ObjectGUID, @Object_Type = ObjectType,
           @PassportGUID = ParentGUID, @LicenseRuleGUID = LicenseRuleGUID
    from #ObjectInfoTbl; 


if (@LinkGUID = '' or @LinkGUID is NULL)
begin
    Set @Message = 'GUID создаваемого объекта не задан ';
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
    Set @Message = 'GUID объекта Passport не определен ';
    RAISERROR(@Message, 12, 1);
    Return -1;
end

if (@LicenseRuleGUID = '' or @LicenseRuleGUID is NULL)
begin
    Set @Message = 'GUID правила белого списка не задан';
    RAISERROR(@Message, 12, 1);
    Return -1;
end

select @ParentTypeCheck = count(*) from Passport
where ObjectGUID = @PassportGUID;
	
if(@ParentTypeCheck < 1)
begin
	Set @Message = 'Объектом-родителем  должен быть Passport';
	RAISERROR(@Message, 12, 1);
	Return -1;
end

set @Check = 0;


   -- Is there this rule already in defined passport?
    select @Count = Count(*) from LicenseRuleLink 
    where LicenseRuleGUID = @LicenseRuleGUID and ParentGUID =  @PassportGUID;
        
    if (@Count = 0 )
    begin       

		 insert into dbo.LicenseRuleLink values(@LinkGUID, @PassportGUID, @LicenseRuleGUID);
		 if (@@Error <> 0)   set @Check = -1;
	
    end
    else 
    begin 
          select @RuleName = Name from LicenseRule
          where ObjectGuid = @LicenseRuleGUID; 
          
          Set @Message = 'Правило с именем ' + @RuleName + ' уже добавлено в паспорт';
	      RAISERROR(@Message, 16, 1);
	      Return -1;
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
	   set @Message = 'Ошибка БД при добавлении правила БС ПО в паспорт';
	else
	   set @Message = Error_Message();
   
    ROLLBACK TRANSACTION
    RAISERROR(@Message, 16, 1);
    Return -1;

END CATCH

END
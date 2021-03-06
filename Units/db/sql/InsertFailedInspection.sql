CREATE PROCEDURE [dbo].[InsertFailedInspection] 
(@ObjectGUID nvarchar(50), @InspectionError nvarchar(2000), @InspectionType smallint)
-- Adding installed soft information from XML 
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;

declare @Message  nvarchar(4000);

SET DATEFORMAT ymd;


if (@ObjectGUID is not NULL)
begin

BEGIN TRY      

    insert into dbo.Action values(NEWID(), 1, getutcdate(),  0, @InspectionError, @InspectionType,  @ObjectGUID, getutcdate(), NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
    

END TRY
BEGIN CATCH

    if (Error_Message()  is NULL or Error_Message() = '')
	   set @Message = 'Ошибка БД при добавлении ошибочной инспекции';
	else
	   set @Message = Error_Message();
   
    RAISERROR(@Message, 12, 1);
    Return -1;

END CATCH

end

END

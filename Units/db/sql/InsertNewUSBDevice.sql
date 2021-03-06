CREATE PROCEDURE [dbo].[InsertNewUSBDevice] 
(@XMLInspectionData nvarchar(max), @InspectionGUID uniqueidentifier)
-- For execution only from  SP InsertNewInspectionData
AS
BEGIN
	-- SET NOCOUNT ON added to prevent extra result sets from
	-- interfering with SELECT statements.
	SET NOCOUNT ON;

declare @XML int;

declare @Message  nvarchar(1000);
declare @Message1  nvarchar(500);
declare @Message2  nvarchar(500);

-- for  transaction
declare @Check int;
set @Check = 0;

SET DATEFORMAT ymd;

BEGIN TRY

    set @XMLInspectionData = RTRIM(@XMLInspectionData);

    EXEC sp_xml_preparedocument @XML OUTPUT, @XMLInspectionData;

END TRY
BEGIN CATCH

    Set @Message = Error_Message();
    RAISERROR(@Message, 12, 1);
    Return -1;

END CATCH

BEGIN TRY

        CREATE TABLE #XMLUSBStoreTable(
                 [Name] [nvarchar](450) collate Cyrillic_General_CI_AS NULL, 
                 [DeviceType] [nvarchar](450) collate Cyrillic_General_CI_AS NULL, 
				 [Description] [nvarchar](1000) collate Cyrillic_General_CI_AS NULL,
				 [SerialNumber] [nvarchar](1000) collate Cyrillic_General_CI_AS NULL, 
				 [LastUsing] [datetime] NULL,
				 [Status] [int] NULL,
             ) ON [PRIMARY]; 

        insert into #XMLUSBStoreTable                
        select *
        from openxml(@XML, '/ResearchInfo/USB_Devices/DeviceInfo', 1)
            with ( Name nvarchar(450),
                   DeviceType  nvarchar(450),
				   Description nvarchar(1000),
				   SerialNumber nvarchar(1000), 
				   LastUsing datetime,
				   Status int);

		insert into dbo.USBDevice 
		select NEWID(), @InspectionGUID,  Name, DeviceType, Description, SerialNumber,  LastUsing, Status
		from #XMLUSBStoreTable; 
		if (@@Error <> 0)   set @Check = -1;   

if (@Check <> 0)
begin
    Set @Message = 'Ошибка при добавлении записи в таблицу ';
    RAISERROR(@Message, 12, 1);
end

EXEC sp_xml_removedocument @XML;
          

END TRY
BEGIN CATCH

    EXEC sp_xml_removedocument @XML;
    Set @Message1 = Error_Message();
    Set @Message2 = 'Ошибка добавления новых файлов в БД: ';
    Set @Message = @Message2 + @Message1;
    RAISERROR(@Message, 12, 1);
    Return -1;

END CATCH 

END 


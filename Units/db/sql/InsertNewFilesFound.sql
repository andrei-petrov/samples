CREATE PROCEDURE [dbo].[InsertNewFilesFound] 
(@XMLInspectionData nvarchar(max), @InspectionGUID uniqueidentifier, @AddingDate  Datetime)
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

        CREATE TABLE #XMLFilesFoundTable(
                 [SearchMask] [nvarchar](300) collate Cyrillic_General_CI_AS NULL,
                 [Item] [nvarchar](300) collate Cyrillic_General_CI_AS NULL,
                 [Size]               [bigint]
             ) ON [PRIMARY]; 

        insert into #XMLFilesFoundTable                
        select *
        from openxml(@XML, '/ResearchInfo/SearchInfo/Found', 1)
            with ( SearchMask nvarchar(300),
                   Item nvarchar(300),
                   Size  bigint);

		insert into dbo.FilesFound 
		select NEWID(), @InspectionGUID, @AddingDate, SearchMask, Item, Size
		from #XMLFilesFoundTable; 
		if (@@Error <> 0)   set @Check = -1;   

if (@Check <> 0)
begin
    Set @Message = 'Ошибка при добавлении записи в таблицу ';
    RAISERROR(@Message, 12, 1);
end
          

END TRY
BEGIN CATCH

    EXEC sp_xml_removedocument @XML;
    Set @Message1 = Error_Message();
    Set @Message2 = 'Ошибка добавления новых файлов в БД: ';
    Set @Message = @Message2 + @Message1;
    RAISERROR(@Message, 12, 1);
    Return -1;

END CATCH 

BEGIN TRY

    EXEC sp_xml_removedocument @XML;

END TRY
BEGIN CATCH

    Set @Message = Error_Message();
    RAISERROR(@Message, 12, 1);
    Return -1;

END CATCH

END 


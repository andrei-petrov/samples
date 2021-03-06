CREATE PROCEDURE [dbo].[AddChipsetLibrary] 
(@XMLObject nvarchar(max))

AS
BEGIN
	
	SET NOCOUNT ON;


SET DATEFORMAT ymd;

declare @XMLDoc int;
declare @Message  nvarchar(1000);

declare @XML int;
declare @N int;
declare @I int;
declare @Vendor nvarchar(150);
declare @Name nvarchar(150);
declare @Northbridge nvarchar(150);
declare @CodeName nvarchar(150);


-- for  transaction
declare @Check int;
set @Check = 0;

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


CREATE TABLE #XMLTable(
          [ID] [int] IDENTITY(1,1) NOT NULL,
          [Vendor] [nvarchar](150) collate Cyrillic_General_CI_AS NULL,
          [Name] [nvarchar](150) collate Cyrillic_General_CI_AS NULL,
          [Northbridge] [nvarchar](150) collate Cyrillic_General_CI_AS NULL,
          [CodeName] [nvarchar](150) collate Cyrillic_General_CI_AS NULL,
     ) ON [PRIMARY];


insert into #XMLTable                
select *
from openxml(@XMLDoc, '/ChipseInfo/Vendor/Chipset', 2)
    WITH(Vendor nvarchar(150) '../@Name', 
         Name nvarchar(150) '@Name', 
         Northbridge nvarchar(150) '@Northbridge', 
         CodeName nvarchar (150) '@CodeName'); 
 

select @N = count(*) from #XMLTable;
set @I = 1;

while (@I <= @N)
begin
       select @Vendor = Vendor, @Name = Name, @Northbridge = Northbridge, @CodeName = CodeName
       from  #XMLTable where ID = @I;

       IF NOT EXISTS(select top(1) * from  ChipsetInfo 
                     where  Vendor = @Vendor and ChipsetName = @Name and Northbridge = @Northbridge and ChipsetCodeName = @CodeName)
       begin 
              insert into dbo.ChipsetInfo values(@Vendor, @Name, @Northbridge, @CodeName);
			  if (@@Error <> 0)   set @Check = -1;
       end
              
       set @I = @I + 1;
end


if (@Check = 0)
 COMMIT TRANSACTION
else 
begin 
  Set @Message = 'Транзакция отменена ';
  RAISERROR(@Message, 12, 1);
end

EXEC sp_xml_removedocument @XMLDoc;

delete #XMLTable;

END TRY
BEGIN CATCH

     EXEC sp_xml_removedocument @XMLDoc;

     delete #XMLTable;
     
     if (Error_Message()  is NULL or Error_Message() = '')
	   set @Message = 'Ошибка при добавлении библиотеки чипсетов';
	 else
	   set @Message = Error_Message();
	   
     ROLLBACK TRANSACTION
     RAISERROR(@Message, 12, 1);
     Return -1;

END CATCH


	
END



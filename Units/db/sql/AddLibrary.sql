CREATE PROCEDURE [dbo].[AddLibrary] 
(@XMLObject nvarchar(max))

AS
BEGIN
	
	SET NOCOUNT ON;


SET DATEFORMAT ymd;

declare @XMLDoc int;
declare @Message  nvarchar(1000);

declare @XML int;
declare @N int;
declare @Date  Datetime;
declare @Message1  nvarchar(500);
declare @Message2  nvarchar(500);
declare @PublisherContainerGUID uniqueidentifier;
declare @UndefinedCategoryGUID uniqueidentifier;
declare @CategoryGUID uniqueidentifier;
declare @SoftTypeContainerGUID uniqueidentifier;
declare @CategoryContainerGUID uniqueidentifier;
declare @PublisherGUID uniqueidentifier;
declare @DBPublisherGUID uniqueidentifier;
declare @SoftTypeGUID uniqueidentifier;
declare @ProductGUID uniqueidentifier;
declare @FileGUID uniqueidentifier;
declare @VersionMajor nvarchar(500);
declare @Phone nvarchar(100);
declare @Address nvarchar(1000);

declare @PublisherName nvarchar(450);
declare @Website nvarchar(1000);
declare @FileName nvarchar(1000);
declare @ProductName nvarchar(450);
declare @CategoryName nvarchar(450);

declare @FileVersion nvarchar(450);
declare @Company nvarchar(450);
declare @FileProductName nvarchar(450);
declare @FileProductVersion nvarchar(450);
declare @Language nvarchar(450);
declare @Size bigint;

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
          [PublisherName] [nvarchar](450) collate Cyrillic_General_CI_AS NULL,
          [Website] [nvarchar](500) collate Cyrillic_General_CI_AS NULL,
          [Phone] [nvarchar](100) collate Cyrillic_General_CI_AS NULL,
          [Address] [nvarchar](1000) collate Cyrillic_General_CI_AS NULL,
          [ProductName] [nvarchar](450) collate Cyrillic_General_CI_AS NULL,
          [VersionMajor] [nvarchar](200) collate Cyrillic_General_CI_AS NULL,
          [FileVersion] [nvarchar](450) collate Cyrillic_General_CI_AS NULL,
          [OriginalFileName] [nvarchar](450) collate Cyrillic_General_CI_AS NULL,  
          [Company] [nvarchar](450) collate Cyrillic_General_CI_AS NULL,
          [FileProductName] [nvarchar](450) collate Cyrillic_General_CI_AS NULL, 
          [FileProductVersion] [nvarchar](450) collate Cyrillic_General_CI_AS NULL,  
          [Language] [nvarchar](450) collate Cyrillic_General_CI_AS NULL,
          [Size] [bigint] NULL,
          [Category] [nvarchar](450) collate Cyrillic_General_CI_AS NULL,
          [CategoryGUID] [uniqueidentifier] NULL
     ) ON [PRIMARY]; 

CREATE TABLE #XMLPublisherTable(
          [PublisherGUID] [uniqueidentifier] NULL,
          [PublisherName] [nvarchar](450) collate Cyrillic_General_CI_AS NULL,
          [Website] [nvarchar](500) collate Cyrillic_General_CI_AS NULL,
          [Phone] [nvarchar](100) collate Cyrillic_General_CI_AS NULL,
          [Address] [nvarchar](1000) collate Cyrillic_General_CI_AS NULL,
     ) ON [PRIMARY];

CREATE TABLE #XMLProductTable(
          [ProductGUID] [uniqueidentifier]  NULL,
          [ProductName] [nvarchar](450) collate Cyrillic_General_CI_AS NULL,
          [VersionMajor] [nvarchar](200) collate Cyrillic_General_CI_AS NULL,
          [CategoryGUID] [uniqueidentifier] NULL,
     ) ON [PRIMARY];
     
CREATE TABLE #XMLCategoryTable(
          [Category] [nvarchar](450) collate Cyrillic_General_CI_AS NULL,
     ) ON [PRIMARY];

CREATE TABLE #XMLFileTable(
          [FileGUID] [uniqueidentifier]  NULL,
          [FileVersion] [nvarchar](450) collate Cyrillic_General_CI_AS NULL,
          [OriginalFileName] [nvarchar](450) collate Cyrillic_General_CI_AS NULL,  
          [Company] [nvarchar](450) collate Cyrillic_General_CI_AS NULL,
          [FileProductName] [nvarchar](450) collate Cyrillic_General_CI_AS NULL, 
          [FileProductVersion] [nvarchar](450) collate Cyrillic_General_CI_AS NULL,  
          [Language] [nvarchar](450) collate Cyrillic_General_CI_AS NULL,
          [Size] [bigint] NULL
     ) ON [PRIMARY];



insert into #XMLTable                
select *
from openxml(@XMLDoc, '/Library/Publisher/Product/ProductFile', 2)
    WITH(PublisherName nvarchar(450) '../../@PublisherName', 
         Website nvarchar(500) '../../@PublisherWWW', 
         Phone nvarchar (100) '../../@PublisherPhone',
         Address nvarchar (1000) '../../@PublisherAddress',
         ProductName nvarchar(450) '../@ProductName', 
         VersionMajor nvarchar(450) '../@ProductVersion', 
		 FileVersion nvarchar(450) '@FileVersion',
		 OriginalFileName nvarchar(450) '@OriginalFileName',  
		 Company nvarchar(450) '@Company',
		 FileProductName nvarchar(450) '@FileProductName', 
		 FileProductVersion nvarchar(450) '@FileProductVersion',  
		 Language nvarchar(450)  '@Language',
		 Size bigint '@FileSize',
         Category nvarchar(450) '../@Category',
         CategoryGUID nvarchar(50));  --NULL 

update #XMLTable
set FileVersion = '%%' where FileVersion is NULL;
if (@@Error <> 0)   set @Check = -1;

update #XMLTable
set Phone = '' where Phone is NULL;
if (@@Error <> 0)   set @Check = -1;

update #XMLTable
set Address = '' where Address is NULL;
if (@@Error <> 0)   set @Check = -1;

select @CategoryContainerGUID = ObjectGUID from ObjectRelations where ObjectType = 'CategoryContainer';
select @PublisherContainerGUID = ObjectGUID from ObjectRelations where ObjectType = 'PublisherContainer';
select @SoftTypeContainerGUID = ObjectGUID from ObjectRelations where ObjectType = 'SoftTypeContainer';


insert into #XMLPublisherTable                
select distinct NULL, PublisherName, Website, Phone, Address
from #XMLTable;
if (@@Error <> 0)   set @Check = -1;

update #XMLPublisherTable
set PublisherGUID = NEWID();
if (@@Error <> 0)   set @Check = -1;
 
insert into #XMLCategoryTable                
select distinct Category
from #XMLTable
where Category <> '' and Category is not NULL;
if (@@Error <> 0)   set @Check = -1;

------------------------------------ insert Category if there is not such in DB---------------

-- insurance for a case of error, to avoid eternal cycle
delete from #XMLCategoryTable where Category is NULL;

while exists (select top(1) * from #XMLCategoryTable)
begin

		select @CategoryName = Category from #XMLCategoryTable;

        IF NOT EXISTS(select top(1) * from  Category where  Name = @CategoryName)   
        begin  
				insert into dbo.Category values(NEWID(), @CategoryContainerGUID, @CategoryName, NULL, 1);
				if (@@Error <> 0)   set @Check = -1;
        end

        delete from #XMLCategoryTable
		where Category = @CategoryName;

end

update #XMLTable
set CategoryGUID = Category.ObjectGUID
from Category, #XMLTable
where #XMLTable.Category = Category.Name;

while exists (select top(1) * from #XMLPublisherTable)
begin

------------------------------------ insert Publisher if there is not such in DB---------------

		select @PublisherGUID = PublisherGUID, @PublisherName = PublisherName,
		       @Website = Website, @Phone = Phone, @Address = Address		       
        from #XMLPublisherTable;

        IF NOT EXISTS( select top(1) * from Publisher 
                       where  Name = @PublisherName 
                         and  (Website = @Website or (Website is NULL and @Website is NULL))
                         and  (Phone = @Phone or (Phone is NULL and @Phone is NULL))
                         and  (Address = @Address or (Address is NULL and @Address is NULL)) 
                     )      
        begin
                insert into dbo.Publisher values (@PublisherGUID, @PublisherContainerGUID, @PublisherName, @Website, @Phone, @Address, 1);
				if (@@Error <> 0)   set @Check = -1;  
        end

------------------------------------ insert Product if there is not such in DB---------------

		insert into #XMLProductTable                
		select distinct NULL, ProductName, VersionMajor, CategoryGUID
		from #XMLTable where PublisherName =  @PublisherName 
		                 and  (Website = @Website or (Website is NULL and @Website is NULL))
                         and  (Phone   = @Phone   or (Phone   is NULL and @Phone   is NULL))
                         and  (Address = @Address or (Address is NULL and @Address is NULL)) ;
        if (@@Error <> 0)   set @Check = -1;

        delete from #XMLProductTable where ProductName is NULL;

        update #XMLProductTable
        set ProductGUID = NEWID();
        if (@@Error <> 0)   set @Check = -1;

        while exists (select top(1) * from #XMLProductTable)
		begin

				select @ProductGUID = ProductGUID, @ProductName = ProductName, @VersionMajor = VersionMajor, @CategoryGUID = CategoryGUID  
				from #XMLProductTable;

                set @SoftTypeGUID = NULL;

                select @SoftTypeGUID = ST.ObjectGUID from SoftType ST, Publisher P
				where ST.PublisherGUID = P.ObjectGUID  
					  and P.Name = @PublisherName 
					  and ST.ProductName = @ProductName 
					  and (ST.VersionMajor = @VersionMajor or (ST.VersionMajor is NULL and @VersionMajor is NULL))
					  and (P.Website = @Website or (P.Website is NULL and @Website is NULL))
					  and (P.Phone   = @Phone   or (P.Phone   is NULL and @Phone   is NULL))
					  and (P.Address = @Address or (P.Address is NULL and @Address is NULL));

                if (@SoftTypeGUID is NULL)     
				begin
                        select @DBPublisherGUID = ObjectGUID from Publisher
                        where Name = @PublisherName 
                         and (Website = @Website or (Website is NULL and @Website is NULL))
					     and (Phone   = @Phone   or (Phone   is NULL and @Phone   is NULL))
					     and (Address = @Address or (Address is NULL and @Address is NULL));
                        
						insert into dbo.SoftType  
						values(@ProductGUID,  @SoftTypeContainerGUID, @DBPublisherGUID, @ProductName, @VersionMajor,  @CategoryGUID, 1); 
						if (@@Error <> 0)   set @Check = -1; 

                        set @SoftTypeGUID = @ProductGUID;

				end 

                insert into #XMLFileTable                
				select distinct NULL, FileVersion, OriginalFileName, Company, 
				                FileProductName, FileProductVersion, Language, Size
				from #XMLTable 
                where PublisherName = @PublisherName  and ProductName = @ProductName 
                   and (VersionMajor = @VersionMajor or (VersionMajor is NULL and @VersionMajor is NULL))
				   and (Website = @Website or (Website is NULL and @Website is NULL))
				   and (Phone   = @Phone   or (Phone   is NULL and @Phone   is NULL))
				   and (Address = @Address or (Address is NULL and @Address is NULL));
				if (@@Error <> 0)   set @Check = -1; 

                delete from #XMLFileTable where OriginalFileName is NULL;

                update #XMLFileTable
				set FileGUID = NEWID();
				if (@@Error <> 0)   set @Check = -1;

                while exists (select top(1) * from #XMLFileTable)
				begin

						select @FileGUID = FileGUID, @FileName = OriginalFileName, 
						       @FileVersion = FileVersion, @Company = Company, 
				               @FileProductName = FileProductName,  @Size = Size,
				               @FileProductVersion = FileProductVersion, @Language = Language      
						from #XMLFileTable;

						IF NOT EXISTS(select top(1) * from LibProductFile L, SoftType ST, Publisher P
									  where L.OriginalFileName = @FileName
										and L.ParentGUID = ST.ObjectGUID
										and ST.ProductName = @ProductName
										and ST.PublisherGUID = P.ObjectGUID
										and P.Name = @PublisherName 
										and (@FileVersion = L.FileVersion or (L.FileVersion is NULL and @FileVersion is NULL))
										and (@Company = L.Company or (L.Company is NULL and @Company is NULL))
										and (@FileProductName = L.ProductName or (L.ProductName is NULL and @FileProductName is NULL))
										and (@Size = L.Size or (L.Size is NULL and @Size is NULL))
										and (@FileProductVersion = L.ProductVersion or (L.ProductVersion is NULL and @FileProductVersion is NULL))
										and (@Language = L.Language or (L.Language is NULL and @Language is NULL))
										and (ST.VersionMajor = @VersionMajor or (ST.VersionMajor is NULL and @VersionMajor is NULL))
										and (P.Website = @Website or (P.Website is NULL and @Website is NULL))
										and (P.Phone   = @Phone   or (P.Phone   is NULL and @Phone   is NULL))
										and (P.Address = @Address or (P.Address is NULL and @Address is NULL))
									  )  
						begin
								insert into LibProductFile values(@FileGUID, @SoftTypeGUID, @FileVersion, @FileName,
								                                  @Company, @FileProductName, @FileProductVersion, @Language, @Size, 1);
								if (@@Error <> 0)   set @Check = -1;
						end

						delete from #XMLFileTable
						where FileGUID = @FileGUID;

				end

				delete from #XMLProductTable
				where ProductGUID = @ProductGUID;	

        end

		delete from #XMLPublisherTable
		where PublisherGUID = @PublisherGUID;

end

if (@Check = 0)
 COMMIT TRANSACTION
else 
begin 
  Set @Message = 'Транзакция отменена ';
  RAISERROR(@Message, 12, 1);
end

EXEC sp_xml_removedocument @XMLDoc;

END TRY
BEGIN CATCH

     EXEC sp_xml_removedocument @XMLDoc;
     
     if (Error_Message()  is NULL or Error_Message() = '')
	   set @Message = 'Ошибка при добавлении библиотеки ПО';
	 else
	   set @Message = Error_Message();
	   
     ROLLBACK TRANSACTION
     RAISERROR(@Message, 12, 1);
     Return -1;

END CATCH


	
END


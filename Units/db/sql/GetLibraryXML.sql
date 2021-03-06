CREATE PROCEDURE [dbo].[GetLibraryXML](@LibType smallint, @InitChars nvarchar(100), @EndChars nvarchar(100))
--  LibType = 1  -  All soft, both initial and added
--  LibType = 0  -  Only added soft
--  @InitChars, @EndChars - for Publisher, @InitChars: >=  @EndChars: <

AS
BEGIN

SET NOCOUNT ON;
	
declare @Message      nvarchar(1000);
declare @XML  XML;
declare @LibraryXML  nvarchar(max);

BEGIN TRY

if (@InitChars is NULL)
set @InitChars = '';


if (@EndChars is NULL or @EndChars = '')
begin
		SET @XML = 
				(SELECT  distinct Publisher.Name as PublisherName, Publisher.Website as PublisherWWW,
				                  Publisher.Phone as PublisherPhone, Publisher.Address as PublisherAddress,
						 Product.ProductName,  Product.ProductVersion, 
						 Replace(Category.Name, '', '') as Category,   -- 'Replace' for needed xml structure 
						 ProductFile.OriginalFileName as OriginalFileName, 
                         ProductFile.FileVersion,  ProductFile.Company, 
                         ProductFile.ProductName as FileProductName, ProductFile.ProductVersion FileProductVersion,
                         ProductFile.Language, ProductFile.Size FileSize              
				 FROM    Publisher
						 left join (select ObjectGUID, ProductName, VersionMajor as ProductVersion, PublisherGUID, CategoryGUID from SoftType
									where InitLibFlag = 0 or InitLibFlag = @LibType ) as Product  -- 0 and 1 for all soft, 0 and 0 for new only
						 on Publisher.ObjectGUID = Product.PublisherGUID
						 left join Category on Category.ObjectGUID = Product.CategoryGUID
						 left join (select ParentGUID, OriginalFileName,  FileVersion, 
										   Company, ProductName, ProductVersion, Language, Size 
                                    from LibProductFile
									where InitLibFlag = 0 or InitLibFlag = @LibType) as ProductFile 
						 on ProductFile.ParentGUID = Product.ObjectGUID  
                 WHERE   Publisher.Name >= @InitChars 
                         and  (Publisher.InitLibFlag = 0 or Publisher.InitLibFlag = @LibType )     
				 ORDER BY PublisherName, PublisherWWW, ProductName, ProductVersion, OriginalFileName
		FOR XML AUTO,TYPE);
end
else
		begin
		SET @XML = 
				(SELECT  distinct Publisher.Name as PublisherName, Publisher.Website as PublisherWWW, 
				                  Publisher.Phone as PublisherPhone, Publisher.Address as PublisherAddress,
						 Product.ProductName,  Product.ProductVersion, 
						 Replace(Category.Name, '', '') as Category,   -- 'Replace' for needed xml structure 
						 ProductFile.OriginalFileName as OriginalFileName,  
                         ProductFile.FileVersion,  ProductFile.Company, 
                         ProductFile.ProductName as FileProductName, ProductFile.ProductVersion FileProductVersion,
                         ProductFile.Language, ProductFile.Size FileSize                 
				 FROM    Publisher
						 left join (select ObjectGUID, ProductName, VersionMajor as ProductVersion, PublisherGUID, CategoryGUID from SoftType
                                    where InitLibFlag = 0 or InitLibFlag = @LibType ) as Product  -- 0 and 1 for all soft, 0 and 0 for new only
						 on Publisher.ObjectGUID = Product.PublisherGUID
						 left join Category on Category.ObjectGUID = Product.CategoryGUID
						 left join (select ParentGUID, OriginalFileName, FileVersion, 
										   Company, ProductName, ProductVersion, Language, Size 
                                    from LibProductFile
									where InitLibFlag = 0 or InitLibFlag = @LibType) as ProductFile 
						 on ProductFile.ParentGUID = Product.ObjectGUID  
                 WHERE   Publisher.Name >= @InitChars and Publisher.Name < @EndChars
                         and  (Publisher.InitLibFlag = 0 or Publisher.InitLibFlag = @LibType)      
				 ORDER BY PublisherName, PublisherWWW, ProductName, ProductVersion, OriginalFileName
		FOR XML AUTO,TYPE);
end

set @LibraryXML = CONVERT ( nvarchar(max) , @XML );
set @LibraryXML = '<Library>' + @LibraryXML + '</Library>';

select @LibraryXML;
           
END TRY
BEGIN CATCH

    if (Error_Message()  is NULL or Error_Message() = '')
	   set @Message = 'Ошибка БД при создании файла с библиотекой ПО';
	else
	   set @Message = Error_Message();
   
    RAISERROR(@Message, 12, 1);
    Return -1;

END CATCH

END

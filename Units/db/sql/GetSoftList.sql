CREATE FUNCTION [dbo].[GetSoftList](@InspectionGUID nvarchar(50),
@PublisherList nvarchar(4000), @ProductList nvarchar(3000), @VersionList nvarchar(500), @LanguageList nvarchar(4000))
RETURNS  nvarchar(3000)
AS
BEGIN

declare @ComputerProductList nvarchar(3000);
declare @TabProductName Table(ID int IDENTITY(1,1), ProductName nvarchar(450) );
declare @TabPublisher Table(Publisher nvarchar(450) );
declare @TabProduct Table(Product nvarchar(450) );
declare @TabVersion Table(Version nvarchar(450) );
declare @TabLanguage Table(Language nvarchar(450) );
declare @Count int;
declare @I int;
declare @L int;
declare @N int;
declare @Publisher    nvarchar(450);
declare @Product    nvarchar(450);
declare @Version    nvarchar(300);
declare @Language    nvarchar(450);

set @ComputerProductList = '';


--  Fill table @TabClass from string whith parameter Class values

set @N = CHARINDEX (';', @PublisherList);

           while(@N > 0)
           begin
                set @Publisher = Left(@PublisherList, @N-1);
                set @Publisher = LTRIM(@Publisher);
                set @Publisher = RTRIM(@Publisher);
                insert into @TabPublisher values(@Publisher);

                set @L = LEN(@PublisherList);
                set @PublisherList = RIGHT(@PublisherList, @L - @N);  
                set @N = CHARINDEX (';', @PublisherList);            
           end

           set @PublisherList = LTRIM(@PublisherList);
           set @PublisherList = RTRIM(@PublisherList);
           insert into @TabPublisher values(@PublisherList);

--  Fill table @TabProduct from string whith parameter Product values

set @N = CHARINDEX (';', @ProductList);

           while(@N > 0)
           begin
                set @Product = Left(@ProductList, @N-1);
                set @Product = LTRIM(@Product);
                set @Product = RTRIM(@Product);
                insert into @TabProduct values(@Product);

                set @L = LEN(@ProductList);
                set @ProductList = RIGHT(@ProductList, @L - @N);  
                set @N = CHARINDEX (';', @ProductList);            
           end

           set @ProductList = LTRIM(@ProductList);
           set @ProductList = RTRIM(@ProductList);
           insert into @TabProduct values(@ProductList);

--  Fill table @TabVersion from string whith parameter Version values

set @N = CHARINDEX (';', @VersionList);

           while(@N > 0)
           begin
                set @Version = Left(@VersionList, @N-1);
                set @Version = LTRIM(@Version);
                set @Version = RTRIM(@Version);
                insert into @TabVersion values(@Version);

                set @L = LEN(@VersionList);
                set @VersionList = RIGHT(@VersionList, @L - @N);  
                set @N = CHARINDEX (';', @VersionList);            
           end

           set @VersionList = LTRIM(@VersionList);
           set @VersionList = RTRIM(@VersionList);
           insert into @TabVersion values(@VersionList);

--  Fill table @TabLanguage from string whith parameter Language values

set @N = CHARINDEX (';', @LanguageList);

           while(@N > 0)
           begin
                set @Language = Left(@LanguageList, @N-1);
                set @Language = LTRIM(@Language);
                set @Language = RTRIM(@Language);
                insert into @TabLanguage values(@Language);

                set @L = LEN(@LanguageList);
                set @LanguageList = RIGHT(@LanguageList, @L - @N);  
                set @N = CHARINDEX (';', @LanguageList);            
           end

           set @LanguageList = LTRIM(@LanguageList);
           set @LanguageList = RTRIM(@LanguageList);
           insert into @TabLanguage values(@LanguageList);



insert  into @TabProductName
select distinct  ST.ProductName  + ' (' + dbo.GetFullVersion(ST.VersionMajor, S.VersionMinor, S.VersionBuildMajor, S.VersionBuildMinor) + ')' as ProductName
FROM     Soft  S, SoftType ST, Publisher P, Installation I 
WHERE  S.SoftTypeGUID = ST.ObjectGUID AND  P.ObjectGUID = ST.PublisherGUID AND I.SoftGUID = S.ObjectGUID
       AND I.ParentGUID = @InspectionGUID
       AND (ST.ProductName IN (select Product from @TabProduct)) 
       AND (P.Name IN (select Publisher from @TabPublisher)) 
       AND (dbo.GetFullVersion(ST.VersionMajor, S.VersionMinor, S.VersionBuildMajor, S.VersionBuildMinor) IN (select Version from @TabVersion)) 
       AND (dbo.GetLanguage(S.Localization) IN (select Language from @TabLanguage)) 
       AND  (S.IsVisible = 1)      
ORDER BY  ProductName

select @Count = Count(*) from @TabProductName;

set @I = 1;

while (@I < = @Count)
 begin
               select @Product = ProductName from @TabProductName where ID = @I;
               set @ComputerProductList = @ComputerProductList + @Product + '; ' ;
               set @I = @I + 1; 
end

   -------------  deleting last  ', '  -----------------------------
set @ComputerProductList = RTRIM(@ComputerProductList);
set @L = LEN(@ComputerProductList); 
if (@L >0)
begin 
      set @ComputerProductList = LEFT(@ComputerProductList, @L - 1);
end

set @ComputerProductList = Replace(@ComputerProductList, '(Неизвестна)', ''); 

RETURN @ComputerProductList;

END


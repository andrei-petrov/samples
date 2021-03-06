CREATE PROCEDURE [dbo].[GetRepComputersWithSoftParams] 
(@ObjectGUIDList nvarchar(max), @SubdivisionName nvarchar(450))

AS
BEGIN

SET NOCOUNT ON;
	
declare @Message     nvarchar(1000);
declare @GUID     nvarchar(100);
declare @N int;
declare @L int;
declare @Min int;
declare @Max int;
declare @I int;
declare @ProductCount int;
declare @Parameter nvarchar(500);
declare @HideElements nvarchar(20);

declare @PublisherList nvarchar(max); 
declare @ProductList nvarchar(max); 
declare @VersionList nvarchar(2000); 
declare @LanguageList nvarchar(2000);

declare @TabGUID Table(SoftGUID nvarchar(100));
declare @TabParameter Table(ID int IDENTITY(1,1), ParameterValue nvarchar(3000)  collate Cyrillic_General_CI_AS );

BEGIN TRY

CREATE TABLE #ReportParams(
	                      [Publisher] [nvarchar](450) collate Cyrillic_General_CI_AS NULL,
	                     [ProductName] [nvarchar](450) collate Cyrillic_General_CI_AS  NULL,
	                     [Language] [nvarchar](450) collate Cyrillic_General_CI_AS  NULL,
	                     [Version] [nvarchar](100) collate Cyrillic_General_CI_AS  NULL,
                     ) ON [PRIMARY]; 

--------------------------   getting definite soft by Object GUID List  ---------------------------------------------------------------------

set @ObjectGUIDList = replace(@ObjectGUIDList, '{', '');
set @ObjectGUIDList = replace(@ObjectGUIDList, '}', '');

set @N = CHARINDEX (',', @ObjectGUIDList);

           while(@N > 0)
           begin
                set @GUID = Left(@ObjectGUIDList, @N-1);
                set @GUID = LTRIM(@GUID);
                set @GUID = RTRIM(@GUID);
                insert into @TabGUID values(@GUID);

                set @L = LEN(@ObjectGUIDList);
                set @ObjectGUIDList = RIGHT(@ObjectGUIDList, @L - @N);  
                set @N = CHARINDEX (',', @ObjectGUIDList);            
           end

           set @ObjectGUIDList = LTRIM(@ObjectGUIDList);
           set @ObjectGUIDList = RTRIM(@ObjectGUIDList);
           insert into @TabGUID values(@ObjectGUIDList);

insert into #ReportParams
select P.Name as Publisher, ST.ProductName,
       dbo.GetLanguage(S.Localization), dbo.GetFullVersion(ST.VersionMajor, S.VersionMinor, S.VersionBuildMajor, S.VersionBuildMinor)
from Soft S, SoftType ST, Publisher P
where ST.PublisherGUID = P.ObjectGUID and S.SoftTypeGUID = ST.ObjectGUID 
      and S.ObjectGUID in (select SoftGUID from @TabGUID);


------------------------------------  Is multy product name ?  -----------------------------------------------------------
--------------------------------------------------------------------------------------------------------------------------

select @ProductCount = count(DISTINCT ProductName) from  #ReportParams;

if(@ProductCount > 1)        --------------------  select ALL languages, publishers and versions for the products -----------------------
begin

set @HideElements = 'true';

         ------------------------------------------------------  ProductList  -----------------------------------------------

delete from @TabParameter;

 insert into @TabParameter
SELECT DISTINCT ProductName from #ReportParams;

    select @Min = min(ID) from @TabParameter;
    select @Max = max(ID) from @TabParameter;

    set @I = @Min;
    set @ProductList = '';

    while (@I < = @Max)
    begin
               select @Parameter = ParameterValue from @TabParameter where ID = @I;
               set @ProductList = @ProductList +@Parameter  + ';';
               set @I = @I + 1; 
    end
     
   -------------  deleting last  '; '  -----------------------------
  set @L = LEN(@ProductList);
  if(@L >0)
  begin
        set @ProductList = LEFT(@ProductList, @L - 1);
  end

  ------------------------------------------------------  PublisherList  -----------------------------------------------

delete from @TabParameter;

 insert into @TabParameter
SELECT DISTINCT P.Name as Publisher from SoftType S, #ReportParams RP, Publisher P
where S.ProductName = RP.ProductName and S.PublisherGUID = P.ObjectGUID;

    select @Min = min(ID) from @TabParameter;
    select @Max = max(ID) from @TabParameter;

    set @I = @Min;
    set @PublisherList = '';

    while (@I < = @Max)
    begin
               select @Parameter = ParameterValue from @TabParameter where ID = @I;
               set @PublisherList = @PublisherList +@Parameter  + ';';
               set @I = @I + 1; 
    end
     
   -------------  deleting last  '; '  -----------------------------
  set @L = LEN(@PublisherList);
  if(@L >0)
  begin
        set @PublisherList = LEFT(@PublisherList, @L - 1);
  end

  ------------------------------------------------------  VersionList  -----------------------------------------------

delete from @TabParameter;

 insert into @TabParameter
SELECT DISTINCT dbo.GetFullVersion(ST.VersionMajor, S.VersionMinor, S.VersionBuildMajor, S.VersionBuildMinor) AS Version from Soft S, SoftType ST, #ReportParams RP
where S.SoftTypeGUID = ST.ObjectGUID and ST.ProductName = RP.ProductName;

    select @Min = min(ID) from @TabParameter;
    select @Max = max(ID) from @TabParameter;

    set @I = @Min;
    set @VersionList = '';

    while (@I < = @Max)
    begin
               select @Parameter = ParameterValue from @TabParameter where ID = @I;
               set @VersionList = @VersionList +@Parameter  + ';';
               set @I = @I + 1; 
    end
     
   -------------  deleting last  '; '  -----------------------------
  set @L = LEN(@VersionList);
  if(@L >0)
  begin
        set @VersionList = LEFT(@VersionList, @L - 1);
  end

  ------------------------------------------------------  LanguageList  -----------------------------------------------

delete from @TabParameter;

 insert into @TabParameter
SELECT DISTINCT dbo.GetLanguage(S.Localization) AS Language from Soft S, SoftType ST, #ReportParams RP
where S.SoftTypeGUID = ST.ObjectGUID and ST.ProductName = RP.ProductName;

    select @Min = min(ID) from @TabParameter;
    select @Max = max(ID) from @TabParameter;

    set @I = @Min;
    set @LanguageList = '';

    while (@I < = @Max)
    begin
               select @Parameter = ParameterValue from @TabParameter where ID = @I;
               set @LanguageList = @LanguageList +@Parameter  + ';';
               set @I = @I + 1; 
    end
     
   -------------  deleting last  '; '  -----------------------------
  set @L = LEN(@LanguageList);
  if(@L >0)
  begin
        set @LanguageList = LEFT(@LanguageList, @L - 1);
  end


end
else            -----------------------------------------    select DEFINITED publishers, languages and versions for the product
begin 

set @HideElements = 'false';

------------------------------------------------------  VersionList  -----------------------------------------------

delete from @TabParameter;

 insert into @TabParameter
SELECT DISTINCT Version from #ReportParams;

    select @Min = min(ID) from @TabParameter;
    select @Max = max(ID) from @TabParameter;

    set @I = @Min;
    set @VersionList = '';

    while (@I < = @Max)
    begin
               select @Parameter = ParameterValue from @TabParameter where ID = @I;
               set @VersionList = @VersionList +@Parameter  + ';';
               set @I = @I + 1; 
    end
     
   -------------  deleting last  '; '  -----------------------------
  set @L = LEN(@VersionList);
  if(@L >0)
  begin
        set @VersionList = LEFT(@VersionList, @L - 1);
  end
     
------------------------------------------------------  ProductList  -----------------------------------------------

select @ProductList = ProductName from  #ReportParams;

------------------------------------------------------  PublisherList  -----------------------------------------------

delete from @TabParameter;

 insert into @TabParameter
SELECT DISTINCT Publisher from #ReportParams;

    select @Min = min(ID) from @TabParameter;
    select @Max = max(ID) from @TabParameter;

    set @I = @Min;
    set @PublisherList = '';

    while (@I < = @Max)
    begin
               select @Parameter = ParameterValue from @TabParameter where ID = @I;
               set @PublisherList = @PublisherList +@Parameter  + ';';
               set @I = @I + 1; 
    end
     
   -------------  deleting last  '; '  -----------------------------
  set @L = LEN(@PublisherList);
  if(@L >0)
  begin
        set @PublisherList = LEFT(@PublisherList, @L - 1);
  end

------------------------------------------------------  LanguageList  -----------------------------------------------

delete from @TabParameter;

 insert into @TabParameter
SELECT DISTINCT Language from #ReportParams;

    select @Min = min(ID) from @TabParameter;
    select @Max = max(ID) from @TabParameter;

    set @I = @Min;
    set @LanguageList = '';

    while (@I < = @Max)
    begin
               select @Parameter = ParameterValue from @TabParameter where ID = @I;
               set @LanguageList = @LanguageList +@Parameter  + ';';
               set @I = @I + 1; 
    end
     
   -------------  deleting last  '; '  -----------------------------
  set @L = LEN(@LanguageList);
  if(@L >0)
  begin
        set @LanguageList = LEFT(@LanguageList, @L - 1);
  end

end


select @PublisherList as Publisher, @ProductList as ProductName, @VersionList as Version,  @LanguageList as Language, @HideElements as HideElements, @SubdivisionName as SubdivisionName;
           
END TRY
BEGIN CATCH

    if (Error_Message()  is NULL or Error_Message() = '')
	   set @Message = 'БД: ошибка запроса параметров отчета';
	else
	   set @Message = Error_Message();
   
    RAISERROR(@Message, 12, 1);
    Return -1;

END CATCH

END




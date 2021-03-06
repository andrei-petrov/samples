CREATE FUNCTION [dbo].[GetSecuritySoftList]
(@InspectionGUID nvarchar(50), @CategoryName nvarchar(4000))
RETURNS  nvarchar(4000)
AS
BEGIN

declare @ComputerProductList nvarchar(4000);
declare @TabProductName Table(ID int IDENTITY(1,1), ProductName nvarchar(450) );
declare @Count int;
declare @I int;
declare @L int;
declare @Product    nvarchar(450);


set @ComputerProductList = '';


insert  into @TabProductName
select distinct  STP.ProductName  + ' (' + dbo.GetFullVersion(STP.VersionMajor, S.VersionMinor, S.VersionBuildMajor, S.VersionBuildMinor) + ')' as ProductName
FROM     Soft AS S INNER JOIN
SoftType AS STP ON STP.ObjectGUID = S.SoftTypeGUID INNER JOIN
Category AS C ON C.ObjectGUID = STP.CategoryGUID INNER JOIN
Installation AS I ON S.ObjectGUID = I.SoftGUID INNER JOIN
ViewMaxComputerAction AS V ON V.InspectionGUID = I.ParentGUID
WHERE  C.Name  =  @CategoryName AND   I.ParentGUID = @InspectionGUID AND  S.IsVisible = 1;     


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


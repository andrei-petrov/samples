CREATE FUNCTION [dbo].[GetFileCheck](@FileGUID uniqueidentifier)
-- The procedure for checking general soft list (not for definite computer)
RETURNS  smallint
AS
BEGIN

declare @FileCheck int;
declare @FileCount int;


set @FileCheck = 0;  -- red, unknown soft
-- 1 - green - found file compliance both with lib file name and with rest file properties (company etc.) 
-- 2 - blue - found file compliance only with soft, not with file name

select @FileCount  = Count(*) 
from   FoundProductFile FF, LibProductFile LF
where  FF.ObjectGUID = @FileGUID 
       -- found file compliance  with lib file
       and FF.OriginalFileName = LF.OriginalFileName 
       and FF.Company like LF.Company
       and FF.ProductName like LF.ProductName
       and FF.ProductVersion like LF.ProductVersion
       and FF.Language like LF.Language
       and (FF.Size = LF.Size or LF.Size = 0);

if (@FileCount > 0)
	 set @FileCheck = 1;     -- green 
else
begin
		select @FileCount  = Count(*) 
		from   FoundProductFile FF,  SoftType ST, Publisher P
		where  -- definite found file
			   FF.ObjectGUID = @FileGUID 
			   -- found file compliance only  with lib soft, not with lib file name
			   and FF.Company = P.Name and ST.ProductName = FF.ProductName and ST.PublisherGUID = P.ObjectGUID
			   and ST.VersionMajor = dbo.GetMajorVersionFromFullVersion(FF.ProductVersion);

        if (@FileCount > 0)
	         set @FileCheck = 2;     -- blue  
	         
end
															



RETURN @FileCheck;

END






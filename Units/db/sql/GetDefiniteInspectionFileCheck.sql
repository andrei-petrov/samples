CREATE FUNCTION [dbo].[GetDefiniteInspectionFileCheck](@InspectionGUID uniqueidentifier, @FileGUID uniqueidentifier)
-- The procedure for checking  definite computer inspection soft
RETURNS  smallint
AS
BEGIN

declare @FileCheck int;
declare @FileCount int;


set @FileCheck = 0;  -- red, unknown soft

select @FileCount  = Count(*) 
from   FoundProductFile FF, LibProductFile LF, SoftType ST, Publisher P, FileInstallation FI, 
       Installation I, Soft S
where  --  lib file relation with softtype and publisher
       LF.ParentGUID = ST.ObjectGUID and ST.PublisherGUID = P.ObjectGUID 
       -- definite found file and inspection
       and FF.ObjectGUID = @FileGUID and FI.ParentGUID =   @InspectionGUID and FI.FoundProductFileGUID = FF.ObjectGUID
       -- found file compliance  with lib file
       and FF.OriginalFileName = LF.OriginalFileName 
       and FF.Company like LF.Company
       and FF.ProductName like LF.ProductName
       and FF.ProductVersion like LF.ProductVersion
       and FF.Language like LF.Language
       and (FF.Size = LF.Size or LF.Size = 0) 
       -- found file compliance  with reestr soft
       and I.ParentGUID =   @InspectionGUID and I.SoftGUID = S.ObjectGUID and S.SoftTypeGUID = ST.ObjectGUID;


if (@FileCount > 0)
	 set @FileCheck = 1;     -- green (file is assotiated with library and the soft present on the computer
                             --        in definite inspection)
else
begin
        select @FileCount  = Count(*) 
		from   FoundProductFile FF, SoftType ST, Publisher P, FileInstallation FI, Installation I, Soft S	   
		where  -- definite found file and inspection
			   FF.ObjectGUID = @FileGUID and FI.ParentGUID =   @InspectionGUID and FI.FoundProductFileGUID = FF.ObjectGUID
			   -- found file compliance  with reestr soft
			   and ST.PublisherGUID = P.ObjectGUID  and FF.Company = P.Name and ST.ProductName = FF.ProductName 
			   and ST.VersionMajor = dbo.GetMajorVersionFromFullVersion(FF.ProductVersion)
			   and I.ParentGUID =   @InspectionGUID and I.SoftGUID = S.ObjectGUID and S.SoftTypeGUID = ST.ObjectGUID;


        if (@FileCount > 0)
			 set @FileCheck = 2;     -- blue (file is not assotiated with library but the soft present on the computer
                                     --        in definite inspection)
        else
        begin

                select @FileCount  = Count(*) 
				from   FoundProductFile FF, LibProductFile LF, SoftType ST, Publisher P, FileInstallation FI
				where  --  lib file relation with softtype and publisher
					   LF.ParentGUID = ST.ObjectGUID and ST.PublisherGUID = P.ObjectGUID 
					   -- definite found file and inspection
					   and FF.ObjectGUID = @FileGUID and FI.ParentGUID =   @InspectionGUID and FI.FoundProductFileGUID = FF.ObjectGUID
					   -- found file compliance  with lib file
					   and FF.OriginalFileName = LF.OriginalFileName 
					   and FF.Company like LF.Company
					   and FF.ProductName like LF.ProductName
					   and FF.ProductVersion like LF.ProductVersion
					   and FF.Language like LF.Language
					   and (FF.Size = LF.Size or LF.Size = 0);

				if (@FileCount > 0)
					 set @FileCheck = 3;     -- yellow (file is assotiated with library but the soft not present on the computer
											 --        in definite inspection) 
        end
end																



RETURN @FileCheck;

END
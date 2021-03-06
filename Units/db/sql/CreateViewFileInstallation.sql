
CREATE VIEW ViewFileInstallation
AS
select FI.*,  'FileInstallation' as ObjectType, F.FileVersion, F.OriginalFileName, 
       F.Company, F.ProductName, F.ProductVersion, F.Language, F.Size,
       dbo.GetDefiniteInspectionFileCheck(FI.ParentGUID, F.ObjectGUID) as FileCheck
from   FileInstallation FI, FoundProductFile F
where  FI.FoundProductFileGUID = F.ObjectGUID;


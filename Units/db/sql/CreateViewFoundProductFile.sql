
CREATE VIEW ViewFoundProductFile
AS
select F.*, 'FoundProductFile' as ObjectType,  dbo.GetFileCheck(F.ObjectGUID) as FileCheck
from FoundProductFile F;


CREATE VIEW ViewTaskComputer
AS
SELECT  C.ObjectGuid, dbo.GetPassportList(C.ObjectGuid) as PassportList, 'Computer' as ObjectType,
		C.Name, C.Status, C.Description, LSI.LastSuccessInspDate, LI.LastInspectionDate,
		S.Name as SubdivisionName, L.ParentGuid, C.SubdivisionGUID, L.ObjectGuid as LinkGuid
from    ComputerLink L 
inner join Computer C  on C.ObjectGuid = L.ComputerGuid 
left join ViewLastComputerInsp LI on C.ObjectGUID = LI.ComputerGUID
left join ViewLastSuccessComputerInsp LSI on C.ObjectGUID = LSI.ComputerGUID
left join Subdivision S on S.ObjectGUID = C.SubdivisionGUID;



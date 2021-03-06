
CREATE VIEW ViewComputer
AS
select 'Computer' as ObjectType, C.*, LI.LastInspectionDate, LSI.LastSuccessInspDate,
	   dbo.GetPassportList(C.ObjectGuid) as PassportList, S.Name as SubdivisionName
from Computer C 
left join ViewLastComputerInsp LI on C.ObjectGUID = LI.ComputerGUID
left join ViewLastSuccessComputerInsp LSI on C.ObjectGUID = LSI.ComputerGUID
left join Subdivision S on C.SubdivisionGUID = S.ObjectGUID;



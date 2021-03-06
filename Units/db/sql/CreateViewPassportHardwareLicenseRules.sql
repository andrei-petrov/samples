
CREATE VIEW ViewPassportHardwareLicenseRule
AS
SELECT     H.ObjectGuid, 'HardwareLicenseRule' as ObjectType, H.Name, 
		   H.MinMemory, H.MaxMemory, H.MinFrequency, H.MaxFrequency, H.MinFreeMemory,
		   H.MaxFreeMemory, H.MinQuantity, H.MaxQuantity, H.ReleaseDateFrom, H.ReleaseDateTo,
		   Replace(Replace(H.Class, '%%', '*'), '%', '*') as Class,
		   Replace(Replace(H.DeviceDesc, '%%', '*'), '%', '*') as DeviceDesc,
		   Replace(Replace(H.Version, '%%', '*'), '%', '*') as Version,  
		   Replace(Replace(H.BusWidth, '%%', '*'), '%', '*')  as BusWidth, 
		   Replace(Replace(H.BusLength, '%%', '*'), '%', '*') as BusLength,  
		   Replace(Replace(H.Socket, '%%', '*'), '%', '*')  as Socket,
		   Replace(Replace(H.Service, '%%', '*'), '%', '*') as Service,  
		   Replace(Replace(H.Chipset, '%%', '*'), '%', '*')  as Chipset,  
		   H.Description, H.CreatedForPassportGUID, Psp.Name as PassportName,
		   L.ParentGuid, L.ObjectGuid as LinkGuid
from       HardwareLicenseRule H
inner join HardwareLicenseRuleLink L on H.ObjectGuid = L.HardwareLicenseRuleGuid 
inner join Passport P  on P.ObjectGUID = L.ParentGUID
left join Passport Psp on H.CreatedForPassportGUID = Psp.ObjectGUID;



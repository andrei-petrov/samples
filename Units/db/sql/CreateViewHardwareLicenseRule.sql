
CREATE VIEW ViewHardwareLicenseRule 
AS
select 'HardwareLicenseRule' as ObjectType, H.Name, H.ParentGUID, H.ObjectGUID, H.CreatedForPassportGUID,
        P.Name as PassportName, H.MaxDeviceAmount, H.ReleaseDateFrom, H.ReleaseDateTo,
		H.Description, H.MinMemory, H.MaxMemory, H.MinFrequency,
		H.MaxFrequency, H.MinFreeMemory, H.MaxFreeMemory, H.MinQuantity, H.MaxQuantity,
		Replace(Replace(H.DeviceDesc, '%%', '*'), '%', '*') as DeviceDesc,  
		Replace(Replace(H.Class, '%%', '*'), '%', '*')  as Class,
		Replace(Replace(H.Version, '%%', '*'), '%', '*') as Version,  
		Replace(Replace(H.BusWidth, '%%', '*'), '%', '*')  as BusWidth, 
		Replace(Replace(H.BusLength, '%%', '*'), '%', '*') as BusLength,  
		Replace(Replace(H.Socket, '%%', '*'), '%', '*')  as Socket ,
		Replace(Replace(H.Service, '%%', '*'), '%', '*') as Service,  
		Replace(Replace(H.Chipset, '%%', '*'), '%', '*')  as Chipset  
from HardwareLicenseRule H
left join Passport P on P.ObjectGUID = H.CreatedForPassportGUID;


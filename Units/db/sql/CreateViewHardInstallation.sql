
CREATE VIEW ViewHardInstallation
AS
select HI.*, 'HardInstallation' as ObjectType, H.Class, H.DeviceDesc, H.IsVisible, H.Memory, H.Version, 
       H.Frequency, H.Quantity, H.Speed, H.WakeupType, H.BusWidth, H.BusLength,	H.Socket, H.Service, H.Chipset, 
dbo.GetHardwarePassportCheck(HI.ParentGUID, H.Class, H.DeviceDesc,
 H.Memory, HI.FreeMemory, H.Frequency, H.Quantity, HI.ReleaseDate, H.Version, H.Service,
H.BusWidth, H.BusLength, H.Chipset, H.Socket) as PassportCheck
from HardInstallation HI, Hardware H 
where HI.HardwareGUID = H.ObjectGUID



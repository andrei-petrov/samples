
CREATE VIEW ViewLicenseRule    
 AS
select 'LicenseRule' as ObjectType, L.Name, L.ParentGUID, L.ObjectGUID, L.Description, 
		L.MaxInstallAmount, L.SoftType, L.CreatedForPassportGUID, P.Name as PassportName,
		Replace(Replace(L.ProductName, '%%', '*'), '%', '*') as ProductName, 
		Replace(Replace(L.Publisher, '%%', '*'), '%', '*') as Publisher,
		Replace(Replace(L.Localization, '%%', '*'), '%', '*') as Localization, 
		Replace(Replace(L.ProductVersion, '%%', '*'), '%', '*') as ProductVersion, 
		Replace(Replace(L.ProductKey, '%%', '*'), '%', '*')  as ProductKey,
		LicenseExpire 
from  LicenseRule L
left join Passport P on L.CreatedForPassportGUID = P.ObjectGUID;


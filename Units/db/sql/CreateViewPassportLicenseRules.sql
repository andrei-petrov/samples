
CREATE VIEW ViewPassportLicenseRule
AS
SELECT LRule.ObjectGuid, 'LicenseRule' as ObjectType, LRule.Name,  
	   Replace(Replace(LRule.Publisher, '%%', '*'), '%', '*') as Publisher, 
	   Replace(Replace(LRule.ProductVersion, '%%', '*'), '%', '*') as ProductVersion, 
	   Replace(Replace(LRule.Localization, '%%', '*'), '%', '*') as Localization,  
	   Replace(Replace(LRule.ProductName, '%%', '*'), '%', '*') as ProductName, 
	   LRule.SoftType,  
	   LRule.Description,
	   LRule.MaxInstallAmount,
	   LRule.CreatedForPassportGUID,
	   Psp.Name as PassportName,
	   Replace(Replace(LRule.ProductKey, '%%', '*'), '%', '*') as ProductKey,
	   Link.ParentGuid, Link.ObjectGuid as LinkGuid
from   LicenseRule LRule 
inner join LicenseRuleLink Link on LRule.ObjectGuid = Link.LicenseRuleGuid
inner join Passport P on Link.ParentGUID = P.ObjectGUID   
left join Passport Psp on LRule.CreatedForPassportGUID = Psp.ObjectGUID;

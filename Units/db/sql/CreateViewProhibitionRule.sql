
CREATE VIEW ViewProhibitionRule   
AS
select 'ProhibitionRule' as ObjectType, P.Name, P.ParentGUID,
		P.ObjectGUID, P.Description,  P.SoftType, 
		Replace(Replace(P.ProductName, '%%', '*'), '%', '*') as ProductName, 
		Replace(Replace(P.Publisher, '%%', '*'), '%', '*') as Publisher,
		Replace(Replace(P.Localization, '%%', '*'), '%', '*') as Localization, 
		Replace(Replace(P.ProductVersion, '%%', '*'), '%', '*') as ProductVersion, 
		Replace(Replace(P.ProductKey, '%%', '*'), '%', '*')  as ProductKey 
from  ProhibitionRule P;


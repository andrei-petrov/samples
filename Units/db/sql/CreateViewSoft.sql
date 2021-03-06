
CREATE VIEW ViewSoft
AS
select 'Soft' as ObjectType, S.ParentGUID, S.ObjectGUID, S.VersionBuildMajor, S.VersionBuildMinor,
	   P.Name as Publisher, ST.PublisherGUID, S.VersionMinor, ST.VersionMajor, S.SoftTypeGUID,
       dbo.GetLanguage(S.Localization) as Localization, S.Localization as LanguageID, ST.ProductName, 
	   S.IsOperatingSystem,  S.ProductKey,  S.IsVisible, 
	   C.Name as CategoryName 
from Soft  S
inner join SoftType ST on S.SoftTypeGUID = ST.ObjectGUID
inner join Publisher P on P.ObjectGUID = ST.PublisherGUID
left join Category C on C.ObjectGUID = ST.CategoryGUID











CREATE VIEW ViewInstallation
AS
select I.*, 'Installation' as ObjectType, P.Name as Publisher, S.VersionMinor, ST.VersionMajor, 
		dbo.GetLanguage(S.Localization) as Localization,  S.IsOperatingSystem,  ST.ProductName,
		S.ProductKey,  S.IsVisible, C.Name as CategoryName, P.ObjectGUID as PublisherGUID, 
		ST.ObjectGUID as SoftTypeGUID, S.VersionBuildMajor, S.VersionBuildMinor,
dbo.GetSoftPassportCheck(I.ParentGUID, ST.ProductName, P.Name, S.Localization,
S.VersionMinor, ST.VersionMajor, S.ProductKey, S.IsOperatingSystem, S.VersionBuildMinor, S.VersionBuildMajor) as PassportCheck 
from Installation I
inner join Soft  S  on I.SoftGUID = S.ObjectGUID
inner join SoftType ST on S.SoftTypeGUID = ST.ObjectGUID
inner join Publisher P on P.ObjectGUID = ST.PublisherGUID
left join Category C on C.ObjectGUID = ST.CategoryGUID;



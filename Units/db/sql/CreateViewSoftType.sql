
CREATE VIEW ViewSoftType
AS
select  ST.ParentGUID, 'SoftType' as ObjectType,  ST.ObjectGUID, ST.PublisherGUID,
		P.Name as Publisher, ST.ProductName, 
		ST.CategoryGUID, C.Name as CategoryName,
		ST.VersionMajor, ST.InitLibFlag  
from SoftType ST
inner join Publisher P on P.ObjectGUID = ST.PublisherGUID
left join Category C on C.ObjectGUID = ST.CategoryGUID







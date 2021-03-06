CREATE FUNCTION [dbo].[GetSoftTypeGUID](@ProductName nvarchar(450))
RETURNS  nvarchar(50)
AS
BEGIN

declare @SoftTypeGUID uniqueidentifier;
declare @TabSoftType Table(SoftTypeGUID uniqueidentifier, 
                           ProductNameGroup nvarchar(450)  collate Cyrillic_General_CI_AS);

set @SoftTypeGUID = NULL;

insert into @TabSoftType
select ObjectGuid, '%' + ProductNameGroup + '%' 
from SoftType;

select  @SoftTypeGUID  = TS.SoftTypeGUID  from @TabSoftType TS
where @ProductName like TS.ProductNameGroup;


RETURN @SoftTypeGUID;

END


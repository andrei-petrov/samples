CREATE FUNCTION [dbo].[GetMajorVersionFromFullVersion](@FullVersion nvarchar(500))
RETURNS  nvarchar(500)
AS
BEGIN

declare @MajorVersion nvarchar(500);
declare @L int;


set @MajorVersion = @FullVersion;

set @L =  CHARINDEX('.', @MajorVersion);

if (@L > 0)
set @MajorVersion = SUBSTRING(@MajorVersion, 0, @L);

															
RETURN @MajorVersion;

END
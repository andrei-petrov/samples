CREATE FUNCTION [dbo].[GetCharGUID](@GUID uniqueIdentifier)
RETURNS  nvarchar(100)
AS
BEGIN

    declare @VarCharGUID nvarchar(100);

    set  @VarCharGUID = @GUID;

	RETURN @VarCharGUID;

END


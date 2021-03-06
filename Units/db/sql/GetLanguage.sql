CREATE FUNCTION [dbo].[GetLanguage](@LanguageCode nvarchar(450))
RETURNS  nvarchar(450)
AS
BEGIN

    declare @Language nvarchar(450);
    --declare @LanguageCode int;

    select @Language = Language from Localization
    where LocalizationCode = @LanguageCode;

    if (@Language is NULL or @Language = '')
    begin  
        select @Language = DefaultLanguage from ObjectRoot;    
    end
       

	RETURN @Language;

END


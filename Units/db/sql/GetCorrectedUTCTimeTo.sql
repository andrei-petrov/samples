CREATE FUNCTION [dbo].[GetCorrectedUTCTimeTo](@UTCInputTime datetime, @LocalInputTime datetime)
RETURNS  datetime
AS
BEGIN

    declare @TimeForSearch datetime;

    if ( (DATEPART(hour,    @LocalInputTime)= 0)     and
         (DATEPART(minute,  @LocalInputTime)= 0)     and 
         (DATEPART(second,  @LocalInputTime)= 0) )
    begin  
        set @TimeForSearch = DATEADD(second, 59, DATEADD(minute, 59,  DATEADD(hour, 23, @UTCInputTime)));    
    end
    else
    begin  
        set @TimeForSearch = @UTCInputTime;    
    end

	RETURN @TimeForSearch;

END



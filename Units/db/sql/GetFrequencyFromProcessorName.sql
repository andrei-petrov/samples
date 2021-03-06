

CREATE FUNCTION [dbo].[GetFrequencyFromProcessorName](@ProcessorName nvarchar(300))
RETURNS  int
AS
BEGIN


declare @I int;
declare @FrequencyInt int;
declare @FrequencyFloat float;
declare @L  int;
declare @Len  int;
declare @L_end  int;
declare @L_begin int;
declare @S nvarchar(1000);
declare @S1 nvarchar(10);
declare @StringFrequency nvarchar(100);
declare @NS nvarchar(100);
declare @PointCount int;
declare @Check int;
declare @FrequencyUnit nvarchar(10);
declare @Frequency int;

set @FrequencyInt = 0;
set @NS = '0123456789';
set @S = '';

--  search GHz or MHz instead of Hz for more probability of designation of frequency

set @L =  CHARINDEX('GHz', @ProcessorName);

if (@L >  0)
       set @FrequencyUnit = 'GHz';
else
begin
         set @L =  CHARINDEX('MHz', @ProcessorName);
         if (@L >  0)
                      set @FrequencyUnit = 'MHz';
end


if (@L > 0)
begin
          set @StringFrequency = '';
          set @S = SUBSTRING(@ProcessorName, 1, @L-1);  
          set @S = RTRIM(@S);

          set @L_end = LEN(@S);

          set @I = @L_end;
          set @L = 0;
          set @PointCount = 0;
          set @Check = 0;

          while (@I > = 0   and  @Check = 0 ) 
          begin
                    set @S1 = SUBSTRING(@ProcessorName, @I, 1);
                    set @L =  CHARINDEX(@S1, @NS)     --       is   character   a number?

                    if (@L > 0)
                               set @I = @I - 1; 
                    else 
                    begin
                              set @L =  CHARINDEX(@S1, '.')      --       is   character   a  point?
                              if (@L > 0)
                              begin
                                        set @PointCount = @PointCount + 1;

                                        if (@PointCount  < 2)
                                                set @I = @I - 1; 
									    else 
									            set @Check = 2;  --  the characters are not a number
                               end
                               else
                                        set @Check = 1;      --   to end the searching
                                        set @L_begin = @I;
                    end
           end

           if (@Check = 2)                       ----  the characters are not number
                      set @FrequencyInt = 0;
           else  
           begin
					   if (@L_begin > 0  and @L_end > @L_begin)
								  set @StringFrequency = SUBSTRING(@ProcessorName, @L_begin + 1, @L_end - @L_begin);
           end

			if (@StringFrequency <> '' and @StringFrequency is not NULL)
			begin
							if(@PointCount = 1)
							begin
									  set @L = CHARINDEX('.', @StringFrequency);
									  set @Len = LEN(@StringFrequency);
							        
									  if (@FrequencyUnit = 'MHz')
									  begin
												set  @StringFrequency =  SUBSTRING(@StringFrequency, 1, @L-1); 
												set @FrequencyInt = @StringFrequency;  
									  end 
									  else   --  @FrequencyUnit = 'GHz'
									  begin
												if ((@Len - @L)  > 3)   
												begin
														  set  @StringFrequency =  SUBSTRING(@StringFrequency, 0, @L + 4);    ---- remain 3 character after  point
												end

												set @FrequencyFloat = @StringFrequency;
												set @FrequencyFloat = @FrequencyFloat * 1000;   --   GHz => MHz;
												set @FrequencyInt = @FrequencyFloat;   

									  end                                  
							  end
							  else   --   if @PointCount = 0
							  begin
							          if (@FrequencyUnit = 'MHz') 
									        set @FrequencyInt = @StringFrequency;  
									  else   --  @FrequencyUnit = 'GHz'
									  begin
									        set @FrequencyInt = @StringFrequency;
									        set @FrequencyInt = @FrequencyInt * 1000;  --   GHz => MHz;
									  end      
							  end		                        
			 
				end 
end

if (@FrequencyInt > 0)
   set @Frequency = @FrequencyInt;
else
   set @Frequency = NULL;

RETURN @Frequency;

END
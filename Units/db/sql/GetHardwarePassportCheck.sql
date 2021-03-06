CREATE FUNCTION [dbo].[GetHardwarePassportCheck](@InspectionGUID nvarchar(50), @Class nvarchar(450),
 @DeviceDesc nvarchar(450), @Memory  int, @FreeMemory  int, @Frequency  int,
 @Quantity  int, @ReleaseDate datetime, @Version nvarchar(300), @Service nvarchar(300),
@BusWidth nvarchar(300), @BusLength nvarchar(300), @Chipset nvarchar(300), @Socket nvarchar(300))
RETURNS  smallint
AS
BEGIN

declare @PassportCheck int;
declare @ComputerGUID nvarchar(50);
declare @PassportCount smallint;
declare @DeviceCount int;

declare @TabDevice Table(Class nvarchar(450),  DeviceDesc nvarchar(450), 
                         MinMemory  int, MinFreeMemory  int, MinFrequency  int, MinQuantity  int, 
                         MaxMemory  int,  MaxFreeMemory  int, MaxFrequency  int, MaxQuantity  int,
						 ReleaseDateFrom datetime, ReleaseDateTo datetime, Version nvarchar(300), Service nvarchar(300),
                         BusWidth nvarchar(300), BusLength nvarchar(300), Chipset nvarchar(300), Socket nvarchar(300));

set @PassportCheck = 2; -- blue (not in rule list)

select  @ComputerGUID = ParentGUID from Action
where ObjectGUID = @InspectionGUID;

select @PassportCount = Count(*) from PassportLink 
where ParentGUID = @ComputerGUID;

if (@PassportCount > 0)
begin

insert into @TabDevice
select   HLR.Class, HLR.DeviceDesc, HLR.MinMemory, HLR.MinFreeMemory, HLR.MinFrequency,
         HLR.MinQuantity, HLR.MaxMemory,  HLR.MaxFreeMemory, HLR.MaxFrequency, HLR.MaxQuantity,
		 HLR.ReleaseDateFrom, HLR.ReleaseDateTo, HLR.Version, HLR.Service,
         HLR.BusWidth, HLR.BusLength, HLR.Chipset, HLR.Socket
from HardwareLicenseRule HLR,  HardwareLicenseRuleLink  HLRL, PassportLink PL, Passport P
where PL.ParentGUID = @ComputerGUID and HLR.ObjectGUID = HLRL.HardwareLicenseRuleGUID 
      and HLRL.ParentGUID = P.ObjectGUID and P.ObjectGUID = PL.PassportGUID;


select @DeviceCount  = Count(*) from @TabDevice
where        @Class = Class AND @DeviceDesc like DeviceDesc 

              AND ( @Class NOT IN ('MEMORY', 'DISPLAY', 'DISKDRIVE', 'PROCESSOR', 
				                     'BIOS', 'USB', 'SYSTEMSLOT', 'MOTHERBOARD')   
              
           OR 
           (
				    (  @Class = 'DISPLAY' 
					   AND (@Memory <= MaxMemory OR   MaxMemory = 0)
					   AND (@Memory >=  MinMemory OR  MinMemory = 0)
					 )    
				 OR (  @Class = 'MEMORY'  
						   AND (@Memory <=  MaxMemory OR   MaxMemory = 0)
						   AND (@Memory >=  MinMemory OR  MinMemory = 0) 
						   AND (@Quantity <=  MaxQuantity OR  MaxQuantity = 0) 
						   AND (@Quantity >=  MinQuantity OR  MinQuantity = 0) 
				    	)
				 OR (  @Class = 'PROCESSOR'  
						   AND (@Frequency <=  MaxFrequency OR  MaxFrequency = 0) 
						   AND (@Frequency >=  MinFrequency OR  MinFrequency = 0)
						   AND (@Quantity <=  MaxQuantity OR  MaxQuantity = 0) 
						   AND (@Quantity >=  MinQuantity OR  MinQuantity = 0)
                        )
				 OR (  @Class = 'DISKDRIVE'  
						   AND (@Memory <=  MaxMemory OR  MaxMemory = 0) 
						   AND (@Memory >=  MinMemory OR  MinMemory = 0)
						   AND (@FreeMemory <=  MaxFreeMemory OR  MaxFreeMemory = 0) 
						   AND (@FreeMemory >=  MinFreeMemory OR  MinFreeMemory = 0)
					    ) 
			     OR (  @Class = 'BIOS' 
				           AND (@ReleaseDate <= ReleaseDateTo OR ReleaseDateTo IS NULL) 
						   AND (@ReleaseDate >= ReleaseDateFrom OR ReleaseDateFrom IS NULL) 
						   AND (@Version LIKE Version  or Version = '%%' and @Version is NULL)
					    ) 
			     OR (  @Class = 'USB'  
				           AND (@Service LIKE Service  or  Service = '%%' and  @Service is NULL)
					  ) 
				 OR (  @Class = 'SYSTEMSLOT'  
				           AND (@BusWidth LIKE BusWidth or BusWidth = '%%' and   @BusWidth is NULL) 
						   AND (@BusLength LIKE BusLength or BusLength = '%%' and   @BusLength is NULL)
					  ) 
				 OR (  @Class = 'MOTHERBOARD'  
				           AND (@Chipset LIKE Chipset or Chipset = '%%' and   @Chipset is NULL) 
						   AND (@Version LIKE Version or Version = '%%' and   @Version is NULL) 
						   AND (@Socket LIKE Socket or Socket = '%%' and   @Socket is NULL)
					)
		   ) );
			
if (@DeviceCount > 0)        --  green (in rule list)
    set @PassportCheck = 1;
else
begin

			select @DeviceCount  = Count(*) from @TabDevice
			where  @Class =  Class   AND   @DeviceDesc like DeviceDesc  AND
					   ( 
								(  @Class = 'DISPLAY'
								   AND (    (@Memory > MaxMemory AND   MaxMemory <> 0)
								         OR (@Memory <  MinMemory AND  MinMemory <> 0) 
								       )
								)    
							 OR (  @Class = 'MEMORY'  
								   AND (    (@Memory >  MaxMemory AND   MaxMemory <> 0)
									     OR (@Memory <  MinMemory AND  MinMemory <> 0) 
									     OR (@Quantity >  MaxQuantity AND  MaxQuantity <> 0) 
									     OR (@Quantity <  MinQuantity AND  MinQuantity <> 0) 
									   )
								)
							 OR (  @Class = 'PROCESSOR' 
								   AND (    (@Frequency >  MaxFrequency AND  MaxFrequency <> 0) 
										 OR (@Frequency <  MinFrequency AND  MinFrequency <> 0)
										 OR (@Quantity >  MaxQuantity AND  MaxQuantity <> 0) 
										 OR (@Quantity <  MinQuantity AND  MinQuantity <> 0)
									   )
						        )
							 OR (  @Class = 'DISKDRIVE'
								   AND (    (@Memory >  MaxMemory AND  MaxMemory <> 0) 
										 OR (@Memory <  MinMemory AND  MinMemory <> 0)
										 OR (@FreeMemory >  MaxFreeMemory AND  MaxFreeMemory <> 0) 
										 OR (@FreeMemory <  MinFreeMemory AND  MinFreeMemory <> 0)
									   )
								)
							OR (  @Class = 'BIOS' 
								   AND  (  (@ReleaseDate > ReleaseDateTo AND ReleaseDateTo IS NOT NULL) 
										   OR (@ReleaseDate < ReleaseDateFrom AND ReleaseDateFrom IS NOT NULL) 
										   OR (@Version not like Version)
								        )  
								) 
						    OR (  @Class = 'USB'   AND (@Service not like Service)  )
							 
							OR (  @Class = 'SYSTEMSLOT'  AND  ( @BusWidth not like BusWidth  OR  @BusLength not like BusLength ) ) 

							OR (  @Class = 'MOTHERBOARD'  
									 AND ( @Chipset not like Chipset  OR   @Version not like Version  OR  @Socket not like Socket )
								) 
						);
						
			if (@DeviceCount > 0)   -- red (in rule list, but not qualify)
			set @PassportCheck = 0;
			
end
						   

																					
end


RETURN @PassportCheck;

END


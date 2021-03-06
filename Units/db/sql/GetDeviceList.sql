CREATE FUNCTION [dbo].[GetDeviceList](@InspectionGUID nvarchar(50),
@Class nvarchar(300), @DeviceList nvarchar(3000),
@MinMemory int, @MaxMemory int,  @MinFreeMemory int, @MaxFreeMemory int,
@MinFrequency int, @MaxFrequency int, @MinQuantity int, @MaxQuantity int, 
@ReleaseDateFrom datetime, @ReleaseDateTo datetime, @VersionList nvarchar(1000), @ServiceList nvarchar(500),
@BusWidthList nvarchar(500), @BusLengthList nvarchar(500), @ChipsetList nvarchar(2000), @SocketList nvarchar(2000), @DateWithNull bit)
RETURNS  nvarchar(3000)
AS
BEGIN

declare @DeviceDescList nvarchar(1000);
declare @TabDevice Table(ID int IDENTITY(1,1), DeviceDesc nvarchar(450), ReleaseDate nvarchar(50));
declare @TabVersion Table(Version nvarchar(100) );
declare @TabService Table(Service nvarchar(450) );
declare @TabBusWidth Table(BusWidth nvarchar(100) );
declare @TabBusLength Table(BusLength nvarchar(100) );
declare @TabChipset Table(Chipset nvarchar(450) );
declare @TabSocket Table(Socket nvarchar(450) );
declare @TabDeviceDesc Table(DeviceDesc nvarchar(450) );
declare @Count int;
declare @I int;
declare @L int;
declare @N int;
declare @K int;
declare @Device    nvarchar(1000);
declare @Version nvarchar(100);
declare @Service nvarchar(100);
declare @BusWidth nvarchar(100);
declare @BusLength nvarchar(100); 
declare @Chipset nvarchar(500); 
declare @Socket nvarchar(500);

set @DeviceDescList = '';

--  Fill table @TabDeviceDesc from string whith parameter DeviceList values

           if(@DeviceList = '*')
           begin
                   insert  into @TabDeviceDesc
                   select distinct DeviceDesc from Hardware  where Class = @Class;
           end
           else
           begin  

				   set @N = CHARINDEX (';', @DeviceList);

				   while(@N > 0)
				   begin
						set @Device = Left(@DeviceList, @N-1);
						set @Device = LTRIM(@Device);
						set @Device = RTRIM(@Device);
						insert into @TabDeviceDesc values(@Device);

						set @L = LEN(@DeviceList);
						set @DeviceList = RIGHT(@DeviceList, @L - @N);  
						set @N = CHARINDEX (';', @DeviceList);            
				   end

				   set @DeviceList = LTRIM(@DeviceList);
				   set @DeviceList = RTRIM(@DeviceList);
				   insert into @TabDeviceDesc values(@DeviceList);

           end


---------------------------------------------------------------------------------------

--  Fill table @TabVersion from string whith parameter VersionList values

           if(@VersionList = '*')
           begin
                   insert  into @TabVersion
                   select distinct Version from Hardware  where Class = @Class;
           end
           else
           begin 
				   set @N = CHARINDEX (';', @VersionList);

				   while(@N > 0)
				   begin
						set @Version = Left(@VersionList, @N-1);
						set @Version = LTRIM(@Version);
						set @Version = RTRIM(@Version);
						insert into @TabVersion values(@Version);

						set @L = LEN(@VersionList);
						set @VersionList = RIGHT(@VersionList, @L - @N);  
						set @N = CHARINDEX (';', @VersionList);            
				   end

				   set @VersionList = LTRIM(@VersionList);
				   set @VersionList = RTRIM(@VersionList);
				   insert into @TabVersion values(@VersionList);
           end


---------------------------------------------------------------------------------------

--  Fill table @TabService from string whith parameter ServiceList values
       
           if(@ServiceList = '*')
           begin
                   insert  into @TabService
                   select distinct Service from Hardware  where Class = @Class;
           end
           else
           begin

				   set @N = CHARINDEX (';', @ServiceList);

				   while(@N > 0)
				   begin
						set @Service = Left(@ServiceList, @N-1);
						set @Service = LTRIM(@Service);
						set @Service = RTRIM(@Service);
						insert into @TabService values(@Service);

						set @L = LEN(@ServiceList);
						set @ServiceList = RIGHT(@ServiceList, @L - @N);  
						set @N = CHARINDEX (';', @ServiceList);            
				   end

				   set @ServiceList = LTRIM(@ServiceList);
				   set @ServiceList = RTRIM(@ServiceList);
				   insert into @TabService values(@ServiceList);
           end


---------------------------------------------------------------------------------------

--  Fill table @TabSocketDesc from string whith parameter SocketList values

           if(@SocketList = '*')
           begin
                   insert  into @TabSocket
                   select distinct Socket from Hardware  where Class = @Class;
           end
           else
           begin

				   set @N = CHARINDEX (';', @SocketList);

				   while(@N > 0)
				   begin
						set @Socket = Left(@SocketList, @N-1);
						set @Socket = LTRIM(@Socket);
						set @Socket = RTRIM(@Socket);
						insert into @TabSocket values(@Socket);

						set @L = LEN(@SocketList);
						set @SocketList = RIGHT(@SocketList, @L - @N);  
						set @N = CHARINDEX (';', @SocketList);            
				   end

				   set @SocketList = LTRIM(@SocketList);
				   set @SocketList = RTRIM(@SocketList);
				   insert into @TabSocket values(@SocketList);

           end


---------------------------------------------------------------------------------------

--  Fill table @TabBusWidthDesc from string whith parameter BusWidthList values

           if(@BusWidthList = '*')
           begin
                   insert  into @TabBusWidth
                   select distinct BusWidth from Hardware  where Class = @Class;
           end
           else
           begin

				   set @N = CHARINDEX (';', @BusWidthList);

				   while(@N > 0)
				   begin
						set @BusWidth = Left(@BusWidthList, @N-1);
						set @BusWidth = LTRIM(@BusWidth);
						set @BusWidth = RTRIM(@BusWidth);
						insert into @TabBusWidth values(@BusWidth);

						set @L = LEN(@BusWidthList);
						set @BusWidthList = RIGHT(@BusWidthList, @L - @N);  
						set @N = CHARINDEX (';', @BusWidthList);            
				   end

				   set @BusWidthList = LTRIM(@BusWidthList);
				   set @BusWidthList = RTRIM(@BusWidthList);
				   insert into @TabBusWidth values(@BusWidthList);
           end

---------------------------------------------------------------------------------------

--  Fill table @TabBusLengthDesc from string whith parameter BusLengthList values

           
           if(@BusLengthList = '*')
           begin
                   insert  into @TabBusLength
                   select distinct BusLength from Hardware  where Class = @Class;
           end
           else
           begin 

				   set @N = CHARINDEX (';', @BusLengthList);

				   while(@N > 0)
				   begin
						set @BusLength = Left(@BusLengthList, @N-1);
						set @BusLength = LTRIM(@BusLength);
						set @BusLength = RTRIM(@BusLength);
						insert into @TabBusLength values(@BusLength);

						set @L = LEN(@BusLengthList);
						set @BusLengthList = RIGHT(@BusLengthList, @L - @N);  
						set @N = CHARINDEX (';', @BusLengthList);            
				   end

				   set @BusLengthList = LTRIM(@BusLengthList);
				   set @BusLengthList = RTRIM(@BusLengthList);
				   insert into @TabBusLength values(@BusLengthList);
           end

---------------------------------------------------------------------------------------

--  Fill table @TabChipset from string whith parameter ChipsetList values

           if(@ChipsetList = '*')
           begin
                   insert  into @TabChipset
                   select distinct Chipset from Hardware  where Class = @Class;
           end
           else
           begin 

				   set @N = CHARINDEX (';', @ChipsetList);

				   while(@N > 0)
				   begin
						set @Chipset = Left(@ChipsetList, @N-1);
						set @Chipset = LTRIM(@Chipset);
						set @Chipset = RTRIM(@Chipset);
						insert into @TabChipset values(@Chipset);

						set @L = LEN(@ChipsetList);
						set @ChipsetList = RIGHT(@ChipsetList, @L - @N);  
						set @N = CHARINDEX (';', @ChipsetList);            
				   end

				   set @ChipsetList = LTRIM(@ChipsetList);
				   set @ChipsetList = RTRIM(@ChipsetList);
				   insert into @TabChipset values(@ChipsetList);
           end


---------------------------------------------------------------------------------------

insert  into @TabDevice
select distinct  dbo.GetExtendDeviceDesc(H.ObjectGuid), convert(nvarchar(50), HI.ReleaseDate, 104) as ReleaseDate
FROM     Hardware  H, HardInstallation HI 
WHERE  H.ObjectGUID = HI.HardwareGUID and HI.ParentGUID = @InspectionGUID
       AND (H.Class = @Class) 
       AND (H.DeviceDesc IN (select DeviceDesc from @TabDeviceDesc)) 
       AND  (H.IsVisible = 1)
       AND 
           (  
				   (H.Class NOT IN ('MEMORY', 'DISPLAY', 'DISKDRIVE', 'PROCESSOR', 'BIOS', 'USB', 'SYSTEMSLOT', 'MOTHERBOARD') )  
				   OR
				   (     (H.Class = 'DISPLAY') AND   
						       ( (H.Memory <= @MaxMemory) OR @MaxMemory is NULL)  AND  ((H.Memory >= @MinMemory) OR  @MinMemory is NULL)
				   ) 
				   OR
				   (  (H.Class = 'Memory') AND     
							   ( (H.Memory <= @MaxMemory) OR @MaxMemory is NULL)  AND  ((H.Memory >= @MinMemory) OR  @MinMemory is NULL)
								   AND ((H.Quantity <= @MaxQuantity) OR @MaxQuantity is NULL )   AND     ( (H.Quantity >= @MinQuantity)  OR @MinQuantity is NULL )
				   )
				   OR
				   (  (H.Class = 'PROCESSOR') AND     
							   ( (H.Frequency <= @MaxFrequency)  OR @MaxFrequency is NULL )  AND  ((H.Frequency >= @MinFrequency)  OR @MinFrequency is NULL)
								   AND ((H.Quantity <= @MaxQuantity) OR @MaxQuantity is NULL )   AND     ( (H.Quantity >= @MinQuantity)  OR @MinQuantity is NULL )
				   )  
				   OR
				   (    (H.Class = 'DISKDRIVE')  AND
							 (  (H.Memory <= @MaxMemory)  OR @MaxMemory is NULL )  AND  ((H.Memory >= @MinMemory)  OR @MinMemory is NULL)
							AND  ((HI.FreeMemory <= @MaxFreeMemory)  OR @MaxFreeMemory is NULL ) AND ( (HI.FreeMemory >= @MinFreeMemory)  OR ( @MinFreeMemory is NULL )) 
					) 
                   OR
				   (    (H.Class = 'BIOS') AND 
                             (  ( (HI.ReleaseDate <= @ReleaseDateTo OR @ReleaseDateTo IS NULL) AND (HI.ReleaseDate >= @ReleaseDateFrom OR  @ReleaseDateFrom IS NULL)
                                  OR ( HI.ReleaseDate IS NULL AND @DateWithNull = 1) )
                               AND H.Version IN (select Version from @TabVersion) )
                    )                               
				   OR
                   (     H.Class = 'USB' AND H.Service IN (select Service from @TabService) )
				   OR
                   (     H.Class = 'SYSTEMSLOT' AND H.BusWidth IN (select BusWidth from @TabBusWidth) AND H.BusLength IN (select BusLength from @TabBusLength) )                    
				   OR
                   (     H.Class = 'MOTHERBOARD' AND H.Chipset IN (select Chipset from @TabChipset) AND H.Version IN (select Version from @TabVersion) AND H.Socket IN (select Socket from @TabSocket) )
     
			)
ORDER BY  dbo.GetExtendDeviceDesc(H.ObjectGuid), ReleaseDate;

update @TabDevice
set DeviceDesc = DeviceDesc + ', дата выпуска: ' + ReleaseDate
where ReleaseDate is not null;

select @Count = Count(*) from @TabDevice;

set @I = 1;

while (@I < = @Count)
 begin
               select @Device = DeviceDesc from @TabDevice where ID = @I;
               set @DeviceDescList = @DeviceDescList + @Device + '; ' ;
               set @I = @I + 1; 
end

   -------------  deleting last  ', '  -----------------------------
set @DeviceDescList = RTRIM(@DeviceDescList);
set @L = LEN(@DeviceDescList); 
if (@L >0)
begin 
      set @DeviceDescList = LEFT(@DeviceDescList, @L - 1);
end

RETURN @DeviceDescList;

END
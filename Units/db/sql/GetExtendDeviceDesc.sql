CREATE FUNCTION [dbo].[GetExtendDeviceDesc](@ObjectGUID nvarchar(50))
RETURNS  nvarchar(1000)
AS
BEGIN

declare @Device    nvarchar(300);
declare @Class    nvarchar(450);
declare @Memory    nvarchar(300);
declare @Frequency    nvarchar(300);
declare @Quantity    nvarchar(300);
declare @ExtendDeviceDesc   nvarchar(1000);
declare @Version nvarchar(500);
declare @WakeupType nvarchar(500);
declare @BusWidth nvarchar(500);
declare @BusLength nvarchar(500);
declare @Socket nvarchar(500);
declare @Service nvarchar(100);
declare @Chipset nvarchar(450);


set @ExtendDeviceDesc = '';

select @Class = Class,  @Device = DeviceDesc, @Memory = Memory, @Frequency = Frequency, @Quantity = Quantity,
       @Version = Version, @WakeupType = WakeupType, @BusWidth = BusWidth, @BusLength = BusLength, @Socket = Socket,
	   @Service = Service, @Chipset = Chipset
from Hardware where ObjectGuid = @ObjectGUID;

set @ExtendDeviceDesc =  @Device;

if (@Class <> 'Memory')  -- Class Memory already include this attribute
begin
         if(@Memory is not NULL)
               set @ExtendDeviceDesc = @ExtendDeviceDesc + ', ' + @Memory + ' Mb'; 		
end

if(@Frequency is not NULL)
      set @ExtendDeviceDesc = @ExtendDeviceDesc + ', ' + @Frequency + ' MHz';
      
if( (@Quantity is not NULL)  and (@Class = 'Processor') )
      set @ExtendDeviceDesc = @ExtendDeviceDesc + ', Кол-во процессоров: ' +  @Quantity;

if(@Version is not NULL  and @Version <> '')
      set @ExtendDeviceDesc = @ExtendDeviceDesc + ', версия: ' + @Version;

if(@WakeupType is not NULL)
      set @ExtendDeviceDesc = @ExtendDeviceDesc + ', тип Wakeup: ' + @WakeupType;

if(@BusWidth is not NULL and @BusWidth <> '')
      set @ExtendDeviceDesc = @ExtendDeviceDesc + ', ширина: ' + @BusWidth;

if(@BusLength is not NULL and @BusLength <> '')
      set @ExtendDeviceDesc = @ExtendDeviceDesc + ', длина: ' + @BusLength;

if(@Socket is not NULL and @Socket <> '')
      set @ExtendDeviceDesc = @ExtendDeviceDesc + ', сокет: ' + @Socket;

if(@Service is not NULL and @Service <> '')
      set @ExtendDeviceDesc = @ExtendDeviceDesc + ', тип: ' + @Service;

if(@Chipset is not NULL and @Chipset <> '')
      set @ExtendDeviceDesc = @ExtendDeviceDesc + ', чипсет: ' + @Chipset;



RETURN @ExtendDeviceDesc;

END

CREATE PROCEDURE [dbo].[GetRepComputersWithHardwareParams] 
(@ObjectGUIDList nvarchar(max))

AS
BEGIN

SET NOCOUNT ON;
	
declare @Message     nvarchar(1000);
declare @GUID     nvarchar(100);
declare @N int;
declare @L int;
declare @Min int;
declare @Max int;
declare @I int;
declare @Parameter nvarchar(500);

declare @ClassList nvarchar(max); 
declare @DeviceDescList nvarchar(max); 


declare @TabGUID Table(SoftGUID nvarchar(100));
declare @TabParameter Table(ID int IDENTITY(1,1), ParameterValue nvarchar(3000)  collate Cyrillic_General_CI_AS );

BEGIN TRY

--------------------------   getting definite soft by Object GUID List  ---------------------------------------------------------------------

set @ObjectGUIDList = replace(@ObjectGUIDList, '{', '');
set @ObjectGUIDList = replace(@ObjectGUIDList, '}', '');

set @N = CHARINDEX (',', @ObjectGUIDList);

           while(@N > 0)
           begin
                set @GUID = Left(@ObjectGUIDList, @N-1);
                set @GUID = LTRIM(@GUID);
                set @GUID = RTRIM(@GUID);
                insert into @TabGUID values(@GUID);

                set @L = LEN(@ObjectGUIDList);
                set @ObjectGUIDList = RIGHT(@ObjectGUIDList, @L - @N);  
                set @N = CHARINDEX (',', @ObjectGUIDList);            
           end

           set @ObjectGUIDList = LTRIM(@ObjectGUIDList);
           set @ObjectGUIDList = RTRIM(@ObjectGUIDList);
           insert into @TabGUID values(@ObjectGUIDList);

select Class, dbo.GetExtendDeviceDesc(ObjectGUID) as ExtDeviceDesc
into #ReportParams from Hardware
where ObjectGUID in (select SoftGUID from @TabGUID);

--------------------------  Report parameter forming -------------------------------------------------------------

------------------------------------------------------  ClassList  -----------------------------------------------

delete from @TabParameter;

 insert into @TabParameter
SELECT DISTINCT Class from #ReportParams;

    select @Min = min(ID) from @TabParameter;
    select @Max = max(ID) from @TabParameter;

    set @I = @Min;
    set @ClassList = '';

    while (@I < = @Max)
    begin
               select @Parameter = ParameterValue from @TabParameter where ID = @I;
               set @ClassList = @ClassList +@Parameter  + ';';
               set @I = @I + 1; 
    end
     
----------------  deleting last  ', '  -----------------------------

  set @L = LEN(@ClassList);
  if(@L >0)
  begin
        set @ClassList = LEFT(@ClassList, @L - 1);
  end
     
------------------------------- DeviceDescList ---------------------

delete from @TabParameter;

 insert into @TabParameter
SELECT DISTINCT ExtDeviceDesc from #ReportParams;

    select @Min = min(ID) from @TabParameter;
    select @Max = max(ID) from @TabParameter;

    set @I = @Min;
    set @DeviceDescList = '';

    while (@I < = @Max)
    begin
               select @Parameter = ParameterValue from @TabParameter where ID = @I;
               set @DeviceDescList = @DeviceDescList +@Parameter  + ';';
               set @I = @I + 1; 
    end
     
----------------  deleting last  ', '  -----------------------------

  set @L = LEN(@DeviceDescList);
  if(@L >0)
  begin
        set @DeviceDescList = LEFT(@DeviceDescList, @L - 1);
  end


select @ClassList as Class, @DeviceDescList as DeviceDesc;
           
END TRY
BEGIN CATCH

    if (Error_Message()  is NULL or Error_Message() = '')
	   set @Message = 'БД: ошибка запроса параметров отчета';
	else
	   set @Message = Error_Message();
   
    RAISERROR(@Message, 12, 1);
    Return -1;

END CATCH

END

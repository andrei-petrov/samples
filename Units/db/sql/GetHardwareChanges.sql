CREATE PROCEDURE [dbo].[GetHardwareChanges] 
(@ComputerName nvarchar(450), @UTCInitInspection datetime, @UTCEndInspection datetime)

AS
BEGIN

	SET NOCOUNT ON;
	
declare @Message     nvarchar(500);
declare @Class         nvarchar(450);
declare @DeviceDesc   nvarchar(300);
declare @Memory   int;
declare @I int;
declare @J int;
declare @MaxID int;
declare @Count int;

SET DATEFORMAT ymd;

BEGIN TRY

--  there are 4 initial tables    in order to don't select dataset twice from db, for speed

CREATE TABLE #InitHardware(
	                      [ID] [int] IDENTITY(1,1) NOT NULL,
	                     [Class] [nvarchar](450) collate Cyrillic_General_CI_AS  NULL,
	                     [DeviceDesc] [nvarchar](450) collate Cyrillic_General_CI_AS  NULL,
                         [Memory] [int] NULL,
                     ) ON [PRIMARY]; 

CREATE TABLE #EndHardware(
	                      [ID] [int] IDENTITY(1,1) NOT NULL,
	                     [Class] [nvarchar](450) collate Cyrillic_General_CI_AS  NULL,
	                     [DeviceDesc] [nvarchar](450) collate Cyrillic_General_CI_AS  NULL,
                         [Memory] [int] NULL,
                     ) ON [PRIMARY]; 

CREATE TABLE #InitHardwareAdd(
	                      [ID] [int] IDENTITY(1,1) NOT NULL,
	                     [Class] [nvarchar](450) collate Cyrillic_General_CI_AS  NULL,
	                     [DeviceDesc] [nvarchar](450)  collate Cyrillic_General_CI_AS NULL,
                         [Memory] [int] NULL, 
                     ) ON [PRIMARY]; 

CREATE TABLE #EndHardwareAdd(
	                      [ID] [int] IDENTITY(1,1) NOT NULL,
	                     [Class] [nvarchar](450) collate Cyrillic_General_CI_AS  NULL,
	                     [DeviceDesc] [nvarchar](450)  collate Cyrillic_General_CI_AS NULL,
                         [Memory] [int] NULL,
                     ) ON [PRIMARY]; 

CREATE TABLE #HardwareChanges(
	                      [ChangeType] [nvarchar](50) collate Cyrillic_General_CI_AS  NULL,
	                     [Class] [nvarchar](450) collate Cyrillic_General_CI_AS  NULL,
	                     [DeviceDesc] [nvarchar](450)  collate Cyrillic_General_CI_AS NULL,
                     ) ON [PRIMARY]; 

INSERT    into #InitHardware
SELECT H.Class, H.DeviceDesc, H.Memory
FROM     HardInstallation AS HI INNER JOIN
               Action AS A ON HI.ParentGUID = A.ObjectGUID INNER JOIN
               Hardware AS H ON HI.HardwareGUID = H.ObjectGUID INNER JOIN
               Computer AS C ON A.ParentGUID = C.ObjectGUID
WHERE  (C.Name = @ComputerName) AND (A.ActionDate = @UTCInitInspection)   AND (H.IsVisible = 1)

INSERT  into #EndHardware
SELECT   H.Class, H.DeviceDesc, H.Memory  
FROM     HardInstallation AS HI INNER JOIN
               Action AS A ON HI.ParentGUID = A.ObjectGUID INNER JOIN
               Hardware AS H ON HI.HardwareGUID = H.ObjectGUID INNER JOIN
               Computer AS C ON A.ParentGUID = C.ObjectGUID
WHERE  (C.Name = @ComputerName) AND (A.ActionDate = @UTCEndInspection)   AND (H.IsVisible = 1)

INSERT    into #InitHardwareAdd
SELECT Class, DeviceDesc, Memory
FROM     #InitHardware

INSERT    into #EndHardwareAdd
SELECT Class, DeviceDesc, Memory
FROM     #EndHardware


---------------------------------------------------------------------------------------------------

select @MaxID = count(*) from #InitHardware;

set @I = 1;
        
while (@I <= @MaxID)
begin

        set @J = -1;

        select  @Class  = Class, @DeviceDesc =  DeviceDesc, @Memory = Memory
        from   #InitHardware       where ID = @I; 

        select @J = ID from  #EndHardware
        where (Class  = @Class and  DeviceDesc =  @DeviceDesc  and Class <> 'MEMORY')
              OR  (Class  = @Class and  Memory =  @Memory  and Class = 'MEMORY');

        if (@J <> -1)
        begin
                delete from #EndHardware where ID = @J;
                delete from #InitHardware where ID = @I;
        end

         set @I = @I+1;    
end

select @Count = count(*) from #InitHardware;

 if (@Count  > 0)
begin
         INSERT  into #HardwareChanges
         SELECT   'Удаленное оборудование' as   ChangeType,  Class, DeviceDesc
         FROM #InitHardware
end

---------------------------------------------------------------------------------------------------

select @MaxID = count(*) from #EndHardwareAdd;

set @I = 1;
        
while (@I <= @MaxID)
begin

        set @J = -1;

        select  @Class  = Class, @DeviceDesc =  DeviceDesc, @Memory = Memory
        from   #EndHardwareAdd     where ID = @I; 

        select @J = ID from  #InitHardwareAdd
        where (Class  = @Class and  DeviceDesc =  @DeviceDesc  and Class <> 'MEMORY')
              OR  (Class  = @Class and  Memory =  @Memory  and Class = 'MEMORY');

        if (@J <> -1)
        begin
                delete from #InitHardwareAdd where ID = @J;
                delete from #EndHardwareAdd where ID = @I;
        end

         set @I = @I+1;    
end

select @Count = count(*) from #EndHardwareAdd;

 if (@Count  > 0)
begin
         INSERT  into #HardwareChanges
         SELECT   'Добавленное оборудование' as   ChangeType,  Class, DeviceDesc FROM #EndHardwareAdd;
end


select  ChangeType,  Class,  DeviceDesc,   Count(DeviceDesc)  as HardwareCount
from #HardwareChanges
group by ChangeType,  Class, DeviceDesc

END TRY
BEGIN CATCH

    Set @Message = Error_Message();
    RAISERROR(@Message, 12, 1);
    Return -1;

END CATCH

END

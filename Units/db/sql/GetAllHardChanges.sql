CREATE  PROCEDURE [dbo].[GetAllHardChanges] 
(@SubdivisionName nvarchar(450), @UTCInitTime datetime, @UTCEndTime datetime)
AS
BEGIN

	SET NOCOUNT ON;
	
declare @Message     nvarchar(500);
declare @Class         nvarchar(450);
declare @DeviceDesc   nvarchar(450);
declare @ComputerGUID   nvarchar(100);
declare @ComputerName   nvarchar(450);
declare @Name   nvarchar(450);
declare @I int;
declare @J int;
declare @K_Max int;
declare @K int;
declare @N int;
declare @L int;
declare @Memory int;
declare @MaxID int;
declare @Count int;
declare @InspectionCount int;
declare @InitActionDate datetime;
declare @LastActionDate datetime;

SET DATEFORMAT ymd;

BEGIN TRY


CREATE TABLE #ComputerName(
	                      [ID] [int] IDENTITY(1,1) NOT NULL,
	                      [ComputerGUID] [nvarchar](50)  collate Cyrillic_General_CI_AS  NULL,
                          [ComputerName] [nvarchar](450)  collate Cyrillic_General_CI_AS  NULL
                     ) ON [PRIMARY]; 

CREATE TABLE #HardwareChanges(
	                      [ChangeType] [nvarchar](50) collate Cyrillic_General_CI_AS NULL,
	                     [Class] [nvarchar](450) collate Cyrillic_General_CI_AS  NULL,
	                     [DeviceDesc] [nvarchar](450) collate Cyrillic_General_CI_AS  NULL,
	                     [CompName] [nvarchar](450) collate Cyrillic_General_CI_AS  NULL,
                     ) ON [PRIMARY]; 

CREATE TABLE #ActionDate(
	                    [ActionDate] [datetime] NULL
                     ) ON [PRIMARY]; 

--  there are 4 initial tables    in order to don't select dataset twice from db, for speed

CREATE TABLE #InitHardware(
	                      [ID] [int] IDENTITY(1,1) NOT NULL,
	                     [Class] [nvarchar](450) collate Cyrillic_General_CI_AS  NULL,
	                     [DeviceDesc] [nvarchar](450)  collate Cyrillic_General_CI_AS NULL,
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
	                     [Class] [nvarchar](450)  collate Cyrillic_General_CI_AS NULL,
	                     [DeviceDesc] [nvarchar](450) collate Cyrillic_General_CI_AS  NULL,
                         [Memory] [int] NULL,
                     ) ON [PRIMARY]; 

CREATE TABLE #EndHardwareAdd(
	                      [ID] [int] IDENTITY(1,1) NOT NULL,
	                     [Class] [nvarchar](450)  collate Cyrillic_General_CI_AS NULL,
	                     [DeviceDesc] [nvarchar](450) collate Cyrillic_General_CI_AS  NULL,
                         [Memory] [int] NULL,
                     ) ON [PRIMARY]; 


insert into #ComputerName 
select distinct C.ObjectGUID, C.Name
from  Action A 
inner join Computer C on C.ObjectGUID = A.ParentGUID
left join Subdivision S on C.SubdivisionGUID = S.ObjectGUID
WHERE  A.ActionType = 1 AND A.ResultCode = 1 
AND  ( (S.Name = @SubdivisionName) OR (@SubdivisionName = 'Все подразделения')
       OR (C.SubdivisionGUID IS NULL AND @SubdivisionName = 'Компьютеры вне подразделений')); 

select @K_Max = count(*) from   #ComputerName;

set @K = 1;
        
while (@K <= @K_Max)
begin

               select @Name  = ComputerName, @ComputerGUID = ComputerGUID  from #ComputerName
               where ID = @K;

---------------------------    calculating   last inspection  but one   ----------------------------------------------------

               insert into #ActionDate 
               select distinct   A.ActionDate  
               from   Action  A
               where  A.ParentGUID =  @ComputerGUID  and  (A.ActionType = 1) AND (A.ResultCode = 1)
                              and A.ActionDate  between  @UTCInitTime and  @UTCEndTime;

               select @InspectionCount = count(*) from #ActionDate;

               if(@InspectionCount > 1)
               begin  

							   select @LastActionDate = max(ActionDate) from #ActionDate;
                               select @InitActionDate = min(ActionDate) from #ActionDate;

-----------------------------------------------------------------------------------------------------------------------

                              INSERT    into #InitHardware
                              SELECT H.Class, H.DeviceDesc, H.Memory
                              FROM     HardInstallation HI,   Action  A,  Hardware  H,   Computer C
                              WHERE  HI.ParentGUID = A.ObjectGUID and  HI.HardwareGUID = H.ObjectGUID and  A.ParentGUID = C.ObjectGUID
                                              and   (C.Name = @Name) AND (A.ActionDate = @InitActionDate)   AND (H.IsVisible = 1);

                              INSERT  into #EndHardware
                              SELECT   H.Class, H.DeviceDesc, H.Memory   
                              FROM     HardInstallation HI,   Action  A,   Hardware  H, Computer C  
                              WHERE  (C.Name = @Name) AND  A.ParentGUID = C.ObjectGUID and  HI.HardwareGUID = H.ObjectGUID
                                              and   HI.ParentGUID = A.ObjectGUID    and   (A.ActionDate = @LastActionDate)   AND (H.IsVisible = 1);
                                       
   
                              INSERT    into #InitHardwareAdd
                              SELECT Class, DeviceDesc, Memory
                              FROM     #InitHardware
               
                              INSERT    into #EndHardwareAdd
                              SELECT Class, DeviceDesc, Memory
                              FROM     #EndHardware


----------------------------------------------------------------------------------------------------------------------------------------------

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
                                       SELECT   'Удаленное оборудование' as   ChangeType,  Class, DeviceDesc,  @Name
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
                                       SELECT   'Добавленное оборудование' as   ChangeType,  Class, DeviceDesc,  @Name   
                                       FROM #EndHardwareAdd;
                              end

                              delete from  #InitHardware;
                              delete from  #EndHardware;
                              delete from  #InitHardwareAdd;
                              delete from  #EndHardwareAdd;

                              DBCC CHECKIDENT (#InitHardware, RESEED, 0);
                              DBCC CHECKIDENT (#EndHardware, RESEED, 0);
                              DBCC CHECKIDENT (#InitHardwareAdd, RESEED, 0);
                              DBCC CHECKIDENT (#EndHardwareAdd, RESEED, 0);
            
               end
               
               delete from #ActionDate;
               set @K = @K + 1;

end

-----------------------------------------------------------------------------------------------------------------------------------
-----------------------------------------------------------------------------------------------------------------------------------

select CompName, ChangeType,  Class,  DeviceDesc,   Count(DeviceDesc)  as HardwareCount
from #HardwareChanges
group by CompName, ChangeType,  Class, DeviceDesc
order by CompName


END TRY
BEGIN CATCH

    Set @Message = Error_Message();
    RAISERROR(@Message, 12, 1);
    Return -1;

END CATCH

END



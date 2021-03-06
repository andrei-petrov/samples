CREATE PROCEDURE [dbo].[UpdateDBOnNewSynonyms] 
AS
BEGIN
	
SET NOCOUNT ON;

SET DATEFORMAT ymd;

declare @ObjectCount int;
declare @I int;
declare @ObjectGUID  nvarchar(50);
declare @Check  int;
declare @Message  nvarchar(1000);

declare @SoftTable  table(
	                      [ID] [int] IDENTITY(1,1) NOT NULL,
	                     [Publisher] [nvarchar](450) collate Cyrillic_General_CI_AS NULL,
	                     [VersionMinor] [nvarchar](20) collate Cyrillic_General_CI_AS NULL,
	                     [VersionMajor] [nvarchar](20) collate Cyrillic_General_CI_AS NULL,
	                     [Language] [nvarchar](450) collate Cyrillic_General_CI_AS NULL,
	                     [ProductName] [nvarchar](450) collate Cyrillic_General_CI_AS NULL,
	                     [IsOperatingSystem] [smallint] NULL,
	                     [ProductKey] [nvarchar](50) collate Cyrillic_General_CI_AS NULL
                     ); 

declare @HardwareTable  table(
	                      [ID] [int] IDENTITY(1,1) NOT NULL,
	                      [Class] [nvarchar](450) collate Cyrillic_General_CI_AS NULL,
	                      [DeviceDesc] [nvarchar](450) collate Cyrillic_General_CI_AS  NULL,
                          [Memory] [int] NULL,
	                      [Frequency] [int] NULL,
	                      [Quantity] [int] NULL,
	                      [Speed] [int] NULL
                     );

declare @GUIDs table( ObjGuid uniqueidentifier NOT NULL  ); 

BEGIN TRY

BEGIN TRANSACTION

set  @Check = 0;

---    update Soft and  Hardware

/*Update Soft
set ProductName = DisplayName from Synonym Syn
where ProductName like Syn.Template and Syn.AttributeName = 'ProductName';
if (@@Error <> 0)   set @Check = -1;

Update Soft
set Publisher = DisplayName from Synonym Syn
where Publisher  like Syn.Template and Syn.AttributeName = 'Publisher';
if (@@Error <> 0)   set @Check = -1;

Update Hardware
set DeviceDesc = DisplayName from Synonym Syn
where DeviceDesc  like Syn.Template and Syn.AttributeName = 'DeviceDesc';
if (@@Error <> 0)   set @Check = -1;*/

update Soft
set ProductName = dbo.GetNameFromTemplate(ProductName, 'ProductName');
if (@@Error <> 0)   set @Check = -1;

update Soft
set Publisher = dbo.GetNameFromTemplate(Publisher, 'Publisher');
if (@@Error <> 0)   set @Check = -1;

update Hardware
set DeviceDesc = dbo.GetNameFromTemplate(DeviceDesc, 'DeviceDesc');
if (@@Error <> 0)   set @Check = -1;

---    Check for  not unique Soft  after update
  
insert into @SoftTable
select Publisher, VersionMinor, VersionMajor,  Localization,  ProductName,  IsOperatingSystem,  ProductKey 
from Soft
group by Publisher, VersionMinor, VersionMajor,  Localization,  ProductName,  IsOperatingSystem,  ProductKey
having Count(ObjectGUID) > 1;
if (@@Error <> 0)   set @Check = -1;

select  @ObjectCount =  count(*) from @SoftTable;

 set @I = 1;
 while (@I <= @ObjectCount) 
 begin
           --  найти все гуиды  подобного софта, заменить в Installation их на первый из них
           --  удалить остальные строки из Soft

           set @ObjectGUID = '';

           insert into @GUIDs
           select ObjectGUID from Soft S, @SoftTable ST
           where S.Publisher = ST.Publisher and S.VersionMinor = ST.VersionMinor 
                      and S.VersionMajor = ST.VersionMajor and S.Localization = ST.Language and S.ProductName = ST.ProductName 
                      and S.IsOperatingSystem = ST.IsOperatingSystem and S.ProductKey = ST.ProductKey 
                      and ST.ID = @I;
           if (@@Error <> 0)   set @Check = -1;

           select  @ObjectGUID =   ObjGuid from @GUIDs;

           if (@ObjectGUID <> '' and @ObjectGUID is not NULL)
           begin
                     delete from @GUIDs
                     where ObjGuid = @ObjectGUID;
                     if (@@Error <> 0)   set @Check = -1;

                     Update Installation
                     set SoftGuid = @ObjectGUID
                     where SoftGuid in (select ObjGuid from @GUIDs);
                     if (@@Error <> 0)   set @Check = -1;

                     delete from Soft
                     where ObjectGUID in (select ObjGuid from @GUIDs);
                     if (@@Error <> 0)   set @Check = -1;

            end

           delete from @GUIDs;   
           set @I = @I +1;
 end


---    Check for  not unique Hardware  after update
  
insert into @HardwareTable
select Class, DeviceDesc, Memory,   Frequency, Quantity, Speed 
from Hardware
group by Class, DeviceDesc, Memory,   Frequency, Quantity, Speed
having Count(ObjectGUID) > 1;
if (@@Error <> 0)   set @Check = -1;

select  @ObjectCount =  count(*) from @HardwareTable;

 set @I = 1;
 while (@I <= @ObjectCount) 
 begin
           --  найти все гуиды  подобного оборудования, заменить их в HardInstallation  на первый из них
           --  удалить остальные строки из Hardware

           set @ObjectGUID = '';
           delete from @GUIDs;

           insert into @GUIDs
           select ObjectGUID from Hardware H, @HardwareTable HT
           where HT.Class = H.Class  and HT.DeviceDesc = H.DeviceDesc  
                                                      and ( (HT.Memory = H.Memory) or (HT.Memory is NULL and  H.Memory is NULL) )
													  and  ( (HT.Frequency = H.Frequency) or (HT.Frequency is NULL and H.Frequency is NULL) )
                                                      and  ( (HT.Quantity = H.Quantity)  or (HT.Quantity is NULL and  H.Quantity is NULL) )    
													  and  ( (HT.Speed = H.Speed )  or (HT.Speed is NULL and  H.Speed is NULL) )    
                                                      and HT.ID = @I;
           if (@@Error <> 0)   set @Check = -1;

           select  @ObjectGUID =  ObjGuid from @GUIDs;

           if (@ObjectGUID <> '' and @ObjectGUID is not NULL)
           begin
                     delete from @GUIDs
                     where ObjGuid = @ObjectGUID;
                     if (@@Error <> 0)   set @Check = -1;

                     Update HardInstallation
                     set HardwareGuid = @ObjectGUID
                     where HardwareGuid in (select ObjGuid from @GUIDs);
                     if (@@Error <> 0)   set @Check = -1;

                     delete from Hardware
                     where ObjectGUID in (select ObjGuid from @GUIDs);
                     if (@@Error <> 0)   set @Check = -1;

            end
   
           set @I = @I +1;
 end


 if (@Check = 0)
                     COMMIT TRANSACTION
 else 
 begin 
      Set @Message = 'Транзакция отменена ';
      RAISERROR(@Message, 12, 1);
 end

END TRY
BEGIN CATCH


    if (Error_Message()  is NULL or Error_Message() = '')
	   set @Message = 'Ошибка БД при обновлении по новым синонимам';
	else
	   set @Message = Error_Message();
	   
ROLLBACK TRANSACTION
RAISERROR(@Message, 12, 1);
Return -1;

END CATCH

END








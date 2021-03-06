CREATE PROCEDURE [dbo].[GetAllSoftChanges] 
(@SubdivisionName nvarchar(450), @UTCInitTime datetime, @UTCEndTime datetime)
AS
BEGIN

	SET NOCOUNT ON;
	
declare @Message     nvarchar(500);
declare @Class         nvarchar(450);
declare @DeviceDesc   nvarchar(450);
declare @ComputerGUID   nvarchar(100);
declare @ComputerName   nvarchar(450);
declare @I int;
declare @J int;
declare @K_Max int;
declare @K int;
declare @N int;
declare @L int;
declare @MaxID int;
declare @Count int;
declare @InspectionCount int;
declare @InitActionDate datetime;
declare @LastActionDate datetime;

SET DATEFORMAT ymd;

BEGIN TRY


CREATE TABLE #Computer(
	                      [ID] [int] IDENTITY(1,1) NOT NULL,
	                     [ComputerGUID] [nvarchar](50)  collate Cyrillic_General_CI_AS  NULL,
                         [ComputerName] [nvarchar](450)  collate Cyrillic_General_CI_AS  NULL
                     ) ON [PRIMARY]; 

CREATE TABLE #SoftChanges(
	                      [ChangeType] [nvarchar](50) collate Cyrillic_General_CI_AS  NULL,
                          [CompName] [nvarchar](450)  collate Cyrillic_General_CI_AS NULL,
                          [Publisher]  [nvarchar](450) collate Cyrillic_General_CI_AS  NULL, 
                          [ReportSortOrder]  tinyint NULL,   
	                     [ProductName] [nvarchar](450) collate Cyrillic_General_CI_AS  NULL,
                          [IsOperatingSystem]  smallint NULL,   
	                     [ProductKey] [nvarchar](500) collate Cyrillic_General_CI_AS  NULL,
	                     [InspectionDate] datetime NULL,
	                     [Language] [nvarchar](450) collate Cyrillic_General_CI_AS  NULL,
	                     [SoftDescription] [nvarchar](500) collate Cyrillic_General_CI_AS  NULL,
	                     [Version] [nvarchar](500) collate Cyrillic_General_CI_AS  NULL,
                     ) ON [PRIMARY]; 

CREATE TABLE #ActionDate(
	                    [ActionDate] [datetime] NULL
                     ) ON [PRIMARY]; 


insert into #Computer
select distinct C.ObjectGUID, C.Name
from  Action A 
inner join Computer C on C.ObjectGUID = A.ParentGUID
left join Subdivision S on C.SubdivisionGUID = S.ObjectGUID
WHERE  A.ActionType = 1 AND A.ResultCode = 1 
AND  ( (S.Name = @SubdivisionName) OR (@SubdivisionName = 'Все подразделения')
       OR (C.SubdivisionGUID IS NULL AND @SubdivisionName = 'Компьютеры вне подразделений'));

select @K_Max = count(*) from   #Computer;

set @K = 1;
        
while (@K <= @K_Max)
begin

               select @ComputerName  = ComputerName, @ComputerGUID =  ComputerGUID  
               from #Computer
               where ID = @K;

---------------------------    calculating   init   and  last inspections   in definite period   if exist -------------------------------------


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
								INSERT    into #SoftChanges
								SELECT  'Установленное ПО' AS ChangeType, @ComputerName AS CompName,  P.Name as Publisher,  ST.ReportSortOrder, STP.ProductName, SF.IsOperatingSystem, 
											   SF.ProductKey, SF.InspectionDate, dbo.GetLanguage(SF.Localization) AS Language,  ST.SoftDescription, dbo.GetFullVersion(STP.VersionMajor, SF.VersionMinor, SF.VersionBuildMajor, SF.VersionBuildMinor) 
											   AS Version
								FROM     Soft SF, SoftType STP, SoftTypeLU ST, Publisher P
								WHERE  P.ObjectGUID = STP.PublisherGUID and SF.IsOperatingSystem = ST.ID   AND SF.SoftTypeGUID = STP.ObjectGUID AND (SF.IsVisible = 1) AND SF.ObjectGUID IN
												   ((SELECT  SF.ObjectGUID
													 FROM     Installation AS I INNER JOIN
																	Action AS A ON I.ParentGUID = A.ObjectGUID INNER JOIN
																	Soft AS SF ON I.SoftGUID = SF.ObjectGUID INNER JOIN
																	Computer AS C ON A.ParentGUID = C.ObjectGUID
													 WHERE  (C.Name = @ComputerName) AND (A.ActionDate = @LastActionDate))
											   EXCEPT
											   (SELECT  SF.ObjectGUID
												FROM     Installation AS I INNER JOIN
															   Action AS A ON I.ParentGUID = A.ObjectGUID INNER JOIN
															   Soft AS SF ON I.SoftGUID = SF.ObjectGUID INNER JOIN
															   Computer AS C ON A.ParentGUID = C.ObjectGUID
												WHERE  (C.Name = @ComputerName) AND (A.ActionDate = @InitActionDate)))
								UNION
								(SELECT  'Удаленное ПО' AS ChangeType, @ComputerName AS CompName, P.Name as Publisher,  ST.ReportSortOrder, STP.ProductName, SF.IsOperatingSystem, 
												SF.ProductKey,  SF.InspectionDate, dbo.GetLanguage(SF.Localization) AS Language,  ST.SoftDescription, dbo.GetFullVersion(STP.VersionMajor, SF.VersionMinor, SF.VersionBuildMajor, SF.VersionBuildMinor) 
												AS Version
								 FROM     Soft SF,  SoftType STP, SoftTypeLU ST, Publisher P
								 WHERE  P.ObjectGUID = STP.PublisherGUID and SF.IsOperatingSystem = ST.ID   AND SF.SoftTypeGUID = STP.ObjectGUID   AND (SF.IsVisible = 1) AND SF.ObjectGUID IN
													((SELECT  SF.ObjectGUID
													  FROM     Installation AS I INNER JOIN
																	 Action AS A ON I.ParentGUID = A.ObjectGUID INNER JOIN
																	 Soft AS SF ON I.SoftGUID = SF.ObjectGUID INNER JOIN
																	 Computer AS C ON A.ParentGUID = C.ObjectGUID
													  WHERE  (C.Name = @ComputerName) AND (A.ActionDate = @InitActionDate))
												EXCEPT
												(SELECT  SF.ObjectGUID
												 FROM     Installation AS I INNER JOIN
																Action AS A ON I.ParentGUID = A.ObjectGUID INNER JOIN
																Soft AS SF ON I.SoftGUID = SF.ObjectGUID INNER JOIN
																Computer AS C ON A.ParentGUID = C.ObjectGUID
												 WHERE  (C.Name = @ComputerName) AND (A.ActionDate = @LastActionDate))));

               end
               
               delete from #ActionDate;
               set @K = @K + 1;

end

-----------------------------------------------------------------------------------------------------------------------------------
-----------------------------------------------------------------------------------------------------------------------------------

select distinct *   from #SoftChanges
order by CompName;


END TRY
BEGIN CATCH

    Set @Message = Error_Message();
    RAISERROR(@Message, 12, 1);
    Return -1;

END CATCH

END
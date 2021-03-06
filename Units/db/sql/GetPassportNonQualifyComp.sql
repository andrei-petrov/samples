CREATE PROCEDURE [dbo].[GetPassportNonQualifyComp](@SubdivisionName nvarchar(450)) 

AS
BEGIN

	SET NOCOUNT ON;
	
declare @Message     nvarchar(3000);
declare @Class         nvarchar(100);
declare @HardRuleName   nvarchar(450);
declare @ComputerGUID   nvarchar(50);
declare @InspectionGUID  nvarchar(50);
declare @RuleNameList nvarchar(max);
declare @HardRuleList nvarchar(max);
declare @RuleName nvarchar(450);
declare @HardRule nvarchar(450);
declare @I int;
declare @K_Max int;
declare @K int;
declare @N int;
declare @L int;
declare @MaxID int;
declare @CountSoft int;
declare @CountHard int;
declare @CountRule int;
declare @InspectionType  int;


SET DATEFORMAT ymd;

BEGIN TRY


CREATE TABLE #Computer(
	                      [ID] [int] IDENTITY(1,1) NOT NULL,
                          [InfoType] [int] NULL,  --  with additional hardware data  0 \ without it  1     
	                     [ComputerGUID] [nvarchar](50)   NULL,
                         [ComputerName] [nvarchar](450) collate Cyrillic_General_CI_AS  NULL,
                         [AbsentSoftware] [nvarchar](max) collate Cyrillic_General_CI_AS  NULL,
                         [AbsentHardware] [nvarchar](max) collate Cyrillic_General_CI_AS  NULL
                     ) ON [PRIMARY]; 

CREATE TABLE #AbsentSoft(  
	                     [RuleName] [nvarchar](450) collate Cyrillic_General_CI_AS  NULL
                     ) ON [PRIMARY]; 

CREATE TABLE #AbsentHard(  
	                     [HardRuleName] [nvarchar](450) collate Cyrillic_General_CI_AS  NULL
                     ) ON [PRIMARY]; 

CREATE TABLE #LicenseRule(  
                          [LicenseRuleGUID] [nvarchar](50)   NULL, 
                          [RuleName]  [nvarchar](450) collate Cyrillic_General_CI_AS  NULL, 
                          [Publisher]  [nvarchar](450) collate Cyrillic_General_CI_AS  NULL, 
	                      [ProductName] [nvarchar](450) collate Cyrillic_General_CI_AS  NULL,
                          [Localization] [nvarchar](500) collate Cyrillic_General_CI_AS  NULL,
                          [ProductVersion] [nvarchar](500) collate Cyrillic_General_CI_AS  NULL,
                          [ProductKey] [nvarchar](500) collate Cyrillic_General_CI_AS  NULL,
                          [SoftType]  smallint NULL,    
                     ) ON [PRIMARY]; 

CREATE TABLE #HardwareLicenseRule(  
                         [HardwareLicenseRuleGUID] [nvarchar](50)   NULL,
                         [RuleName]  [nvarchar](450) collate Cyrillic_General_CI_AS  NULL, 
                         [Class] [nvarchar](450) collate Cyrillic_General_CI_AS  NULL,
	                     [DeviceDesc] [nvarchar](450) collate Cyrillic_General_CI_AS  NULL,  
                         [MinMemory] [int] NULL,
						 [MaxMemory] [int] NULL,
						 [MinFreeMemory] [int] NULL,
						 [MaxFreeMemory] [int] NULL,
						 [MinFrequency] [int] NULL,
						 [MaxFrequency] [int] NULL,
						 [MinQuantity] [int]  NULL,
						 [MaxQuantity] [int]  NULL,
						 [ReleaseDateFrom] [datetime]  NULL,
						 [ReleaseDateTo] [datetime]  NULL,
						 [Version] [nvarchar](300) collate Cyrillic_General_CI_AS  NULL,
						 [Service] [nvarchar](300) collate Cyrillic_General_CI_AS  NULL,
                         [BusWidth] [nvarchar](300) collate Cyrillic_General_CI_AS  NULL,
						 [BusLength] [nvarchar](300) collate Cyrillic_General_CI_AS  NULL,
						 [Chipset] [nvarchar](300) collate Cyrillic_General_CI_AS  NULL,
						 [Socket] [nvarchar](300) collate Cyrillic_General_CI_AS  NULL
                     ) ON [PRIMARY]; 

CREATE TABLE #ComputerPassport(  
                         [PassportGUID] [nvarchar](50)   NULL,
                     ) ON [PRIMARY]; 

---  all computers with passports
insert into #Computer 
select distinct 0, C.ObjectGUID,  C.Name, '', ''  
from Computer C
-- computers with passports
inner join  PassportLink  PL  on C.ObjectGUID = PL.ParentGUID
-- computers with success inspections
inner join (select distinct ParentGUID from Action
            where ActionType = 1 AND ResultCode = 1 ) S
on C.ObjectGUID   = S.ParentGUID          
-- definite subdivision
inner join   Subdivision D on C.SubdivisionGUID = D.ObjectGUID  
where D.Name = @SubdivisionName;          

select @K_Max = count(*) from   #Computer;

set @K = 1;
        
while (@K <= @K_Max)
begin
            select @ComputerGUID  = ComputerGUID from #Computer
            where ID = @K;
               
            --  computer passports
            INSERT    into #ComputerPassport
			select PassportGUID from PassportLink where  ParentGUID = @ComputerGUID; 
    
            ----  last success inspection for  this computer
            select @InspectionGUID  = InspectionGUID, @InspectionType = InspectionType 
            FROM     ViewMaxComputerAction   WHERE ComputerGUID = @ComputerGUID;

			----------- LicenseRule for the computer
			INSERT  into #LicenseRule
			select  LR.ObjectGUID, LR.Name AS RuleName, LR.Publisher, LR.ProductName,  
                    LR.Localization, LR.ProductVersion, LR.ProductKey, LR.SoftType
			from    LicenseRule LR, LicenseRuleLink  LRL, #ComputerPassport P
			where   LR.ObjectGUID = LRL.LicenseRuleGUID and LRL.ParentGUID  = P.PassportGUID;

            ----   HardwareLicenseRule for the computer
			INSERT    into #HardwareLicenseRule
			select   HLR.ObjectGUID, HLR.Name AS RuleName,  HLR.Class, HLR.DeviceDesc, HLR.MinMemory, HLR.MaxMemory, HLR.MinFreeMemory,
					   HLR.MaxFreeMemory, HLR.MinFrequency, HLR.MaxFrequency, HLR.MinQuantity,    HLR.MaxQuantity,
					   HLR.ReleaseDateFrom, HLR.ReleaseDateTo, HLR.Version, HLR.Service,
                       HLR.BusWidth, HLR.BusLength, HLR.Chipset, HLR.Socket
			from HardwareLicenseRule HLR,  HardwareLicenseRuleLink  HLRL, #ComputerPassport P
			where HLR.ObjectGUID = HLRL.HardwareLicenseRuleGUID and HLRL.ParentGUID  =  P.PassportGUID; 

            -- rules with additional hardware data, which only agent inspection have
             select  @CountRule = Count(*) from  #HardwareLicenseRule
			 where (MinMemory        <> 0        or MaxMemory         <> 0)
				  or  (MinFreeMemory <> 0        or MaxFreeMemory  <> 0)   
				  or  (MinFrequency     <> 0        or MaxFrequency      <> 0) 
				  or  (MinQuantity        <> 0        or MaxQuantity         <> 0);   

            --  if exist rule with additional hardware data and inspection from reestr  =>  set computer to undefined 
            if (@InspectionType = 0 and  @CountRule > 0)
            begin
                      update #Computer
					 set InfoType = 1 
					 where ID = @K;    --   there are no additional data for hardware
            end 
            else   
            begin
						INSERT    into #AbsentSoft
						select  distinct LRule.RuleName  from  #LicenseRule LRule
                        where LRule.LicenseRuleGUID not in
                        ( select LR.LicenseRuleGUID from    #LicenseRule  LR,    
										(SELECT  P.Name as Publisher, STP.ProductName,  S.ProductKey, dbo.GetLanguage(S.Localization) AS Language, 
														 dbo.GetFullVersion(STP.VersionMajor,  S.VersionMinor, S.VersionBuildMajor, S.VersionBuildMinor) AS Version, S.IsOperatingSystem 
										  FROM     Soft  S,  Installation  I , SoftType STP, Publisher P 
										WHERE   P.ObjectGUID = STP.PublisherGUID and STP.ObjectGUID = S.SoftTypeGUID and S.ObjectGUID = I.SoftGUID 
                                                and  I.ParentGUID = @InspectionGUID and S.IsVisible = 1) SC    --ПО на компьютере
						WHERE (SC.Publisher like LR.Publisher) AND (SC.Version like LR.ProductVersion) 
						AND (SC.Language like LR.Localization) AND   (SC.ProductName like LR.ProductName) and (SC.ProductKey like LR.ProductKey)
						AND SC.IsOperatingSystem = LR.SoftType);


						INSERT    into #AbsentHard
						select  distinct HLRule.RuleName  from #HardwareLicenseRule HLRule
                        where HLRule.HardwareLicenseRuleGUID not in
								 (         select HLR.HardwareLicenseRuleGUID from
										   #HardwareLicenseRule HLR,    --Hardware  в паспорте
															(SELECT  H.Class, H.DeviceDesc, H.Memory, H.Frequency, H.Quantity, HI.FreeMemory, HI.ReleaseDate, 
                                                                     H.Version, H.Service, H.BusWidth, H.BusLength, H.Chipset, H.Socket
															  FROM     Hardware H,  HardInstallation  HI  
															WHERE   H.ObjectGUID = HI.HardwareGUID and  HI.ParentGUID  = @InspectionGUID and H.IsVisible = 1) HC    --Оборудование на компьютере
								    		WHERE (HC.Class like HLR.Class) AND (HC.DeviceDesc like HLR.DeviceDesc)  
													  AND 
                                                      (  
															   (HC.Class NOT IN ('MEMORY', 'DISPLAY', 'DISKDRIVE', 'PROCESSOR', 'BIOS', 'USB', 'SYSTEMSLOT', 'MOTHERBOARD') )  
															   OR
															   (     ( (HC.Class = 'MEMORY') OR (HC.Class = 'DISPLAY') )  
																	AND  ( (HC.Memory <= HLR.MaxMemory) OR HLR.MaxMemory = 0)  AND  ((HC.Memory >= HLR.MinMemory) OR  HLR.MinMemory = 0)
															   ) 
															   OR
															   (  (HC.Class = 'PROCESSOR') AND     
																		   ( (HC.Frequency <= HLR.MaxFrequency)  OR HLR.MaxFrequency = 0 )  AND  ((HC.Frequency >= HLR.MinFrequency)  OR HLR.MinFrequency = 0)
																			   AND ((HC.Quantity <= HLR.MaxQuantity) OR HLR.MaxQuantity = 0 )   AND     ( (HC.Quantity >= HLR.MinQuantity)  OR HLR.MinQuantity = 0 )
															   )  
															   OR
															   (    (HC.Class = 'DISKDRIVE')  AND
																		 (  (HC.Memory <= HLR.MaxMemory)  OR HLR.MaxMemory = 0 )  AND  ((HC.Memory >= HLR.MinMemory)  OR HLR.MinMemory = 0)
																		AND  ((HC.FreeMemory <= HLR.MaxFreeMemory)  OR HLR.MaxFreeMemory = 0 ) AND ( (HC.FreeMemory >= HLR.MinFreeMemory)  OR HLR.MinFreeMemory = 0 ) 
																)
																 OR (  HC.Class = 'BIOS' 
																		   AND (HC.ReleaseDate <= ReleaseDateTo OR HLR.ReleaseDateTo IS NULL) 
																		   AND (HC.ReleaseDate >= ReleaseDateFrom OR HLR.ReleaseDateFrom IS NULL) 
																		   AND (HC.Version LIKE HLR.Version)
																		) 
																 OR (  HC.Class = 'USB'  
																		   AND (HC.Service LIKE HLR.Service)
																	  ) 
																 OR (  HC.Class = 'SYSTEMSLOT'  
																		   AND (HC.BusWidth LIKE HLR.BusWidth)   AND (HC.BusLength LIKE HLR.BusLength)
																	  ) 
																 OR (  HC.Class = 'MOTHERBOARD'  
																		   AND (HC.Chipset LIKE HLR.Chipset) AND (HC.Version LIKE HLR.Version)  AND (HC.Socket LIKE HLR.Socket)
																	)      
													)
                                     );
                         



						select @CountSoft = Count(*) from #AbsentSoft;
						select @CountHard = Count(*) from #AbsentHard;

						if ((@CountSoft = 0) and (@CountHard = 0))
						begin
								 delete from #Computer where ID = @K;
						end
						else
						begin
								  if(@CountSoft > 0)
								  begin
												  set @RuleNameList = '';
												  while exists (select top(1) * from #AbsentSoft)
												  begin
														  select @RuleName = RuleName from #AbsentSoft; 
														  set @RuleNameList = @RuleNameList + @RuleName + '; ' ;
														  delete from   #AbsentSoft where RuleName = @RuleName;
												  end

												  set @RuleNameList = RTRIM(@RuleNameList);
												  set @L = LEN(@RuleNameList); 
												  if (@L >0)
												  begin 
														 set @RuleNameList = LEFT(@RuleNameList, @L - 1);
												  end
								                  
												  update #Computer
												  set AbsentSoftware = @RuleNameList
												  where ID = @K;
				            	end 

							   if(@CountHard > 0)
							   begin
											  set @HardRuleList = '';
											  while exists (select top(1) * from #AbsentHard)
											  begin
													  select @HardRule = HardRuleName from #AbsentHard; 
													  set @HardRuleList = @HardRuleList + @HardRule + '; ' ;
													  delete from   #AbsentHard where HardRuleName = @HardRule;
											  end

											 set @HardRuleList = RTRIM(@HardRuleList);
											 set @L = LEN(@HardRuleList); 
											 if (@L >0)
											 begin 
													 set @HardRuleList = LEFT(@HardRuleList, @L - 1);
											 end

											  update #Computer
											  set AbsentHardware = @HardRuleList
											  where ID = @K;
							  end 
                end

      end

       delete from #ComputerPassport;
       delete from #AbsentSoft;
       delete from #AbsentHard;
       delete from #LicenseRule;
       delete from #HardwareLicenseRule;
	   set @K = @K + 1;

end

select *   from #Computer
order by ComputerName;

END TRY
BEGIN CATCH

    Set @Message = Error_Message();
    RAISERROR(@Message, 12, 1);
    Return -1;

END CATCH

END
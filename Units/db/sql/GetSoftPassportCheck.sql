CREATE FUNCTION [dbo].[GetSoftPassportCheck](@InspectionGUID nvarchar(50), @ProductName nvarchar(450), @Publisher nvarchar(450), @Localization nvarchar(50),
                                                                         @VersionMinor nvarchar(50), @VersionMajor nvarchar(50), @ProductKey nvarchar(500),
                                                                          @SoftType int, @VersionBuildMinor nvarchar(150), @VersionBuildMajor nvarchar(150))
RETURNS  smallint
AS
BEGIN

declare @PassportCheck int;
declare @ComputerGUID nvarchar(50);
declare @PassportCount smallint;
declare @ProductCount int;

set @PassportCheck = 2;  -- blue


--  white list

select  @ComputerGUID = ParentGUID from Action
where ObjectGUID = @InspectionGUID;

select @PassportCount = Count(*) from PassportLink where ParentGUID = @ComputerGUID;

if (@PassportCount > 0)
begin

        select @ProductCount  = Count(*) from 
		(select   LR.ProductName, LR.Publisher, LR.ProductVersion, LR.Localization, LR.ProductKey, LR.SoftType
		from LicenseRule LR,  LicenseRuleLink  LRL, PassportLink PL
		where LR.ObjectGUID = LRL.LicenseRuleGUID and LRL.ParentGUID  = PL.PassportGUID and PL.ParentGUID = @ComputerGUID) S
		where  (@ProductName like S.ProductName) and  (@Publisher like S.Publisher) and (@ProductKey like S.ProductKey) 
                    and   (@SoftType = S.SoftType) and  (dbo.GetLanguage(@Localization)  like S.Localization)
				    and 	(dbo.GetFullVersion(@VersionMajor, @VersionMinor, @VersionBuildMajor, @VersionBuildMinor) like S.ProductVersion) 

        if (@ProductCount > 0)
		begin
				 set @PassportCheck = 1;     -- green 
		end																

end

--  black list
		
select @ProductCount  = Count(*) from ProhibitionRule PR
where (@ProductName like PR.ProductName) and  (@Publisher like PR.Publisher) and (@ProductKey like PR.ProductKey) 
            and   (@SoftType = PR.SoftType) and  (dbo.GetLanguage(@Localization)  like PR.Localization)
		    and 	(dbo.GetFullVersion(@VersionMajor, @VersionMinor, @VersionBuildMajor, @VersionBuildMinor) like PR.ProductVersion) 

if (@ProductCount > 0)
begin
		 set @PassportCheck = 0;   -- red
end


RETURN @PassportCheck;

END

